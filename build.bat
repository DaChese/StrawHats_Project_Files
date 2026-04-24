@echo off
setlocal EnableDelayedExpansion

echo.
echo === Straw Hats ChocAn - Build ===
echo.

:: ── 1. Find cmake ────────────────────────────────────────────────────────────

set CMAKE_EXE=

:: Check PATH first
where cmake >nul 2>&1
if %ERRORLEVEL% == 0 (
    set CMAKE_EXE=cmake
    goto :found
)

:: Search common Visual Studio install locations (2022 + 2019, all editions)
for %%E in (Community Professional Enterprise BuildTools) do (
    for %%Y in (2022 2019) do (
        set TRY="C:\Program Files\Microsoft Visual Studio\%%Y\%%E\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
        if exist !TRY! (
            set CMAKE_EXE=!TRY!
            goto :found
        )
    )
)

:: Check default standalone CMake install location
if exist "C:\Program Files\CMake\bin\cmake.exe" (
    set CMAKE_EXE="C:\Program Files\CMake\bin\cmake.exe"
    goto :found
)

echo ERROR: cmake not found.
echo.
echo Install one of:
echo   - Visual Studio 2019/2022 with "Desktop development with C++" workload
echo   - CMake from https://cmake.org/download  (tick "Add to PATH")
echo.
exit /b 1

:found
echo Found cmake: %CMAKE_EXE%
echo.

:: ── 2. Configure ─────────────────────────────────────────────────────────────

%CMAKE_EXE% -S . -B build -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo.
    echo ERROR: CMake configure failed.
    exit /b 1
)

:: ── 3. Build ──────────────────────────────────────────────────────────────────

%CMAKE_EXE% --build build --config Release
if %ERRORLEVEL% neq 0 (
    echo.
    echo ERROR: Build failed.
    exit /b 1
)

:: ── 4. Done ───────────────────────────────────────────────────────────────────

echo.
echo ============================================================
echo  Build successful!
echo.
echo  Run the app:
echo    Windows (Release):  build\Release\strawhats.exe --data-dir data
echo    Windows (Debug):    build\Debug\strawhats.exe   --data-dir data
echo ============================================================
echo.
