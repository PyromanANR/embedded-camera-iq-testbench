from __future__ import annotations

import cv2
import numpy as np


def _gray(image: np.ndarray) -> np.ndarray:
    return cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)


def sharpness_score(image: np.ndarray) -> float:
    gray = _gray(image)
    return float(cv2.Laplacian(gray, cv2.CV_64F).var())


def noise_score(image: np.ndarray) -> float:
    gray = _gray(image).astype(np.float32)
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)
    residual = gray - blurred
    return float(np.std(residual) / 255.0)


def exposure_analysis(image: np.ndarray) -> dict[str, float | str]:
    gray = _gray(image)
    total = gray.size
    under = float(np.count_nonzero(gray < 25) / total)
    over = float(np.count_nonzero(gray > 245) / total)
    mean = float(np.mean(gray))
    if under > 0.35 or mean < 45:
        status = "underexposed"
    elif over > 0.20 or mean > 215:
        status = "overexposed"
    elif under > 0.15 or over > 0.08 or mean < 65 or mean > 190:
        status = "warning"
    else:
        status = "ok"
    return {
        "underexposed_ratio": under,
        "overexposed_ratio": over,
        "mean_brightness": mean,
        "exposure_status": status,
    }


def contrast_score(image: np.ndarray) -> float:
    return float(np.std(_gray(image)))


def color_cast_detection(image: np.ndarray) -> dict[str, float | str]:
    means = image.reshape(-1, 3).mean(axis=0)
    b, g, r = [float(v) for v in means]
    avg = max((r + g + b) / 3.0, 1.0)
    deltas = {"blue": b - avg, "green": g - avg, "red": r - avg}
    dominant = max(deltas, key=lambda key: abs(deltas[key]))
    imbalance = abs(deltas[dominant]) / avg
    if imbalance < 0.10:
        label = "none"
    elif imbalance < 0.22:
        label = f"slight_{dominant}"
    else:
        label = f"strong_{dominant}"
    return {
        "color_cast": label,
        "color_imbalance": float(imbalance),
        "mean_r": r,
        "mean_g": g,
        "mean_b": b,
    }


def jpeg_artifacts_score(image: np.ndarray) -> float:
    gray = _gray(image).astype(np.float32)
    h, w = gray.shape
    vertical = np.mean(np.abs(gray[:, 8:w:8] - gray[:, 7:w - 1:8])) if w > 8 else 0.0
    horizontal = np.mean(np.abs(gray[8:h:8, :] - gray[7:h - 1:8, :])) if h > 8 else 0.0
    return float((vertical + horizontal) / 510.0)


def calculate_iq_metrics(image: np.ndarray) -> dict[str, float | str | list[str]]:
    metrics: dict[str, float | str | list[str]] = {}
    metrics["sharpness_score"] = sharpness_score(image)
    metrics["noise_score"] = noise_score(image)
    metrics.update(exposure_analysis(image))
    metrics["contrast_score"] = contrast_score(image)
    metrics.update(color_cast_detection(image))
    metrics["jpeg_artifacts_score"] = jpeg_artifacts_score(image)
    return metrics

