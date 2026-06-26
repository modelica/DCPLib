set back=%1
set BUILD_CONFIG=%2

echo > waiting.tmp
start cmd /c wrapper.bat %back%\build\example\slave\%BUILD_CONFIG% %back%\build\example\slave\%BUILD_CONFIG%\dcpslave.exe
cd %back%\build\example\master\%BUILD_CONFIG%
start /wait %back%\build\example\master\%BUILD_CONFIG%\dcpmaster.exe

:loop
if exist waiting.tmp goto :loop

taskkill /IM dcpslave.exe /F
cd %back%