from __future__ import annotations

import sys
from pathlib import Path

import cv2
import numpy as np

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from python_test_station.app.config import DEVICE_MODES, FRAMES_DIR, ensure_runtime_dirs
from python_test_station.app.image_degradation import apply_degradation, create_placeholder_image


def create_clean_sample() -> np.ndarray:
    image = create_placeholder_image()
    cv2.rectangle(image, (920, 120), (1180, 360), (40, 190, 95), -1)
    cv2.putText(image, "FOCUS TARGET", (930, 410), cv2.FONT_HERSHEY_SIMPLEX, 1.1, (255, 255, 255), 2, cv2.LINE_AA)
    for x in range(930, 1170, 20):
        cv2.line(image, (x, 135), (x, 345), (255, 255, 255), 2)
    return image


def main() -> None:
    ensure_runtime_dirs()
    clean = create_clean_sample()
    stock_path = FRAMES_DIR / "stock" / "sample_stock.jpg"
    cv2.imwrite(str(stock_path), clean)
    for mode in DEVICE_MODES:
        if mode == "stock":
            continue
        degraded = apply_degradation(clean, mode)
        cv2.imwrite(str(FRAMES_DIR / mode / f"sample_{mode}.jpg"), degraded)
    print(f"Sample images generated under {FRAMES_DIR}")


if __name__ == "__main__":
    main()

