# C++ Embedded Camera Gateway

Main simulated device API for the project.

## Build

```bat
..\scripts\windows\build_cpp_gateway.bat
```

## Run

From the repository root:

```bat
scripts\windows\run_cpp_gateway.bat
```

Default URL:

```text
http://127.0.0.1:8081
```

## Endpoints

- `GET /health`
- `GET /telemetry`
- `GET /snapshot.jpg`
- `POST /control/mode`
- `POST /control/reset`
- `GET /dev/photo-paths`

The gateway serves user images from `data/frames/*` when available and generates placeholder/degraded JPEG frames when folders are empty.

