# Test Cases

| ID | Title | Steps | Expected Result |
|---|---|---|---|
| TC-001 | C++ Gateway health endpoint | Start gateway, request `/health`. | JSON contains `status=ok`, device ID and active mode. |
| TC-002 | C++ Gateway telemetry endpoint | Request `/telemetry`. | JSON contains frame ID, battery, lux, exposure, gain and power. |
| TC-003 | Snapshot endpoint | Request `/snapshot.jpg`. | Response is an image and never crashes when stock folder is empty. |
| TC-004 | Dev Controls mode switch | Use dashboard Dev Controls to select Blur. | Gateway active mode changes to `blurry`. |
| TC-005 | Blurry mode IQ detection | Switch to Blur and run a test. | Sharpness score decreases and verdict is WARNING or FAIL. |
| TC-006 | Noisy mode IQ detection | Switch to Noise and run a test. | Noise score increases. |
| TC-007 | Exposure modes | Switch to Overexposed and Underexposed. | Exposure status reports warning or fault. |
| TC-008 | Report generation | Run Single Test. | JSON and HTML reports are saved under `python_test_station/reports/`. |
| TC-009 | Missing user images fallback | Empty `data/frames/stock/` and request snapshot. | C++ Gateway returns generated placeholder JPEG. |
| TC-010 | Zephyr qemu_cortex_m3 build script | Run `scripts\windows\build_zephyr.bat`. | Zephyr firmware builds for `qemu_cortex_m3` or reports missing environment setup clearly. |
