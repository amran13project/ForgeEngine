@echo off
setlocal
cd /d "%~dp0"
if "%~1"=="" (
  echo Usage: Run-ForgeRuntime.cmd "C:\Path\To\ForgeProject"
  pause
  exit /b 64
)
if not exist "%~dp0bin\ForgeRuntime.exe" (
  echo ForgeRuntime.exe not found. Build it first with Build-ForgeEngine-Windows.cmd
  pause
  exit /b 1
)
"%~dp0bin\ForgeRuntime.exe" "%~1"
