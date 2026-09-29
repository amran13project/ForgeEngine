@echo off
setlocal
cd /d "%~dp0"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Build-ForgeEngine-Windows.ps1"
if errorlevel 1 (
  echo.
  echo Forge Engine build failed.
  pause
  exit /b 1
)
echo.
echo Forge Engine build completed successfully.
pause
