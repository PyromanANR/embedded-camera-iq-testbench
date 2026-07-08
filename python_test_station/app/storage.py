from __future__ import annotations

import json
from pathlib import Path

from .config import FRAMES_DIR, IMAGE_EXTENSIONS, REPORTS_DIR, ensure_runtime_dirs


def has_stock_images() -> bool:
    stock = FRAMES_DIR / "stock"
    return any(path.suffix.lower() in IMAGE_EXTENSIONS for path in stock.iterdir()) if stock.exists() else False


def list_reports() -> list[dict]:
    ensure_runtime_dirs()
    reports = []
    for path in sorted(REPORTS_DIR.glob("*.json"), reverse=True):
        try:
            data = json.loads(path.read_text(encoding="utf-8"))
            reports.append({
                "id": path.stem,
                "timestamp": data.get("timestamp", path.stem),
                "mode": data.get("mode", "unknown"),
                "verdict": data.get("verdict", "UNKNOWN"),
                "issues": data.get("issues", []),
            })
        except Exception:
            reports.append({"id": path.stem, "timestamp": path.stem, "mode": "unreadable", "verdict": "ERROR", "issues": []})
    return reports


def load_report(report_id: str) -> dict | None:
    path = REPORTS_DIR / f"{report_id}.json"
    if not path.exists():
        return None
    return json.loads(path.read_text(encoding="utf-8"))
