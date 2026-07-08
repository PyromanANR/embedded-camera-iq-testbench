from __future__ import annotations

from .image_degradation import decode_image
from .iq_metrics import calculate_iq_metrics
from .verdict import evaluate_verdict


def analyze_snapshot(image_bytes: bytes) -> dict:
    image = decode_image(image_bytes)
    metrics = calculate_iq_metrics(image)
    metrics.update(evaluate_verdict(metrics))
    return metrics
