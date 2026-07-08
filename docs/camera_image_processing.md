# Camera Image Processing Notes

## Sharpness / Focus Score

The project uses variance of Laplacian as a focus metric. A blurred image has lower high-frequency content and therefore a lower score.

## Exposure

Exposure analysis checks mean brightness, dark pixel ratio and clipped highlight ratio. This detects underexposed and overexposed frames.

## Gain and Noise

Higher sensor gain often increases visible noise. The C++ Gateway telemetry raises `sensor_gain` in noisy and low-light modes.

## Low Light

Low-light mode combines low lux, longer exposure, higher gain, IR LED mode, darker image and extra noise.

## Color Cast

Color cast detection compares RGB channel means and flags significant imbalance, such as blue or red shifts.

## Basic 3A Concepts

- Auto exposure adjusts exposure time and gain for brightness.
- Auto white balance adjusts channel gains to neutralize color cast.
- Autofocus scoring uses sharpness metrics to select the best lens position.

## Demosaic and Noise Reduction

Real image sensors capture Bayer raw data that must be demosaiced into RGB. Noise reduction removes random sensor noise while preserving edges and details.
