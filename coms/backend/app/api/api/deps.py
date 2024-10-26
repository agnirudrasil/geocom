import asyncio
import functools
import json
import operator
from datetime import datetime
from json import JSONDecodeError
from typing import List, Union, TypeVar, Type, Generator

from fastapi import Depends, HTTPException, status, Request, Header, UploadFile
from fastapi.exceptions import RequestValidationError
from fastapi.security import OAuth2AuthorizationCodeBearer, SecurityScopes
from jose import jwt
from pydantic import ValidationError, BaseModel
from sqlalchemy.orm import Session

from api import models, schemas, crud
from api.core import security
from api.core.compute_permissions import compute_overwrites
from api.core.config import settings
from api.core.permissions import Permissions
from api.db.session import SessionLocal
from api.schemas.oauth2 import OAUTH2_SCOPES
from api.utils.attachment import file_to_attachment

reusable_oauth2 = OAuth2AuthorizationCodeBearer(
    tokenUrl=f"{settings.FRONTEND_URL}/oauth2/token",
    authorizationUrl=f"{settings.API_V1_STR}/oauth2/authorize",
    refreshUrl=f"{settings.API_V1_STR}/oauth2/token",
    scopes=OAUTH2_SCOPES,
)


def ensure_header(content_type: str = Header(...)):
    if content_type != "application/x-www-form-urlencoded":
        raise HTTPException(status.HTTP_415_UNSUPPORTED_MEDIA_TYPE,
                            f"Unsupported media type {content_type}. It must be application/x-www-form-urlencoded")
    return


def get_db() -> Generator[Session, None, None]:
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()


def get_oauth2_user(security_scopes: SecurityScopes, db: Session = Depends(get_db),
                    token: str = Depends(reusable_oauth2)) -> tuple[models.User, str]:
    authenticate_value = f'Bearer scope="{security_scopes.scope_str}"'
    try:
        user = get_current_user(db, token)
        return user, "all"
    except HTTPException:
        token: models.Token = db.query(
            models.Token).filter_by(access_token=token).first()
        if token and not token.is_expired():
            for scope in security_scopes.scopes:
                if scope not in token.scope.split(" "):
                    raise HTTPException(
                        status_code=status.HTTP_401_UNAUTHORIZED,
                        detail="Not enough permissions",
                        headers={"WWW-Authenticate": authenticate_value},
                    )
            return token.user, token.scope
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Could not validate credentials",
                            headers={"WWW-Authenticate": authenticate_value})
    pass


def get_current_user(
        db: Session = Depends(get_db), token: str = Depends(reusable_oauth2)
) -> models.User:
    try:
        payload = jwt.decode(
            token, settings.SECRET_KEY, algorithms=[security.ALGORITHM]
        )
        token_data = schemas.TokenPayload(**payload)
    except (jwt.JWTError, ValidationError):
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Could not validate credentials",
        )

    user = crud.user.get(db, id=token_data.sub)

    if not user:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Could not validate credentials",
        )

    if not user.last_login:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Token has expired",
        )
    if user.mfa_enabled != token_data.mfa:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Token has expired",
        )
    if user.last_login.replace(microsecond=0) > datetime.fromtimestamp(token_data.iat):
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Token has expired",
        )
    if not user:
        raise HTTPException(
            status_code=403, detail="Could not validate credentials")

    return user


class ChannelPerms:
    def __init__(self, permission: Union[Permissions, List[Permissions]]) -> None:
        if isinstance(permission, list):
            self.permission = functools.reduce(operator.or_, permission)
        else:
            self.permission = permission

    async def is_valid(self, channel: models.Channel, current_user: models.User, db: Session) -> bool:
        if not channel:
            return False

        if not channel.guild_id:
            channel = db.query(models.Channel).join(models.channels.ChannelMembers).filter(
                models.channels.ChannelMembers.user_id == current_user.id,
                models.Channel.id == channel.id).first()
            if channel:
                return True
            return False

        guild_member: models.GuildMembers = db.query(models.GuildMembers).filter_by(
            guild_id=channel.guild_id).filter_by(
            user_id=current_user.id).first()

        if not guild_member:
            return False

        if guild_member.is_owner:
            return True

        if guild_member.permissions & Permissions.ADMINISTRATOR == Permissions.ADMINISTRATOR:
            return True

        permissions = await compute_overwrites(guild_member.permissions, channel, guild_member, db)

        if not permissions & self.permission == self.permission:
            return False

        return True

    async def __call__(
            self,
            channel_id: int,
            current_user: models.User = Depends(get_current_user),
            db: Session = Depends(get_db),
    ) -> tuple[models.Channel, models.User]:
        channel: models.Channel = db.query(
            models.Channel).filter_by(id=channel_id).first()
        if not channel:
            raise HTTPException(status_code=404, detail="Channel Not Found")
        if not await self.is_valid(channel, current_user, db):
            raise HTTPException(status_code=403, detail="Not Authorized")
        return channel, current_user


class InvitePerms:
    def __init__(self, permission: Permissions) -> None:
        self.permission = permission

    async def __call__(
            self,
            request: Request,
            current_user: models.User = Depends(get_current_user),
            db: Session = Depends(get_db),
    ) -> models.Invite:
        code = request.path_params["code"]
        invite: models.Invite = db.query(
            models.Invite).filter_by(id=code).first()
        if not invite:
            raise HTTPException(status_code=404, detail="Invite not found")
        channels = invite.channel
        if channels.guild_id:
            guild_member: models.GuildMembers = db.query(models.GuildMembers).filter_by(
                user_id=current_user.id, guild_id=channels.guild_id
            ).first()
            if guild_member:
                permissions = await compute_overwrites(
                    guild_member.permissions, channels, guild_member, db)
                if permissions & self.permission == self.permission:
                    return invite
        if channels.is_member(current_user.id):
            return invite
        raise HTTPException(
            status_code=403, detail="You do not have permission to delete this invite")


class GuildPerms:
    def __init__(self, permission: Permissions) -> None:
        self.permission = permission

    async def __call__(
            self,
            guild_id: int,
            current_user: models.User = Depends(get_current_user),
            db: Session = Depends(get_db),
    ) -> tuple[models.Guild, models.User, models.GuildMembers]:
        guild: models.Guild = db.query(
            models.Guild).filter_by(id=guild_id).first()
        if not guild:
            raise HTTPException(status_code=404, detail="Guild not found")

        member: models.GuildMembers = db.query(models.GuildMembers).filter_by(guild_id=guild_id).filter_by(
            user_id=current_user.id).first()

        if not member:
            raise HTTPException(status_code=403, detail="Not Authorized")

        if member.is_owner:
            return guild, current_user, member

        if member.permissions & Permissions.ADMINISTRATOR == Permissions.ADMINISTRATOR:
            return guild, current_user, member

        if not member.permissions & self.permission == self.permission:
            raise HTTPException(status_code=403, detail="Not Authorized")

        return guild, current_user, member


BodyType = TypeVar("BodyType", bound=BaseModel)


class ExtractBody:
    def __init__(self, body_type: Type[BodyType]) -> None:
        self.body_type = body_type

    async def __call__(self, request: Request, content_type: str = Header(...), db: Session = Depends(get_db)) -> \
            tuple[BodyType, list]:
        attachments = []
        channel_id = request.path_params.get("channel_id", request.path_params.get("webhook_id"))
        if "channel_id" not in request.path_params:
            webhook = db.query(models.Webhook).filter_by(id=channel_id).first()
            channel_id = webhook.channel_id
        body = self.body_type()
        if content_type.startswith('multipart/form-data'):
            form_body = await request.form()
            files: list[UploadFile] = form_body.getlist("files")
            if files:
                tasks = []
                for file in files:
                    tasks.append(file_to_attachment(file, channel_id))
                attachments = await asyncio.gather(*tasks)

            payload_json: UploadFile = form_body.get('payload_json')
            if payload_json and payload_json.content_type == "application/json":
                try:
                    payload = await payload_json.read(1024 * 1024 * 1)
                    body = self.body_type(**json.loads(payload))
                except JSONDecodeError:
                    raise HTTPException(status_code=400, detail='Invalid JSON')
                except ValidationError as e:
                    raise HTTPException(status_code=400, detail=e.json())
        elif content_type.startswith('application/json'):
            try:
                json_body = await request.json()
                body = self.body_type(**json_body)

            except ValidationError as e:
                raise RequestValidationError(errors=e.raw_errors)

        else:
            raise HTTPException(detail="Invalid content type",
                                status_code=status.HTTP_415_UNSUPPORTED_MEDIA_TYPE)

        return body, attachments
