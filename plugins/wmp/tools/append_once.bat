REM %1 - string to append
REM %2 - file to append it to
find /C /I %1 %2
if not errorlevel 1 goto end
    echo start /WAIT "" %1 /SILENT >> %2
:end