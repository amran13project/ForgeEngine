@echo off
setlocal
cd /d "%~dp0"
set "BUNDLE=%~dp0ForgeEngine_FULL_2026-09-29.bundle"
if not exist "%BUNDLE%" (
  echo ForgeEngine_FULL_2026-09-29.bundle not found next to this script.
  pause
  exit /b 1
)

echo Fetching Forge Engine full changes from Git bundle...
git fetch "%BUNDLE%" refs/heads/forge-full-20260929:refs/remotes/forge-bundle/forge-full-20260929
if errorlevel 1 goto :fail

echo Applying the two new commits onto the existing GitHub main...
git cherry-pick 6933f2b 38353e5
if errorlevel 1 goto :fail

echo Pushing to GitHub main...
git push origin main
if errorlevel 1 goto :fail

echo.
echo Forge Engine full changes applied and pushed successfully.
pause
exit /b 0

:fail
echo.
echo Forge Engine update failed. No fake success is reported.
pause
exit /b 1
