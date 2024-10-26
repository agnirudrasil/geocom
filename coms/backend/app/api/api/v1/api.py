import io

import requests
from fastapi import APIRouter, Depends, Response, HTTPException
from sqlalchemy.orm import Session
from starlette.responses import StreamingResponse

from api.api import deps
from api.api.v1.endpoints import auth, channels, gifs, users, guilds, invites, oauth2, webhooks, applications
from api.core import emitter
from api.core.events import Events, websocket_emitter
from api.models import ChannelMembers
from api.models.guilds import Guild, GuildMembers, GuildBans
from api.models.invites import Invite
from api.models.user import User

api_router = APIRouter()


@api_router.get("/proxy")
async def proxy(path: str, response: Response):
    content = requests.get(path)
    return StreamingResponse(io.BytesIO(content.content), media_type=content.headers["content-type"])


@api_router.post('/invites/{code}')
async def join_guild(code: str,
                     response: Response,
                     current_user: User = Depends(deps.get_current_user),
                     db: Session = Depends(deps.get_db)):
    if current_user.bot:
        raise HTTPException(status_code=403, detail="Bots cannot join invites")
    invite: Invite = db.query(Invite).filter_by(id=code).first()
    if invite:
        if not invite.guild_id:
            if len(invite.channel.members) < 10:
                channel_member = ChannelMembers(closed=False)
                channel_member.user = current_user
                invite.channel.members.append(channel_member)
                db.commit()
                await websocket_emitter(channel_id=invite.channel_id, guild_id=None, event=Events.CHANNEL_CREATE,
                                        args=invite.channel.serialize(current_user.id), user_id=current_user.id, db=db)
                for cm in invite.channel.members:
                    await websocket_emitter(channel_id=invite.channel_id, guild_id=None, event=Events.CHANNEL_UPDATE,
                                            args=invite.channel.serialize(cm.user_id), user_id=cm.user_id, db=db)
                return {'success': True}
            raise HTTPException(status_code=404, detail="Invite not found")
        guild_id = invite.channel.guild_id
        if invite.max_uses != 0 and invite.max_uses == invite.count:
            db.delete(invite)
            db.commit()
            response.status_code = 403
            return {"message": "Invite has been used"}
        if guild_id:
            guild_member = db.query(GuildMembers).filter_by(guild_id=guild_id).filter_by(
                user_id=current_user.id).first()
            guild_ban = db.query(GuildBans).filter_by(
                guild_id=guild_id).filter_by(user_id=current_user.id).first()
            if guild_ban:
                response.status_code = 403
                return {"error": "You are banned from this server"}
            if guild_member:
                response.status_code = 403
                return {"id": str(guild_id)}
            guild: Guild = db.query(Guild).filter_by(id=guild_id).first()
            user = db.query(User).filter_by(id=current_user.id).first()
            guild_member = GuildMembers()
            guild_member.member = user
            guild_member.guild = guild
            guild.members.append(guild_member)
            db.add(guild)
            invite.count += 1
            db.add(invite)
            db.commit()
            response.status_code = 200
            await emitter.in_room(str(current_user.id)).sockets_join(str(guild_id))
            await websocket_emitter(channel_id=None, guild_id=invite.channel.guild_id, event=Events.GUILD_CREATE,
                                    args={"guild": guild.serialize(
                                    ), "member": guild_member.serialize()},
                                    user_id=current_user.id, db=db)
            await websocket_emitter(channel_id=None, guild_id=invite.channel.guild_id, event=Events.GUILD_MEMBER_ADD,
                                    args=guild_member.serialize(), db=db)
            return guild.serialize()
        return "Success"
    response.status_code = 404
    return ""


api_router.include_router(auth.router, prefix='/auth', tags=["login"])
api_router.include_router(gifs.router, prefix='/gifs', tags=["gifs"])
api_router.include_router(
    channels.router, prefix="/channels", tags=["channels"])
api_router.include_router(users.router, prefix="/users", tags=["users"])
api_router.include_router(guilds.router, prefix="/guilds", tags=["guilds"])
api_router.include_router(invites.router, prefix='/invites', tags=["invites"])
api_router.include_router(
    webhooks.router, prefix='/webhooks', tags=["webhooks"])
api_router.include_router(
    applications.router, prefix='/applications', tags=["applications"])
api_router.include_router(oauth2.router, prefix='/oauth2', tags=["oauth2"])
