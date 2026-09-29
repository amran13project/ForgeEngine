@echo off
setlocal
cd /d "%~dp0"
set "FORGE_ENGINE_BIN=%~dp0bin"
if not exist "%~dp0bin\ForgeEngine.exe" (
  echo ForgeEngine.exe not found. Build it first with Build-ForgeEngine-Windows.cmd
  pause
  exit /b 1
)
start "Forge Engine Professional" "%~dp0bin\ForgeEngine.exe"
