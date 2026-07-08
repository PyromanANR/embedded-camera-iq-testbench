from __future__ import annotations

from python_test_station.app.verdict import evaluate_verdict


def test_pass_verdict_for_nominal_metrics() -> None:
    metrics = {
        "sharpness_score": 180.0,
        "noise_score": 0.02,
        "contrast_score": 55.0,
        "exposure_status": "ok",
        "color_cast": "none",
        "jpeg_artifacts_score": 0.01,
    }

    assert evaluate_verdict(metrics)["verdict"] == "PASS"


def test_fail_verdict_for_blur_and_exposure() -> None:
    metrics = {
        "sharpness_score": 12.0,
        "noise_score": 0.02,
        "contrast_score": 55.0,
        "exposure_status": "underexposed",
        "color_cast": "none",
        "jpeg_artifacts_score": 0.01,
    }

    result = evaluate_verdict(metrics)
    assert result["verdict"] == "FAIL"
    assert "severe_blur" in result["issues"]
    assert "underexposed" in result["issues"]

