@echo off
setlocal
cd /d "%~dp0"
echo This legacy bundle helper is disabled.
echo It referenced an old bundle and pushed directly to main.
echo No changes were applied. Use a reviewed feature branch and follow GIT_WORKFLOW.md.
exit /b 2
