# Sample Bug Reports

## BUG-001: Overexposed Image Classified as PASS

Severity: High

Steps:
1. Start the C++ Gateway and Python dashboard.
2. Switch Dev Controls mode to Overexposed.
3. Run Single Test.

Expected: verdict is WARNING or FAIL with an exposure issue.

Actual: verdict is PASS.

Notes: exposure thresholds may be too loose for clipped highlights.

## BUG-002: Telemetry Missing from Generated Report

Severity: Medium

Steps:
1. Start both runtime services.
2. Run Single Test.
3. Open the generated JSON report.

Expected: telemetry contains frame ID, battery, lux and estimated power.

Actual: telemetry object is empty.

Notes: report generation should fail clearly if telemetry cannot be fetched.

## BUG-003: Dev Controls Mode Switch Does Not Update Snapshot

Severity: Medium

Steps:
1. Open Dev Controls.
2. Select Blur and press Apply.
3. Refresh Dashboard.

Expected: active mode is `blurry` and snapshot/metrics change.

Actual: active mode changes but the snapshot remains stock.

Notes: check gateway state, snapshot caching and dashboard cache-busting query string.

