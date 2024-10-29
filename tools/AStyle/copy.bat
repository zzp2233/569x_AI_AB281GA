@echo off
REM 获取当前脚本所在目录
set SCRIPT_DIR=%~dp0

REM 进入AB5691_SDK目录
cd /d %SCRIPT_DIR%\..\..

copy tools\AStyle\pre-commit .git\hooks\pre-commit /Y

@REM pause
