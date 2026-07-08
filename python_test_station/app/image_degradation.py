from __future__ import annotations

from io import BytesIO

import cv2
import numpy as np
from PIL import Image, ImageDraw, ImageFont


def create_placeholder_image(width: int = 1280, height: int = 720) -> np.ndarray:
    image = np.full((height, width, 3), (42, 48, 55), dtype=np.uint8)
    for y in range(0, height, 40):
        color = int(60 + (y / height) * 110)
        cv2.line(image, (0, y), (width, y), (color, color, color + 20), 1)
    cv2.rectangle(image, (80, 80), (width - 80, height - 80), (220, 220, 220), 3)
    cv2.circle(image, (width // 2, height // 2), 140, (60, 170, 220), -1)
    cv2.circle(image, (width // 2, height // 2), 70, (245, 245, 245), -1)
    cv2.putText(
        image,
        "SIMCAM PLACEHOLDER",
        (120, height - 150),
        cv2.FONT_HERSHEY_SIMPLEX,
        1.6,
        (255, 255, 255),
        3,
        cv2.LINE_AA,
    )
    cv2.putText(
        image,
        "Add clean camera images to data/frames/stock/",
        (120, height - 95),
        cv2.FONT_HERSHEY_SIMPLEX,
        1.0,
        (210, 235, 255),
        2,
        cv2.LINE_AA,
    )
    return image


def encode_jpeg(image: np.ndarray, quality: int = 90) -> bytes:
    ok, encoded = cv2.imencode(".jpg", image, [int(cv2.IMWRITE_JPEG_QUALITY), quality])
    if not ok:
        raise RuntimeError("Failed to encode JPEG image")
    return encoded.tobytes()


def decode_image(image_bytes: bytes) -> np.ndarray:
    buffer = np.frombuffer(image_bytes, dtype=np.uint8)
    image = cv2.imdecode(buffer, cv2.IMREAD_COLOR)
    if image is None:
        raise ValueError("Failed to decode image bytes")
    return image


def apply_degradation(image: np.ndarray, mode: str) -> np.ndarray:
    mode = mode.lower()
    if mode in {"stock", "clean"}:
        return image.copy()
    if mode == "blurry":
        return cv2.GaussianBlur(image, (25, 25), 0)
    if mode == "noisy":
        noise = np.random.normal(0, 28, image.shape).astype(np.int16)
        return np.clip(image.astype(np.int16) + noise, 0, 255).astype(np.uint8)
    if mode == "overexposed":
        return np.clip(image.astype(np.float32) * 1.45 + 65, 0, 255).astype(np.uint8)
    if mode == "underexposed":
        return np.clip(image.astype(np.float32) * 0.34, 0, 255).astype(np.uint8)
    if mode == "low_contrast":
        return np.clip((image.astype(np.float32) - 128) * 0.35 + 128, 0, 255).astype(np.uint8)
    if mode == "color_cast":
        shifted = image.astype(np.int16)
        shifted[:, :, 0] += 55
        shifted[:, :, 1] -= 8
        shifted[:, :, 2] -= 20
        return np.clip(shifted, 0, 255).astype(np.uint8)
    if mode == "jpeg_artifacts":
        return decode_image(encode_jpeg(image, quality=18))
    if mode == "dead_pixels":
        degraded = image.copy()
        h, w = degraded.shape[:2]
        count = max(200, int(h * w * 0.006))
        ys = np.random.randint(0, h, count)
        xs = np.random.randint(0, w, count)
        degraded[ys, xs] = np.where(np.random.rand(count, 1) > 0.5, 255, 0)
        return degraded
    if mode == "low_light":
        dark = np.clip(image.astype(np.float32) * 0.22, 0, 255).astype(np.uint8)
        noise = np.random.normal(0, 18, image.shape).astype(np.int16)
        return np.clip(dark.astype(np.int16) + noise, 0, 255).astype(np.uint8)
    return image.copy()

