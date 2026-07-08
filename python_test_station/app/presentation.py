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
    "temperature_c": ("Temperature", "C"),
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

BATTERY_CAPACITY_MAH = 2600
BATTERY_NOMINAL_VOLTAGE = 3.7


def battery_status(telemetry: dict[str, Any] | None) -> dict[str, Any]:
    capacity_wh = BATTERY_CAPACITY_MAH * BATTERY_NOMINAL_VOLTAGE / 1000
    if not telemetry:
        return {
            "available": False,
            "percent": "--",
            "state": "offline",
            "estimated_power_mw": 0,
            "runtime_label": "--",
            "remaining_wh": 0.0,
            "capacity_wh": round(capacity_wh, 2),
            "breakdown": [],
        }

    percent = _as_float(telemetry.get("battery_percent"), 0.0)
    estimated_power = max(_as_float(telemetry.get("estimated_power_mw"), 0.0), 0.0)
    remaining_wh = capacity_wh * percent / 100
    runtime_hours = remaining_wh / (estimated_power / 1000) if estimated_power > 0 else 0

    if percent >= 50:
        state = "good"
    elif percent >= 20:
        state = "warning"
    else:
        state = "critical"

    return {
        "available": True,
        "percent": f"{percent:.0f}",
        "state": state,
        "estimated_power_mw": int(round(estimated_power)),
        "runtime_label": _runtime_label(runtime_hours),
        "remaining_wh": round(remaining_wh, 2),
        "capacity_wh": round(capacity_wh, 2),
        "breakdown": power_breakdown(telemetry),
    }


def power_breakdown(telemetry: dict[str, Any] | None) -> list[dict[str, Any]]:
    telemetry = telemetry or {}
    power_state = str(telemetry.get("power_state", "capture"))
    led_mode = str(telemetry.get("led_mode", "OFF"))
    estimated = int(round(_as_float(telemetry.get("estimated_power_mw"), 0.0)))

    if power_state == "sleep":
        modules = [("MCU sleep", 5)]
    elif power_state == "idle":
        modules = [("MCU idle", 20), ("UART debug", 5)]
    else:
        modules = [("MCU active", 35), ("Image sensor", 120), ("UART debug", 5)]
        if power_state == "transmit":
            modules.append(("Network TX", 300))
        if led_mode == "WLED":
            modules.append(("White LED", 450))
        elif led_mode == "IR":
            modules.append(("IR LED", 380))

    module_total = sum(value for _, value in modules)
    transient = max(0, estimated - module_total)
    if transient:
        modules.append(("Transient load", transient))
        module_total += transient

    total = max(module_total, estimated, 1)
    return [
        {
            "key": name.lower().replace(" ", "_"),
            "name": name,
            "mw": value,
            "share": round(value * 100 / total),
        }
        for name, value in modules
    ]


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


def _as_float(value: Any, default: float) -> float:
    try:
        return float(value)
    except (TypeError, ValueError):
        return default


def _runtime_label(hours: float) -> str:
    if hours <= 0:
        return "--"
    if hours < 1:
        return f"{hours * 60:.0f} min"
    if hours < 48:
        return f"{hours:.1f} h"
    return f"{hours / 24:.1f} days"


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
