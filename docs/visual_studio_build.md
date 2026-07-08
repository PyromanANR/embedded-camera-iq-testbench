# Visual Studio 2022 Build

## Command Line

```bat
scripts\windows\build_cpp_gateway.bat
```

## Visual Studio IDE

1. Open Visual Studio 2022.
2. Select `Open a local folder`.
3. Open the repository root.
4. Let Visual Studio configure the CMake project.
5. Select the `simcam_gateway` target.
6. Build and run.

The executable is generated under:

```text
camera_gateway_cpp/build/
```

Run it from the repository root so relative paths such as `data/frames/stock/` resolve correctly.

