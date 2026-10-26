@echo off
setlocal
cd /d "%~dp0"
for /f "delims=" %%B in ('git branch --show-current') do set "CURRENT_BRANCH=%%B"
if not defined CURRENT_BRANCH (
  echo No current Git branch was found. This helper will not initialize or rename the repository.
  exit /b 2
)
if /I "%CURRENT_BRANCH%"=="main" (
  echo Refusing to push main. Push a reviewed feature branch and open a pull request.
  exit /b 2
)
if not "x%~1"=="x" (
  git remote get-url origin >nul 2>&1
  if errorlevel 1 (git remote add origin "%~1") else (git remote set-url origin "%~1")
)
git remote get-url origin >nul 2>&1
if errorlevel 1 (
  echo No origin remote is configured. Configure the intended repository first.
  exit /b 2
)
echo Current branch: %CURRENT_BRANCH%
echo Review and commit only intended files before continuing.
git status --short
git push -u origin "%CURRENT_BRANCH%"
if errorlevel 1 (
  echo Feature-branch push failed. No main branch was changed.
  exit /b 1
)
echo Feature branch pushed. Open a pull request for review.
pause
