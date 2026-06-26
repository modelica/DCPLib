@echo off
set back=%cd%
set BUILD_CONFIG=Release

if not exist dependencies (mkdir dependencies)
cd dependencies

WHERE vcpkg --version >nul 2>nul
REM IF %ERRORLEVEL% NEQ 0 (ECHO vcpkg was not found && cd %back% && EXIT /b 1)
IF %ERRORLEVEL% NEQ 0 (
  if not exist %cd%\vcpkg\scripts\buildsystems\vcpkg.cmake (
    ECHO vcpkg was not found, installing
    if not exist vcpkg (git clone https://github.com/microsoft/vcpkg.git)
    cd vcpkg
    if not exist vcpkg.exe ( bootstrap-vcpkg.bat -disableMetrics )
    set VCPKG=%cd%\vcpkg\vcpkg.exe
    %VCPKG% integrate install || ECHO Installed vcpkg locally
  )
) ELSE (
  ECHO vcpkg found
  set VCPKG=vcpkg
)

%VCPKG% --version

cd %back%

cd dependencies
WHERE cmake --version >nul 2>nul
REM IF %ERRORLEVEL% NEQ 0 (ECHO cmake was not found && cd %back% && EXIT /b 1)
IF %ERRORLEVEL% NEQ 0 (
  ECHO cmake was not found, installing
  if not exist cmake.zip ( curl -L -o cmake.zip https://github.com/Kitware/CMake/releases/download/v4.0.0-rc5/cmake-4.0.0-rc5-windows-x86_64.zip )
  REM if not exist cmake (tar -xf cmake.zip)
  if not exist cmake (mkdir cmake && tar -xf cmake.zip --strip-components=1 -C cmake)
  cd cmake
  cd bin
  set CMAKE=%cd%\cmake\bin\cmake.exe
) ELSE (
  ECHO cmake found
  set CMAKE=cmake
)

%CMAKE% --version

cd %back%

%VCPKG% install asio xerces-c libzip

if not exist build (mkdir build)

cd build
if not exist install (mkdir install)
set VCPKG_TC_FILE=C:/Users/%USERNAME%/Downloads/dcp_PR/DCPLib/dependencies/vcpkg/scripts/buildsystems/vcpkg.cmake
%CMAKE% .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=%VCPKG_TC_FILE% -DCMAKE_INSTALL_PREFIX=install

%CMAKE% --build . --target=install --config %BUILD_CONFIG%

cd %back%


REM Build Examples

for /f "tokens=*" %%G in ('dir /b /a:d "example\*"') do (
  echo Found %back%\example\%%G
  if not exist %back%\build\example\%%G (mkdir %back%\build\example\%%G)
  cd %back%\build\example\%%G
  %CMAKE% %back%\example\%%G -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=%VCPKG_TC_FILE% -DDCPLib_DIR=%back%\build\install\lib\DCPLib -DCMAKE_INSTALL_PREFIX=install
  %CMAKE% --build . --config %BUILD_CONFIG%
  REM copy /Y %back%\example\%%G\*.xml %back%\build\example\%%G
  cd %back%
)

REM make sure the XML File is available for any build configuration!!! (e.g. master needs one)

run_test.bat %cd% %BUILD_CONFIG%