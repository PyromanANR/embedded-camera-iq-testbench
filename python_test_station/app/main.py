from __future__ import annotations

import time

from fastapi import FastAPI, Form, HTTPException, Request
from fastapi.responses import RedirectResponse, Response
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates

from .camera_client import CameraClient
from .config import (
    DEVICE_BASE_URL,
    DEVICE_MODES,
    FRAMES_DIR,
    MODE_LABELS,
    PROJECT_ROOT,
    STATIC_SNAPSHOT_DIR,
    TEST_STATION_URL,
    ensure_runtime_dirs,
    photo_paths,
)
from .image_degradation import create_placeholder_image, encode_jpeg
from .image_pipeline import analyze_snapshot
from .presentation import metric_rows, telemetry_rows
from .report_generator import save_report
from .storage import has_stock_images, list_reports, load_report


ensure_runtime_dirs()
app = FastAPI(title="Embedded Camera IQ Test Station", version="0.1.0")
templates = Jinja2Templates(directory=str(PROJECT_ROOT / "python_test_station" / "app" / "templates"))
app.mount("/static", StaticFiles(directory=str(PROJECT_ROOT / "python_test_station" / "app" / "static")), name="static")
app.mount("/report-snapshots", StaticFiles(directory=str(STATIC_SNAPSHOT_DIR)), name="report_snapshots")


def _client() -> CameraClient:
    return CameraClient()


async def _dashboard_context(request: Request) -> dict:
    client = _client()
    health, health_error = await client.health()
    telemetry, telemetry_error = await client.telemetry() if not health_error else (None, health_error)
    snapshot_bytes, snapshot_headers, snapshot_error = await client.snapshot() if not health_error else (None, {}, health_error)
    metrics = None
    if snapshot_bytes:
        try:
            metrics = analyze_snapshot(snapshot_bytes)
        except Exception as exc:
            snapshot_error = f"Snapshot analysis failed: {exc}"

    stock_missing = not has_stock_images()
    return {
        "request": request,
        "device_base_url": DEVICE_BASE_URL,
        "test_station_url": TEST_STATION_URL,
        "health": health,
        "telemetry": telemetry,
        "telemetry_rows": telemetry_rows(telemetry),
        "metrics": metrics,
        "metric_rows": metric_rows(metrics),
        "latest_reports": list_reports()[:5],
        "device_error": health_error or telemetry_error or snapshot_error,
        "snapshot_url": f"/snapshot/current.jpg?ts={int(time.time() * 1000)}",
        "stock_missing": stock_missing,
        "stock_path": "data/frames/stock/",
        "active_page": "dashboard",
    }


@app.get("/")
async def dashboard(request: Request):
    return templates.TemplateResponse(request, "dashboard.html", await _dashboard_context(request))


@app.get("/run-test")
async def run_test_page(request: Request):
    return RedirectResponse(url="/", status_code=303)


@app.post("/tests/run")
async def run_test(frames: int = Form(1)):
    frames = max(1, min(frames, 10))
    client = _client()
    health, health_error = await client.health()
    if health_error:
        raise HTTPException(status_code=503, detail="C++ Camera Gateway is offline. Start the gateway on port 8081.")

    telemetry = None
    metrics = None
    snapshot_bytes = None
    for _ in range(frames):
        telemetry, _ = await client.telemetry()
        snapshot_bytes, _, snapshot_error = await client.snapshot()
        if snapshot_error or not snapshot_bytes:
            raise HTTPException(status_code=502, detail=f"Could not fetch snapshot: {snapshot_error}")
        metrics = analyze_snapshot(snapshot_bytes)

    report = save_report(
        mode=(health or {}).get("active_mode", "unknown"),
        telemetry=telemetry,
        metrics=metrics or {},
        snapshot_bytes=snapshot_bytes,
        frame_count=frames,
    )
    return RedirectResponse(url=f"/reports/{report['id']}", status_code=303)


@app.get("/snapshot/current.jpg")
async def current_snapshot():
    client = _client()
    snapshot_bytes, headers, error = await client.snapshot()
    if error or not snapshot_bytes:
        snapshot_bytes = encode_jpeg(create_placeholder_image())
    response_headers = {"Cache-Control": "no-store"}
    if "x-simcam-placeholder" in headers:
        response_headers["X-Simcam-Placeholder"] = headers["x-simcam-placeholder"]
    return Response(content=snapshot_bytes, media_type="image/jpeg", headers=response_headers)


@app.get("/dev-controls")
async def dev_controls(request: Request):
    client = _client()
    health, health_error = await client.health()
    paths, _ = await client.photo_paths()
    return templates.TemplateResponse(
        request,
        "dev_controls.html",
        {
            "request": request,
            "active_page": "dev_controls",
            "health": health,
            "device_error": health_error,
            "modes": DEVICE_MODES + ["random_fault"],
            "mode_labels": MODE_LABELS,
            "photo_paths": paths or photo_paths(),
            "stock_path": "data/frames/stock/",
            "snapshot_url": f"/snapshot/current.jpg?ts={int(time.time() * 1000)}",
        },
    )


@app.post("/dev-controls/mode")
async def apply_mode(mode: str = Form(...)):
    _, error = await _client().set_mode(mode)
    if error:
        raise HTTPException(status_code=503, detail=f"Failed to switch mode: {error}")
    return RedirectResponse(url="/dev-controls", status_code=303)


@app.post("/dev-controls/reset")
async def reset_device():
    _, error = await _client().reset()
    if error:
        raise HTTPException(status_code=503, detail=f"Failed to reset device: {error}")
    return RedirectResponse(url="/dev-controls", status_code=303)


@app.get("/reports")
async def reports(request: Request):
    return templates.TemplateResponse(
        request,
        "reports.html",
        {"request": request, "active_page": "reports", "reports": list_reports()},
    )


@app.get("/reports/{report_id}")
async def report_detail(request: Request, report_id: str):
    report = load_report(report_id)
    if not report:
        raise HTTPException(status_code=404, detail="Report not found")
    return templates.TemplateResponse(
        request,
        "report_detail.html",
        {
            "request": request,
            "active_page": "reports",
            "report": report,
            "metric_rows": metric_rows(report.get("image_metrics")),
            "telemetry_rows": telemetry_rows(report.get("telemetry")),
        },
    )


@app.get("/setup")
async def setup(request: Request):
    return templates.TemplateResponse(
        request,
        "setup.html",
        {
            "request": request,
            "active_page": "setup",
            "project_root": ".",
            "stock_path": "data/frames/stock/",
            "device_base_url": DEVICE_BASE_URL,
            "test_station_url": TEST_STATION_URL,
        },
    )
