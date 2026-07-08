from __future__ import annotations


def parse_uart_telemetry(packet: str) -> dict[str, str | int | float]:
    packet = packet.strip()
    if not packet.startswith("$SIMCAM"):
        raise ValueError("Unsupported telemetry packet prefix")
    result: dict[str, str | int | float] = {}
    for field in packet.split(",")[1:]:
        if "=" not in field:
            continue
        key, value = field.split("=", 1)
        if key == "CRC":
            result[key] = value
            continue
        try:
            if "." in value:
                result[key] = float(value)
            else:
                result[key] = int(value)
        except ValueError:
            result[key] = value
    return result

