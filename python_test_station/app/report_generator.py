from __future__ import annotations

import html
import json
from datetime import datetime, timezone

from .config import REPORTS_DIR, STATIC_SNAPSHOT_DIR, ensure_runtime_dirs


def _report_id() -> str:
    return datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S_%f")


def save_report(
    *,
    mode: str,
    telemetry: dict | None,
    metrics: dict,
    snapshot_bytes: bytes | None,
    frame_count: int,
) -> dict:
    ensure_runtime_dirs()
    report_id = _report_id()
    snapshot_path = None
    if snapshot_bytes:
        snapshot_file = STATIC_SNAPSHOT_DIR / f"{report_id}.jpg"
        snapshot_file.write_bytes(snapshot_bytes)
        snapshot_path = f"python_test_station/reports/snapshots/{report_id}.jpg"

    report = {
        "id": report_id,
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "mode": mode,
        "frame_count": frame_count,
        "telemetry": telemetry or {},
        "image_metrics": metrics,
        "verdict": metrics.get("verdict", "UNKNOWN"),
        "issues": metrics.get("issues", []),
        "snapshot_path": snapshot_path,
        "explanation": _explain(metrics),
    }

    json_path = REPORTS_DIR / f"{report_id}.json"
    html_path = REPORTS_DIR / f"{report_id}.html"
    json_path.write_text(json.dumps(report, indent=2), encoding="utf-8")
    html_path.write_text(_html_report(report), encoding="utf-8")
    report["json_path"] = str(json_path)
    report["html_path"] = str(html_path)
    return report


def _explain(metrics: dict) -> str:
    verdict = metrics.get("verdict", "UNKNOWN")
    issues = metrics.get("issues", [])
    if verdict == "PASS":
        return "The frame is within the configured objective image quality limits."
    if not issues:
        return "The result is not passing, but no specific issue was classified."
    return "Detected image quality issues: " + ", ".join(str(issue) for issue in issues) + "."


def _html_report(report: dict) -> str:
    metrics_rows = "\n".join(
        f"<tr><th>{html.escape(str(key))}</th><td>{html.escape(str(value))}</td></tr>"
        for key, value in report["image_metrics"].items()
    )
    telemetry_rows = "\n".join(
        f"<tr><th>{html.escape(str(key))}</th><td>{html.escape(str(value))}</td></tr>"
        for key, value in report["telemetry"].items()
    )
    snapshot = f"<p>Snapshot: {html.escape(report.get('snapshot_path') or 'not saved')}</p>"
    return f"""<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <title>SIMCAM IQ Report {html.escape(report['id'])}</title>
  <style>
    body {{ font-family: Arial, sans-serif; margin: 32px; color: #172026; }}
    table {{ border-collapse: collapse; width: 100%; margin: 16px 0; }}
    th, td {{ border: 1px solid #c8d1d8; padding: 8px; text-align: left; }}
    th {{ background: #eef3f6; width: 260px; }}
    .verdict {{ font-size: 28px; font-weight: 700; }}
  </style>
</head>
<body>
  <h1>SIMCAM Image Quality Report</h1>
  <p class="verdict">Verdict: {html.escape(str(report['verdict']))}</p>
  <p>Timestamp: {html.escape(report['timestamp'])}</p>
  <p>Mode: {html.escape(report['mode'])}</p>
  <p>Frames: {report['frame_count']}</p>
  <p>{html.escape(report['explanation'])}</p>
  {snapshot}
  <h2>Image Metrics</h2>
  <table>{metrics_rows}</table>
  <h2>Telemetry</h2>
  <table>{telemetry_rows}</table>
</body>
</html>
"""
