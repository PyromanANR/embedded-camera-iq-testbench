from __future__ import annotations


def evaluate_verdict(metrics: dict) -> dict[str, object]:
    issues: list[str] = []
    severity = 0

    sharpness = float(metrics.get("sharpness_score", 0))
    noise = float(metrics.get("noise_score", 0))
    contrast = float(metrics.get("contrast_score", 0))
    exposure_status = str(metrics.get("exposure_status", "unknown"))
    color_cast = str(metrics.get("color_cast", "none"))
    jpeg_artifacts = float(metrics.get("jpeg_artifacts_score", 0))

    if sharpness < 25:
        issues.append("severe_blur")
        severity = max(severity, 2)
    elif sharpness < 80:
        issues.append("possible_blur")
        severity = max(severity, 1)

    if noise > 0.105:
        issues.append("high_noise")
        severity = max(severity, 2)
    elif noise > 0.065:
        issues.append("visible_noise")
        severity = max(severity, 1)

    if exposure_status in {"underexposed", "overexposed"}:
        issues.append(exposure_status)
        severity = max(severity, 2)
    elif exposure_status == "warning":
        issues.append("exposure_warning")
        severity = max(severity, 1)

    if contrast < 22:
        issues.append("low_contrast")
        severity = max(severity, 2)
    elif contrast < 34:
        issues.append("reduced_contrast")
        severity = max(severity, 1)

    if color_cast.startswith("strong"):
        issues.append("strong_color_cast")
        severity = max(severity, 2)
    elif color_cast.startswith("slight"):
        issues.append("slight_color_cast")
        severity = max(severity, 1)

    if jpeg_artifacts > 0.09:
        issues.append("jpeg_artifacts")
        severity = max(severity, 1)

    verdict = "PASS" if severity == 0 else "WARNING" if severity == 1 else "FAIL"
    return {"verdict": verdict, "issues": issues}

