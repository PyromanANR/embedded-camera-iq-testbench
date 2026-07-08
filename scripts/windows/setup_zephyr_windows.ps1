$ErrorActionPreference = "Stop"

Write-Host "Checking Zephyr/Windows development tools..." -ForegroundColor Cyan

function Test-Command($Name) {
    $cmd = Get-Command $Name -ErrorAction SilentlyContinue
    if ($cmd) {
        Write-Host "[OK] $Name -> $($cmd.Source)"
        return $true
    }
    Write-Host "[MISSING] $Name" -ForegroundColor Yellow
    return $false
}

function Find-West {
    $cmd = Get-Command west -ErrorAction SilentlyContinue
    if ($cmd) {
        return $cmd.Source
    }
    $candidate = Join-Path $env:USERPROFILE "zephyrproject\.venv\Scripts\west.exe"
    if (Test-Path $candidate) {
        return $candidate
    }
    return $null
}

$hasPy = (Test-Command "py") -or (Test-Command "python")
$westPath = Find-West
if ($westPath) {
    Write-Host "[OK] west -> available"
    $hasWest = $true
} else {
    Write-Host "[MISSING] west" -ForegroundColor Yellow
    $hasWest = $false
}
$hasCmake = Test-Command "cmake"
$hasNinja = Test-Command "ninja"
$hasDtc = Test-Command "dtc"

if (-not $hasWest) {
    if ($hasPy) {
        Write-Host "west is missing. If your Python has pip, install it with:"
        if (Get-Command py -ErrorAction SilentlyContinue) {
            Write-Host "  py -m pip install --user west"
        } else {
            Write-Host "  python -m pip install --user west"
        }
    } else {
        Write-Host "Python was not found. Install Python first." -ForegroundColor Red
    }
}

if (-not $hasCmake) { Write-Host "Install CMake or add it to PATH." -ForegroundColor Yellow }
if (-not $hasNinja) { Write-Host "Install Ninja or add it to PATH." -ForegroundColor Yellow }
if (-not $hasDtc) { Write-Host "Install Devicetree Compiler (dtc) or add it to PATH." -ForegroundColor Yellow }

$defaultZephyrBase = Join-Path $env:USERPROFILE "zephyrproject\zephyr"
if ($env:ZEPHYR_BASE -and (Test-Path $env:ZEPHYR_BASE)) {
    Write-Host "[OK] ZEPHYR_BASE is set"
} elseif (Test-Path $defaultZephyrBase) {
    Write-Host "[OK] Default Zephyr checkout found under user profile"
    Write-Host "The build/run scripts will set ZEPHYR_BASE for this project session."
} else {
    Write-Host "[INFO] ZEPHYR_BASE is not set or does not exist." -ForegroundColor Yellow
    Write-Host "Open a Zephyr environment shell or set ZEPHYR_BASE to your Zephyr checkout."
}

Write-Host ""
Write-Host "Build command: scripts\windows\build_zephyr.bat"
Write-Host "Run command:   scripts\windows\run_zephyr.bat"
