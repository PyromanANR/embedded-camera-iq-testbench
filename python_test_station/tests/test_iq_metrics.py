from __future__ import annotations

import cv2
import numpy as np

from python_test_station.app.image_degradation import apply_degradation, create_placeholder_image
from python_test_station.app.iq_metrics import calculate_iq_metrics, exposure_analysis, noise_score, sharpness_score


def test_sharpness_blur_detection() -> None:
    clean = create_placeholder_image(640, 360)
    blurred = apply_degradation(clean, "blurry")

    assert sharpness_score(clean) > sharpness_score(blurred) * 3


def test_noise_score_increases_with_noise() -> None:
    clean = create_placeholder_image(640, 360)
    noisy = apply_degradation(clean, "noisy")

    assert noise_score(noisy) > noise_score(clean)


def test_exposure_detection() -> None:
    clean = create_placeholder_image(640, 360)
    over = apply_degradation(clean, "overexposed")
    under = apply_degradation(clean, "underexposed")

    assert exposure_analysis(over)["exposure_status"] in {"overexposed", "warning"}
    assert exposure_analysis(under)["exposure_status"] in {"underexposed", "warning"}


def test_metrics_include_expected_keys() -> None:
    metrics = calculate_iq_metrics(create_placeholder_image(640, 360))

    assert "sharpness_score" in metrics
    assert "noise_score" in metrics
    assert "contrast_score" in metrics
    assert "color_cast" in metrics

