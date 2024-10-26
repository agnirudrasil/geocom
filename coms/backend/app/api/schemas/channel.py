from typing import Literal, Optional

from pydantic import BaseModel, validator

from api.models.channels import ChannelType


class ThreadCreate(BaseModel):
    name: str
    auto_arrive_duration: Optional[int] = 1440
    type: Literal['public', 'private']


class ChannelValidate(BaseModel):
    name: str
    type: str
    parent_id: Optional[int]
    nsfw: Optional[bool]
    topic: Optional[str]
    privateChannel: bool

    @classmethod
    @validator('name')
    def name_validate(cls, v: str, values):
        v = v.strip()
        if v is None:
            raise ValueError("Name cannot be empty")
        if len(v) > 100:
            raise ValueError("Name must be less than 100 characters")
        elif len(v) < 1:
            raise ValueError("Name must be at least 1 characters")
        return v

    @classmethod
    @validator('nsfw')
    def nsfw_validate(cls, v: Optional[bool]):
        return bool(v)

    @classmethod
    @validator('topic')
    def topic_validate(cls, v: str):
        v = v.strip()
        if 1 <= len(v) <= 1024:
            raise ValueError("Topic must be less than 1024 characters")
        return v or ""

    @validator("type")
    def validate_type(cls, field):
        if field not in ChannelType.__members__ and field not in ChannelType.__members__.values():
            raise ValueError("Invalid channel type")
        return field


class ChannelEdit(BaseModel):
    name: Optional[str] = None
    owner_id: Optional[int] = None
    parent_id: Optional[int] = None
    topic: Optional[str] = None
    icon: Optional[str] = None
