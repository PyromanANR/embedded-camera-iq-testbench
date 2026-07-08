from __future__ import annotations

from python_test_station.app.telemetry_parser import parse_uart_telemetry


def test_uart_telemetry_parser() -> None:
    packet = "$SIMCAM,FRAME=128,TEMP=42.5,BAT=87,LUX=320,EXP=8.3,GAIN=1.4,LED=IR,PWR=184,ERR=0,CRC=7A"

    parsed = parse_uart_telemetry(packet)

    assert parsed["FRAME"] == 128
    assert parsed["TEMP"] == 42.5
    assert parsed["LED"] == "IR"
    assert parsed["CRC"] == "7A"

