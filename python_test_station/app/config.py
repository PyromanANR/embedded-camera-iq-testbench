from __future__ import annotations

import os
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parents[2]
DEVICE_BASE_URL = os.getenv("SIMCAM_DEVICE_URL", "http://127.0.0.1:8081")
TEST_STATION_URL = os.getenv("SIMCAM_TEST_STATION_URL", "http://127.0.0.1:8000")

DATA_DIR = PROJECT_ROOT / "data"
FRAMES_DIR = DATA_DIR / "frames"
GENERATED_DIR = DATA_DIR / "generated"
REPORTS_DIR = PROJECT_ROOT / "python_test_station" / "reports"
STATIC_SNAPSHOT_DIR = REPORTS_DIR / "snapshots"

IMAGE_EXTENSIONS = {".jpg", ".jpeg", ".png", ".bmp", ".webp"}

DEVICE_MODES = [
    "stock",
    "blurry",
    "noisy",
    "overexposed",
    "underexposed",
    "low_contrast",
    "color_cast",
    "jpeg_artifacts",
    "dead_pixels",
    "low_light",
]

MODE_LABELS = {
    "stock": "Stock / Clean image",
    "blurry": "Blur",
    "noisy": "Noise",
    "overexposed": "Overexposed",
    "underexposed": "Underexposed",
    "low_contrast": "Low contrast",
    "color_cast": "Color cast",
    "jpeg_artifacts": "JPEG artifacts",
    "dead_pixels": "Dead pixels",
    "low_light": "Low light",
    "random_fault": "Random fault",
}


def ensure_runtime_dirs() -> None:
    REPORTS_DIR.mkdir(parents=True, exist_ok=True)
    STATIC_SNAPSHOT_DIR.mkdir(parents=True, exist_ok=True)
    for mode in DEVICE_MODES:
        (FRAMES_DIR / mode).mkdir(parents=True, exist_ok=True)
    GENERATED_DIR.mkdir(parents=True, exist_ok=True)


def photo_paths() -> dict[str, str]:
    return {mode: f"data/frames/{mode}" for mode in DEVICE_MODES}
