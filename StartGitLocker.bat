@echo off
cd /d "%~dp0"
cd GitLock
python.exe UnrealLockFile.py
pause