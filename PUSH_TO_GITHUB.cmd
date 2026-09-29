@echo off
setlocal
cd /d "%~dp0"
if not exist .git\config git init -b main
if not "x%~1"=="x" git remote set-url origin "%~1"
if "x%~1"=="x" (
  git remote get-url origin >nul 2>&1 || git remote add origin https://github.com/amran13project/ForgeEngine.git
)
git add .
git commit -m "chore: publish Forge Engine Professional 3.0 creator platform foundation" 2>nul
if errorlevel 1 echo Nothing new to commit.
git branch -M main
git push -u origin main
pause
