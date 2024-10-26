# from datetime import datetime, timedelta
import re
from typing import Optional

from fastapi import APIRouter, Depends, Response, Security, HTTPException
from pydantic import BaseModel
from sqlalchemy import asc, desc, func
from sqlalchemy.exc import IntegrityError
from sqlalchemy.orm import Session
from werkzeug.utils import secure_filename

from api.api import deps
from api.core import emitter
from api.core.events import Events, websocket_emitter
from api.core.permissions import Permissions
from api.models import Emoji
from api.models.channels import Channel, ChannelType, Overwrite
from api.models.guilds import Guild, GuildBans, GuildMembers
from api.models.roles import Role
from api.models.user import User
from api.schemas.channel import ChannelValidate
from api.utils.validate_avatar import validate_avatar
from api.utils.validate_emoji import validate_emoji

router = APIRouter()


class CreateGuildBan(BaseModel):
    reason: Optional[str] = None
    delete_message_days: Optional[int] = 0


class GuildMemberUpdate(BaseModel):
    nick: Optional[str] = None


class RoleCreate(BaseModel):
    name: Optional[str] = 'new role'
    permissions: Optional[int] = 0
    color: Optional[int] = 0
    mentionable: Optional[bool] = False


class RoleUpdate(BaseModel):
    name: Optional[str]
    color: Optional[int]
    permissions: Optional[int]
    mentionable: Optional[bool]


class RolePositionUpdate(BaseModel):
    id: int
    position: int


class GuildEdit(BaseModel):
    name: str
    icon: Optional[str] = None


class GuildCreate(BaseModel):
    name: str
    icon: Optional[str] = None


@router.post('/')
async def create_guild(
        body: GuildCreate,
        current_user: User = Depends(deps.get_current_user),
        db: Session = Depends(deps.get_db)):
    guild = Guild(body.name, current_user.id)
    if body.icon:
        icon_hash = await validate_avatar(guild.id, body.icon, "icons")
        guild.icon = icon_hash
    guild_member = GuildMembers(is_owner=True)
    role = Role(role_id=guild.id, name='@everyone', color=0, position=0, permissions=1071698660929, mentionable=True,
                guild_id=guild.id)
    category = Channel(ChannelType.guild_category,
                       guild.id, 'TEXT CHANNELS')
    general = Channel(ChannelType.guild_text,
                      guild.id, 'general', parent_id=category.id)
    guild.channels.append(category)
    guild.channels.append(general)
    guild_member.member = current_user
    guild.members.append(guild_member)
    db.add(guild)
    db.add(role)
    db.commit()
    await emitter.in_room(str(current_user.id)).sockets_join(str(guild.id))
    await websocket_emitter(channel_id=None, guild_id=None, event=Events.GUILD_CREATE,
                            args={"guild": guild.serialize(), "member": guild_member.serialize()},
                            user_id=current_user.id, db=db)
    return guild.preview()


@router.get('/{guild_id}')
async def get_guild(guild_id: int,
                    current_user: User = Depends(deps.get_current_user),
                    db: Session = Depends(deps.get_db)):
    guild: Guild = db.query(Guild).filter_by(id=guild_id).first()
    if guild and guild.is_member(db, current_user.id):
        return guild.serialize()
    return Response(status_code=404)


@router.get('/{guild_id}/preview')
async def get_guild(guild_id: int,
                    current_user: User = Depends(deps.get_current_user),
                    db: Session = Depends(deps.get_db)):
    guild: Guild = db.query(Guild).filter_by(id=guild_id).first()
    if guild and guild.is_member(db, current_user.id):
        return guild.preview()
    return Response(status_code=404)


@router.patch('/{guild_id}')
async def edit_guild(body: GuildEdit,
                     db: Session = Depends(deps.get_db),
                     dependencies: tuple[Guild, User, GuildMembers] = Depends(
                         deps.GuildPerms(Permissions.MANAGE_GUILD))):
    guild, user, member = dependencies
    guild.name = body.name
    body_dict = body.dict(exclude_unset=True)
    if 'icon' in body_dict:
        icon_hash = None
        if body_dict['icon']:
            icon_hash = await validate_avatar(guild.id, body.icon, "icons")
        guild.icon = icon_hash
    db.commit()
    await websocket_emitter(channel_id=None, guild_id=guild.id, event=Events.GUILD_UPDATE, args=guild.serialize(),
                            db=db)
    return guild.serialize()


@router.delete('/{guild_id}', status_code=204)
async def delete_guild(guild_id: int,
                       response: Response,
                       db: Session = Depends(deps.get_db),
                       user: User = Depends(deps.get_current_user)):
    guild: Guild = db.query(Guild).filter_by(id=guild_id).first()
    if guild and guild.is_owner(user.id):
        await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_DELETE, args=guild.serialize(),
                                db=db)
        db.delete(guild)
        db.commit()
        return
    response.status_code = 404
    return {"error": "Guild not found"}


@router.put('/{guild_id}/members/{user_id}/roles/{role_id}', status_code=204)
async def add_user_role(guild_id: int,
                        user_id: int,
                        role_id: int,
                        dependencies: tuple[Guild, User, GuildMembers] = Depends(
                            deps.GuildPerms(Permissions.MANAGE_ROLES)),
                        db: Session = Depends(deps.get_db)):
    *_, member = dependencies
    guild_member = db.query(GuildMembers).filter_by(
        user_id=user_id).filter_by(guild_id=guild_id).first()
    if guild_member:
        role = db.query(Role).filter_by(id=role_id).first()
        if member.is_owner or (member.roles and role.position < member.roles[0].position):
            guild_member.roles.append(role)
            db.commit()

            await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_MEMBER_UPDATE,
                                    args=guild_member.serialize(), db=db)
            return ''
        return Response(status_code=403)
    return Response(status_code=404)


@router.delete('/{guild_id}/members/{user_id}/roles/{role_id}', status_code=204,
               dependencies=[])
async def delete_user_role(guild_id: int,
                           user_id: int,
                           role_id: int,
                           dependencies: tuple[Guild, User, GuildMembers] = Depends(
                               deps.GuildPerms(Permissions.MANAGE_ROLES)),
                           db: Session = Depends(deps.get_db)):
    *_, member = dependencies
    guild_member = db.query(GuildMembers).filter_by(
        user_id=user_id, guild_id=guild_id).first()
    if guild_member:
        try:
            role = db.query(Role).filter_by(id=role_id).first()
            if member.is_owner or (member.roles and role.position < member.roles[0].position):
                guild_member.roles.remove(role)
                db.commit()
                await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_MEMBER_UPDATE,
                                        args=guild_member.serialize(), db=db)
        except ValueError:
            pass
        return ''
    return Response(status_code=404)


@router.get('/{guild_id}/roles')
async def get_roles(guild_id: int,
                    current_user: User = Depends(deps.get_current_user),
                    db: Session = Depends(deps.get_db)):
    guild = db.query(Guild).filter_by(id=guild_id).first()
    if guild and guild.is_member(db, current_user.id):
        roles = db.query(Role).filter_by(guild_id=guild_id).order_by(
            desc(Role.position)).order_by(asc(Role.id)).all()
        return [role.serialize() for role in roles]
    return Response(status_code=404)


@router.get('/{guild_id}/roles/{role_id}')
async def get_role(guild_id: int,
                   role_id: int,
                   current_user: User = Depends(deps.get_current_user),
                   db: Session = Depends(deps.get_db)):
    guild = db.query(Guild).filter_by(id=guild_id).first()
    if guild and guild.is_member(db, current_user.id):
        role = db.query(Role).filter_by(
            guild_id=guild_id).filter_by(id=role_id).first()
        return role.serialize()
    return Response(status_code=404)


@router.get('/{guild_id}/roles/{role_id}/members', dependencies=[Depends(deps.GuildPerms(Permissions.MANAGE_ROLES))])
async def get_role_members(guild_id: int,
                           role_id: int,
                           db: Session = Depends(deps.get_db)):
    members = db.query(GuildMembers).filter_by(guild_id=guild_id).filter(
        GuildMembers.roles.any(Role.id == role_id)).all()
    return [member.serialize() for member in members]


@router.post('/{guild_id}/roles', dependencies=[])
async def create_role(guild_id: int,
                      data: RoleCreate,
                      dependencies: tuple[Guild, User, GuildMembers] = Depends(
                          deps.GuildPerms(Permissions.MANAGE_ROLES)),
                      db: Session = Depends(deps.get_db)):
    *_, member = dependencies
    everyone_role = db.query(Role).filter_by(id=guild_id).first()
    if member.is_owner:
        position = db.query(func.max(Role.position)).filter_by(
            guild_id=guild_id).first()[0] + 1
    else:
        position = max(member.roles[0].position - 1, 1) if member.roles else 1

    role = Role(guild_id, data.name, data.color, position,
                data.permissions or everyone_role.permissions,
                data.mentionable)
    db.query(Role).filter_by(guild_id=guild_id).filter(
        Role.position >= position).update({Role.position: Role.position + 1})
    updated_roles = db.query(Role).filter_by(
        guild_id=guild_id).filter(Role.position >= position + 1).all()
    db.add(role)
    db.commit()
    await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_ROLE_CREATE,
                            args={'role': role.serialize(), 'guild_id': str(guild_id)}, db=db)
    for role in updated_roles:
        await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_ROLE_UPDATE,
                                args={'role': role.serialize(), 'guild_id': str(guild_id)}, db=db)
    return role.serialize()


# TODO: Add code to update role order
@router.patch('/{guild_id}/roles', status_code=204, dependencies=[])
async def update_role_positions(guild_id: int,
                                data: RolePositionUpdate,
                                dependencies: tuple[Guild, User, GuildMembers] = Depends(
                                    deps.GuildPerms(Permissions.MANAGE_ROLES)),
                                db: Session = Depends(deps.get_db)):
    *_, member = dependencies
    if data.position:
        role = db.query(Role).filter_by(
            id=data.id).filter_by(guild_id=guild_id).first()
        if not member.is_owner and member.roles[0].position < role.position:
            return Response(status_code=403)
        if not member.is_owner and data.position > member.roles[0].position:
            return Response(status_code=403)
        original_position = role.position
        max_position = db.query(func.max(Role.position)).filter_by(
            guild_id=guild_id).first()[0]
        if data.position > max_position:
            data.position = max_position
        if role:
            if role.position == data.position:
                return
            if role.position < data.position:
                db.query(Role).filter_by(guild_id=guild_id).filter(Role.position <= data.position,
                                                                   Role.position > role.position).update(
                    {Role.position: Role.position - 1})
            else:
                db.query(Role).filter_by(guild_id=guild_id).filter(Role.position >= data.position,
                                                                   Role.position < role.position).update(
                    {Role.position: Role.position + 1})
            role.position = data.position
            db.commit()
            update_roles = db.query(Role).filter_by(guild_id=guild_id).filter(Role.position <= data.position,
                                                                              Role.position >= original_position) \
                .all() if original_position < data.position else db.query(
                Role).filter_by(guild_id=guild_id).filter(Role.position >= data.position,
                                                          Role.position <= original_position).all()
            for role in update_roles:
                await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_ROLE_UPDATE,
                                        args={'role': role.serialize(), 'guild_id': str(guild_id)}, db=db)
            return
        return {'success': False, 'error': 'Role not found'}, 404
    return {'success': False, 'error': 'No position provided'}, 404


@router.patch('/{guild_id}/roles/{role_id}', dependencies=[])
async def update_role(guild_id: int, role_id: int,
                      data: RoleUpdate,
                      dependencies: tuple[Guild, User, GuildMembers] = Depends(
                          deps.GuildPerms(Permissions.MANAGE_ROLES)),
                      db: Session = Depends(deps.get_db)):
    *_, member = dependencies
    role = db.query(Role).filter_by(
        id=role_id).filter_by(guild_id=guild_id).first()
    if not member.is_owner and member.roles[0].position <= role.position:
        return Response(status_code=403)
    if data.name:
        role.name = data.name
    if data.color:
        role.color = data.color
    if data.permissions:
        extra_permissions = data.permissions & ~role.permissions
        if member.is_owner or (member.permissions & Permissions.ADMINISTRATOR == Permissions.ADMINISTRATOR) or (
                (member.permissions & extra_permissions) == extra_permissions):
            role.permissions = int(data.permissions)
        else:
            return Response(status_code=403)
    if data.mentionable:
        role.mentionable = data.mentionable
    db.commit()
    await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_ROLE_UPDATE,
                            args={'role': role.serialize(), 'guild_id': str(guild_id)}, db=db)
    return role.serialize()


@router.delete('/{guild_id}/roles/{role_id}', dependencies=[])
async def delete_role(guild_id: int, role_id: int,
                      dependencies: tuple[Guild, User, GuildMembers] = Depends(
                          deps.GuildPerms(Permissions.MANAGE_ROLES)),
                      db: Session = Depends(deps.get_db)):
    *_, member = dependencies
    role = db.query(Role).filter_by(id=role_id, guild_id=guild_id).first()
    if not member.is_owner and member.roles[0].position <= role.position:
        return Response(status_code=403)
    if role:
        position = role.position
        db.delete(role)
        db.query(Role).filter_by(guild_id=guild_id).filter(Role.position > position).update(
            {Role.position: Role.position - 1})
        await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_ROLE_DELETE,
                                args={'role': role.serialize(), 'guild_id': str(guild_id)}, db=db)
        update_roles = db.query(Role).filter_by(
            guild_id=guild_id).filter(Role.position >= position).all()
        for role in update_roles:
            await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_ROLE_UPDATE,
                                    args={'role': role.serialize(), 'guild_id': str(guild_id)}, db=db)
        db.commit()
        return ''
    return Response(status_code=404)


@router.get('/{guild_id}/channels')
async def get_guild_channels(guild_id: int, response: Response,
                             db: Session = Depends(deps.get_db),
                             user: User = Depends(deps.get_current_user)):
    guild = db.query(Guild).filter_by(id=guild_id).first()
    if guild and guild.is_member(db, user.id):
        return [channel.serialize() for channel in guild.channels]
    response.status_code = 404
    return []


@router.post("/{guild_id}/channels", status_code=201,
             dependencies=[Depends(deps.GuildPerms(Permissions.MANAGE_CHANNELS))])
async def create_channel(guild_id: int,
                         data: ChannelValidate,
                         db: Session = Depends(deps.get_db)):
    channel = Channel(data.type, guild_id, data.name,
                      data.topic, data.nsfw,
                      parent_id=data.parent_id)
    if data.privateChannel:
        overwrite = Overwrite(
            guild_id, 0, deny=1024 if data.privateChannel else 0, allow=0)
        channel.overwrites.append(overwrite)
    db.add(channel)
    db.commit()
    await websocket_emitter(channel_id=channel.id, guild_id=guild_id, event=Events.CHANNEL_CREATE,
                            args=channel.serialize(), db=db)
    return channel.serialize()


# TODO change channel positions

@router.get('/{guild_id}/members/@me')
async def get_guild_member(guild_id: int,
                           user: User = Security(deps.get_oauth2_user, scopes=[
                               'guilds.members.read']),
                           db: Session = Depends(deps.get_db)):
    current, scope = user
    member: GuildMembers = db.query(GuildMembers).filter_by(
        guild_id=guild_id).filter_by(user_id=current.id).first()
    if not member:
        raise HTTPException(
            status_code=404, detail='User is not a member of this guild')
    return member.serialize()


@router.get('/{guild_id}/members/{user_id}')
async def get_guild_member(guild_id: int, user_id: int, response: Response,
                           user: User = Depends(deps.get_current_user),
                           db: Session = Depends(deps.get_db)):
    guild: Guild = db.query(Guild).filter_by(id=guild_id).first()
    if guild and guild.is_member(db, user.id):
        member = db.query(GuildMembers).filter_by(
            guild_id=guild_id).filter_by(user_id=user_id).first()
        if member:
            return member.serialize()
        response.status_code = 404
        return
    response.status_code = 404
    return


@router.get('/{guild_id}/members')
async def get_guild_members(guild_id: int, response: Response,
                            limit: int = 50,
                            after: Optional[int] = None,
                            user: User = Depends(deps.get_current_user),
                            db: Session = Depends(deps.get_db)):
    guild: Guild = db.query(Guild).filter_by(id=guild_id).first()
    limit = max(limit, 1)
    limit = min(1000, limit)
    real_limit = limit + 1
    if guild and guild.is_member(db, user.id):
        if after:
            members = db.query(GuildMembers).filter_by(guild_id=guild_id).filter(User.id > after).limit(
                real_limit).all()
            return {"members": [member.serialize() for member in members[0:limit]], "has_more": len(members) > limit}
        members = db.query(GuildMembers).filter_by(
            guild_id=guild_id).limit(real_limit).all()
        return {"members": [member.serialize() for member in members[0:limit]], 'has_more': len(members) > limit}
    response.status_code = 404
    return


@router.put('/{guild_id}/members/{user_id}')
async def add_guild_member(guild_id: int, user_id: int,
                           response: Response,
                           oauth2_user: tuple[User, str] = Security(
                               deps.get_oauth2_user, scopes=['guilds.join']),
                           db: Session = Depends(deps.get_db)):
    current_user, scope = oauth2_user
    guild = db.query(Guild).filter_by(id=guild_id).first()
    if guild:
        member = db.query(GuildMembers).filter_by(
            guild_id=guild_id).filter_by(user_id=user_id).first()
        if member:
            response.status_code = 204
            return
        member = GuildMembers()
        guild.members.append(member)
        db.add(member)
        db.commit()
        await emitter.in_room(str(current_user.id)).sockets_join(str(guild_id))
        await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_CREATE,
                                args={"guild": guild.serialize(
                                ), "member": member.serialize()},
                                user_id=current_user.id, db=db)
        await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_MEMBER_ADD,
                                args=member.serialize(), db=db)
        return member.serialize()


@router.patch('/{guild_id}/members/@me')
async def update_guild_member_me(guild_id: int, data: GuildMemberUpdate,
                                 dependencies: tuple[Guild, User, GuildMembers] = Depends(
                                     deps.GuildPerms(Permissions.CHANGE_NICKNAME)),
                                 db: Session = Depends(deps.get_db)):
    _, user, member = dependencies
    member = db.query(GuildMembers).filter_by(
        guild_id=guild_id).filter_by(user_id=user.id).first()
    if member:
        member.nickname = data.nick
        db.add(member)
        db.commit()
        await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_MEMBER_UPDATE,
                                args=member.serialize(), db=db)
        return member.serialize()
    return Response(status_code=404)


@router.patch('/{guild_id}/members/{user_id}', dependencies=[])
async def update_guild_member(guild_id: int, user_id: int, data: GuildMemberUpdate,
                              dependencies: tuple[Guild, User, GuildMembers] = Depends(
                                  deps.GuildPerms(Permissions.MANAGE_NICKNAMES)),
                              db: Session = Depends(deps.get_db)):
    *_, my_member = dependencies
    member = db.query(GuildMembers).filter_by(
        guild_id=guild_id).filter_by(user_id=user_id).first()
    if member:
        if data.nick:
            if (not member.roles and not member.is_owner) or (not member.is_owner and (
                    my_member.is_owner or my_member.roles[0].position >= member.roles[0].position)):
                member.nickname = data.nick
                db.commit()
                await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_MEMBER_UPDATE,
                                        args=member.serialize(), db=db)
                return member.serialize()
            return Response(status_code=403)
    return Response(status_code=404)


@router.delete('/{guild_id}/members/{user_id}', status_code=204,
               dependencies=[])
async def delete_guild_member(guild_id: int, user_id: int,
                              dependencies: tuple[Guild, User, GuildMembers] = Depends(
                                  deps.GuildPerms(Permissions.KICK_MEMBERS)),
                              db: Session = Depends(deps.get_db)):
    *_, my_member = dependencies
    member: GuildMembers = db.query(GuildMembers).filter_by(
        guild_id=guild_id).filter_by(user_id=user_id).first()
    if member:
        if (not member.roles and not member.is_owner) or (
                not member.is_owner and (
                my_member.is_owner or my_member.roles[0].position >= member.roles[0].position)):
            if member.member.bot:
                await member.member.application.remove_bot_from_guild(db, member.guild)
            else:
                await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_MEMBER_REMOVE,
                                        args=member.serialize(), db=db)
                db.delete(member)
                db.commit()
                await emitter.in_room(str(user_id)).sockets_leave(str(guild_id))
                await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_DELETE,
                                        args={'id': str(guild_id)},
                                        user_id=user_id, db=db)
            return ''
        return Response(status_code=403)
    return Response(status_code=404)


@router.get('/{guild_id}/bans')
async def get_guild_bans(
        dependencies: tuple[Guild, User, GuildMembers] = Depends((deps.GuildPerms(Permissions.BAN_MEMBERS)))):
    guild, *_ = dependencies
    return [ban.serialize() for ban in guild.bans]


@router.get('/{guild_id}/bans/{user_id}', dependencies=[Depends(deps.GuildPerms(Permissions.BAN_MEMBERS))])
async def get_guild_ban(guild_id: int, user_id: int,
                        db: Session = Depends(deps.get_db)):
    ban = db.query(GuildBans).filter_by(
        guild_id=guild_id).filter_by(user_id=user_id).first()
    if ban:
        return ban.serialize()
    return Response(status_code=404)


@router.put('/{guild_id}/bans/{user_id}')
async def add_guild_ban(guild_id: int, user_id: int, body: CreateGuildBan,
                        dependencies: tuple[Guild, User, GuildMembers] = Depends(
                            deps.GuildPerms(Permissions.BAN_MEMBERS)),
                        db: Session = Depends(deps.get_db)):
    guild, user, my_member = dependencies
    try:
        ban = GuildBans(body.reason)
        guild.bans.append(ban)
        member: GuildMembers = db.query(GuildMembers).filter_by(
            guild_id=guild_id).filter_by(user_id=user_id).first()
        if member:
            if (not member.roles and not member.is_owner) or (not member.is_owner and (
                    my_member.is_owner or my_member.roles[0].position >= member.roles[0].position)):

                ban.user = member.member
                if member.member.bot:
                    await member.member.application.remove_bot_from_guild(db, member.guild)
                else:
                    await emitter.in_room(str(member.user_id)).sockets_leave(str(guild_id))
                    await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_MEMBER_REMOVE,
                                            args=member.serialize(), db=db)
                    db.delete(member)
                    await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_DELETE,
                                            args={'id': str(guild_id)},
                                            user_id=user_id, db=db)
            else:
                return Response(status_code=403)
        else:
            user = db.query(User).filter_by(id=user_id).first()
            ban.user = user

        db.add(ban)
        db.commit()
        await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_BAN_ADD, args=ban.serialize(),
                                db=db)
        return ban.serialize()

    except IntegrityError:
        return


@router.delete('/{guild_id}/bans/{user_id}', dependencies=[Depends(deps.GuildPerms(Permissions.BAN_MEMBERS))])
async def delete_guild_ban(guild_id: int, user_id: int,
                           db: Session = Depends(deps.get_db)):
    guild_ban = db.query(GuildBans).filter_by(
        guild_id=guild_id).filter_by(user_id=user_id).first()
    if guild_ban:
        await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_BAN_REMOVE,
                                args=guild_ban.serialize(), db=db)
        db.delete(guild_ban)
        db.commit()
        return Response(status_code=204)
    return Response(status_code=404)


@router.get('/{guild_id}/webhooks')
async def get_guild_webhooks(
        dependencies: tuple[Guild, User, GuildMembers] = Depends(deps.GuildPerms(Permissions.MANAGE_WEBHOOKS))):
    guild, *_ = dependencies
    return [webhook.serialize() for webhook in guild.webhooks]


@router.get("/{guild_id}/emojis")
async def get_guild_emojis(guild_id: int, db: Session = Depends(deps.get_db),
                           current_user: User = Depends(deps.get_current_user)):
    guild = db.query(Guild).filter_by(id=guild_id).first()
    if guild and guild.is_member(db, current_user.id):
        emojis = db.query(Emoji).filter_by(guild_id=guild_id).all()
        return [emoji.serialize() for emoji in emojis]
    return Response(status_code=404)


@router.get("/{guild_id}/emojis/{emoji_id}")
async def get_guild_emojis(guild_id: int, emoji_id: int, db: Session = Depends(deps.get_db),
                           current_user: User = Depends(deps.get_current_user)):
    guild = db.query(Guild).filter_by(id=guild_id).first()
    if guild and guild.is_member(db, current_user.id):
        emoji = db.query(Emoji).filter_by(id=emoji_id).filter_by(guild_id=guild_id).first()
        if emoji:
            return emoji.serialize()
    return Response(status_code=404)


class EditEmoji(BaseModel):
    name: Optional[str] = None
    roles: Optional[list[int]] = None


class CreateEmoji(BaseModel):
    name: str
    image: str
    roles: Optional[list[int]] = []


@router.post("/{guild_id}/emojis")
async def create_guild_emoji(guild_id: int,
                             body: CreateEmoji,
                             db: Session = Depends(deps.get_db),
                             dependencies: tuple[Guild, User, GuildMembers] = Depends(
                                 deps.GuildPerms(Permissions.MANAGE_EMOJIS_AND_STICKERS))):
    guild, user, _ = dependencies
    emoji = Emoji(name=re.sub('\W', '', body.name), guild_id=guild_id, user_id=user.id, roles=body.roles,
                  animated=False)
    roles = list(filter(lambda r: r != str(guild_id), body.roles))
    roles = db.query(Role).filter(Role.id.in_(roles)).all()
    emoji.roles = list(map(lambda r: r.id, roles))
    animated = await validate_emoji(emoji.id, body.image)
    emoji.animated = animated
    db.add(emoji)

    db.commit()

    emojis = db.query(Emoji).filter_by(guild_id=guild_id).all()

    await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_EMOJIS_UPDATE,
                            args={"emojis": [emoji.serialize() for emoji in emojis], "guild_id": str(guild_id)}, db=db)

    return emoji.serialize()


@router.patch("/{guild_id}/emojis/{emoji_id}")
async def update_guild_emoji(guild_id: int,
                             emoji_id: int,
                             body: EditEmoji,
                             db: Session = Depends(deps.get_db),
                             dependencies: tuple[Guild, User, GuildMembers] = Depends(
                                 deps.GuildPerms(Permissions.MANAGE_EMOJIS_AND_STICKERS))):
    guild, user, _ = dependencies
    emoji = db.query(Emoji).filter_by(id=emoji_id).filter_by(guild_id=guild_id).first()
    if not emoji:
        return Response(status_code=404)

    if body.name:
        emoji.name = secure_filename(body.name)
    if body.roles is not None:
        roles = list(filter(lambda r: r != str(guild_id), body.roles))
        roles = db.query(Role).filter(Role.id.in_(roles)).all()
        emoji.roles = list(map(lambda r: r.id, roles))

    db.commit()

    emojis = db.query(Emoji).filter_by(guild_id=guild_id).all()

    await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_EMOJIS_UPDATE,
                            args={"emojis": [emoji.serialize() for emoji in emojis], "guild_id": str(guild_id)}, db=db)

    return emoji.serialize()


@router.delete("/{guild_id}/emojis/{emoji_id}")
async def delete_guild_emoji(guild_id: int,
                             emoji_id: int,
                             db: Session = Depends(deps.get_db),
                             dependencies: tuple[Guild, User, GuildMembers] = Depends(
                                 deps.GuildPerms(Permissions.MANAGE_EMOJIS_AND_STICKERS))):
    guild, user, _ = dependencies
    emoji = db.query(Emoji).filter_by(id=emoji_id).filter_by(guild_id=guild_id).first()
    if not emoji:
        return Response(status_code=404)

    db.delete(emoji)
    db.commit()

    emojis = db.query(Emoji).filter_by(guild_id=guild_id).all()

    await websocket_emitter(channel_id=None, guild_id=guild_id, event=Events.GUILD_EMOJIS_UPDATE,
                            args={"emojis": [emoji.serialize() for emoji in emojis], "guild_id": str(guild_id)}, db=db)

    return Response(status_code=204)
