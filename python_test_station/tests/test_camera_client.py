from __future__ import annotations

import pytest
import httpx

from python_test_station.app.camera_client import CameraClient


@pytest.fixture
def anyio_backend() -> str:
    return "asyncio"


def _transport(request: httpx.Request) -> httpx.Response:
    if request.url.path == "/health":
        return httpx.Response(200, json={"status": "ok", "active_mode": "stock"})
    if request.url.path == "/telemetry":
        return httpx.Response(200, json={"frame_id": 1, "active_mode": "stock"})
    if request.url.path == "/dev/photo-paths":
        return httpx.Response(200, json={"stock": "data/frames/stock"})
    if request.url.path == "/snapshot.jpg":
        return httpx.Response(200, content=b"jpeg-bytes", headers={"content-type": "image/jpeg"})
    if request.url.path == "/control/mode":
        return httpx.Response(200, json={"ok": True, "active_mode": "blurry"})
    if request.url.path == "/control/reset":
        return httpx.Response(200, json={"ok": True, "active_mode": "stock"})
    return httpx.Response(404)


@pytest.mark.anyio
async def test_camera_client_calls_gateway_endpoints() -> None:
    client = CameraClient(base_url="http://gateway.test", transport=httpx.MockTransport(_transport))

    health, health_error = await client.health()
    telemetry, telemetry_error = await client.telemetry()
    paths, paths_error = await client.photo_paths()
    snapshot, headers, snapshot_error = await client.snapshot()
    mode, mode_error = await client.set_mode("blurry")
    reset, reset_error = await client.reset()

    assert health_error is None
    assert telemetry_error is None
    assert paths_error is None
    assert snapshot_error is None
    assert mode_error is None
    assert reset_error is None
    assert health == {"status": "ok", "active_mode": "stock"}
    assert telemetry == {"frame_id": 1, "active_mode": "stock"}
    assert paths == {"stock": "data/frames/stock"}
    assert snapshot == b"jpeg-bytes"
    assert headers["content-type"] == "image/jpeg"
    assert mode == {"ok": True, "active_mode": "blurry"}
    assert reset == {"ok": True, "active_mode": "stock"}
