from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class MetricSpec:
    key: str
    label: str
    unit: str
    normal: str
    direction: str
    pass_min: float | None = None
    pass_max: float | None = None
    warn_min: float | None = None
    warn_max: float | None = None
    precision: int = 2


METRIC_SPECS = [
    MetricSpec("sharpness_score", "Sharpness / focus", "score", ">= 80 pass, 25-80 warning", "higher", pass_min=80, warn_min=25),
    MetricSpec("noise_score", "High-frequency noise", "ratio", "<= 0.065 pass, <= 0.105 warning", "lower", pass_max=0.065, warn_max=0.105, precision=4),
    MetricSpec("mean_brightness", "Mean brightness", "0-255", "65-190 pass", "range", pass_min=65, pass_max=190, warn_min=45, warn_max=215),
    MetricSpec("contrast_score", "Contrast", "std dev", ">= 34 pass, 22-34 warning", "higher", pass_min=34, warn_min=22),
    MetricSpec("underexposed_ratio", "Dark pixel ratio", "%", "<= 15% pass, <= 35% warning", "lower", pass_max=0.15, warn_max=0.35, precision=1),
    MetricSpec("overexposed_ratio", "Clipped highlight ratio", "%", "<= 8% pass, <= 20% warning", "lower", pass_max=0.08, warn_max=0.20, precision=1),
    MetricSpec("color_imbalance", "RGB channel imbalance", "%", "<= 10% pass, <= 22% warning", "lower", pass_max=0.10, warn_max=0.22, precision=1),
    MetricSpec("jpeg_artifacts_score", "JPEG block artifact score", "ratio", "<= 0.09 warning limit", "lower", pass_max=0.05, warn_max=0.09, precision=4),
]

TELEMETRY_LABELS = {
    "device_id": ("Device ID", ""),
    "firmware_version": ("Firmware version", ""),
    "frame_id": ("Frame ID", "frames"),
    "temperature_c": ("Temperature", "°C"),
    "battery_percent": ("Battery charge", "%"),
    "lux": ("Illuminance", "lx"),
    "exposure_ms": ("Exposure time", "ms"),
    "sensor_gain": ("Sensor gain", "x"),
    "led_mode": ("LED mode", ""),
    "power_state": ("Power state", ""),
    "estimated_power_mw": ("Estimated power", "mW"),
    "dropped_frames": ("Dropped frames", "frames"),
    "connection_status": ("Connection status", ""),
    "error_code": ("Error code", ""),
    "active_mode": ("Active mode", ""),
}


def metric_rows(metrics: dict[str, Any] | None) -> list[dict[str, str]]:
    if not metrics:
        return []
    rows = []
    for spec in METRIC_SPECS:
        if spec.key not in metrics:
            continue
        raw = metrics[spec.key]
        value = float(raw)
        status = _metric_status(value, spec)
        rows.append(
            {
                "key": spec.key,
                "label": spec.label,
                "value": _format_metric_value(value, spec),
                "unit": spec.unit,
                "normal": spec.normal,
                "delta": _delta_from_normal(value, spec),
                "status": status,
            }
        )
    rows.append(
        {
            "key": "exposure_status",
            "label": "Exposure classification",
            "value": str(metrics.get("exposure_status", "unknown")),
            "unit": "",
            "normal": "ok",
            "delta": "classification",
            "status": "pass" if metrics.get("exposure_status") == "ok" else "fail",
        }
    )
    rows.append(
        {
            "key": "color_cast",
            "label": "Color cast",
            "value": str(metrics.get("color_cast", "unknown")),
            "unit": "",
            "normal": "none",
            "delta": "classification",
            "status": "pass" if metrics.get("color_cast") == "none" else "warning",
        }
    )
    return rows


def telemetry_rows(telemetry: dict[str, Any] | None) -> list[dict[str, str]]:
    if not telemetry:
        return []
    rows = []
    for key, value in telemetry.items():
        label, unit = TELEMETRY_LABELS.get(key, (key.replace("_", " ").title(), ""))
        rows.append(
            {
                "key": key,
                "label": label,
                "value": _format_telemetry_value(value),
                "unit": unit,
            }
        )
    return rows


def _format_metric_value(value: float, spec: MetricSpec) -> str:
    if spec.unit == "%":
        return f"{value * 100:.{spec.precision}f}"
    return f"{value:.{spec.precision}f}"


def _format_telemetry_value(value: Any) -> str:
    if isinstance(value, float):
        return f"{value:.2f}".rstrip("0").rstrip(".")
    return str(value)


def _metric_status(value: float, spec: MetricSpec) -> str:
    if spec.direction == "higher":
        if spec.pass_min is not None and value >= spec.pass_min:
            return "pass"
        if spec.warn_min is not None and value >= spec.warn_min:
            return "warning"
        return "fail"
    if spec.direction == "lower":
        if spec.pass_max is not None and value <= spec.pass_max:
            return "pass"
        if spec.warn_max is not None and value <= spec.warn_max:
            return "warning"
        return "fail"
    if spec.pass_min is not None and spec.pass_max is not None and spec.pass_min <= value <= spec.pass_max:
        return "pass"
    if spec.warn_min is not None and spec.warn_max is not None and spec.warn_min <= value <= spec.warn_max:
        return "warning"
    return "fail"


def _delta_from_normal(value: float, spec: MetricSpec) -> str:
    display_value = value * 100 if spec.unit == "%" else value
    unit = "%" if spec.unit == "%" else spec.unit
    if spec.direction == "higher" and spec.pass_min is not None:
        delta = display_value - spec.pass_min
        return f"{delta:+.{spec.precision}f} {unit} vs pass limit"
    if spec.direction == "lower" and spec.pass_max is not None:
        limit = spec.pass_max * 100 if spec.unit == "%" else spec.pass_max
        delta = display_value - limit
        return f"{delta:+.{spec.precision}f} {unit} vs pass limit"
    if spec.direction == "range" and spec.pass_min is not None and spec.pass_max is not None:
        if spec.pass_min <= value <= spec.pass_max:
            return "inside normal range"
        nearest = spec.pass_min if value < spec.pass_min else spec.pass_max
        return f"{value - nearest:+.{spec.precision}f} {unit} vs range"
    return "-"

