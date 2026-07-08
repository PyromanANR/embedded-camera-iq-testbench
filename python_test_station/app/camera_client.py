from __future__ import annotations

import httpx

from .config import DEVICE_BASE_URL


class CameraClient:
    def __init__(
        self,
        base_url: str = DEVICE_BASE_URL,
        timeout_s: float = 3.0,
        transport: httpx.AsyncBaseTransport | None = None,
    ) -> None:
        self.base_url = base_url.rstrip("/")
        self.timeout_s = timeout_s
        self.transport = transport

    async def _get_json(self, path: str) -> tuple[dict | None, str | None]:
        try:
            async with httpx.AsyncClient(timeout=self.timeout_s, transport=self.transport) as client:
                response = await client.get(f"{self.base_url}{path}")
                response.raise_for_status()
                return response.json(), None
        except Exception as exc:
            return None, str(exc)

    async def health(self) -> tuple[dict | None, str | None]:
        return await self._get_json("/health")

    async def telemetry(self) -> tuple[dict | None, str | None]:
        return await self._get_json("/telemetry")

    async def photo_paths(self) -> tuple[dict | None, str | None]:
        return await self._get_json("/dev/photo-paths")

    async def snapshot(self) -> tuple[bytes | None, dict, str | None]:
        try:
            async with httpx.AsyncClient(timeout=self.timeout_s, transport=self.transport) as client:
                response = await client.get(f"{self.base_url}/snapshot.jpg")
                response.raise_for_status()
                return response.content, dict(response.headers), None
        except Exception as exc:
            return None, {}, str(exc)

    async def set_mode(self, mode: str) -> tuple[dict | None, str | None]:
        try:
            async with httpx.AsyncClient(timeout=self.timeout_s, transport=self.transport) as client:
                response = await client.post(f"{self.base_url}/control/mode", json={"mode": mode})
                response.raise_for_status()
                return response.json(), None
        except Exception as exc:
            return None, str(exc)

    async def reset(self) -> tuple[dict | None, str | None]:
        try:
            async with httpx.AsyncClient(timeout=self.timeout_s, transport=self.transport) as client:
                response = await client.post(f"{self.base_url}/control/reset")
                response.raise_for_status()
                return response.json(), None
        except Exception as exc:
            return None, str(exc)
