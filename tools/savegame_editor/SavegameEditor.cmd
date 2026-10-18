@echo off
setlocal
where py >nul 2>nul
if not errorlevel 1 (
    py -3 "%~dp0editor.py" %*
    if errorlevel 1 pause
    exit /b
)
if exist "%LocalAppData%\Programs\Python" (
    for /d %%P in ("%LocalAppData%\Programs\Python\Python3*") do (
        if exist "%%~fP\python.exe" (
            "%%~fP\python.exe" "%~dp0editor.py" %*
            if errorlevel 1 pause
            exit /b
        )
    )
)
if exist "%UserProfile%\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe" (
    "%UserProfile%\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe" "%~dp0editor.py" %*
    if errorlevel 1 pause
    exit /b
)
echo Python 3.10 or newer with Tkinter is required.
echo Install Python with its launcher, then run this file again.
pause
