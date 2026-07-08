from __future__ import annotations

from pydantic import BaseModel


class ModeCommand(BaseModel):
    mode: str

