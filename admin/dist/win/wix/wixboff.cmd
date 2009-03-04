@echo off
rem Requires Wix 3.0.x
rem Run with current directory same as .cmd location

echo WIX = %WIX%
echo QTDIR = %QTDIR%

"%WIX%"\bin\candle boffin.wxs
if ERRORLEVEL 1 goto ERROR

"%WIX%"\bin\light -ext WixUIExtension boffin.wixobj -b ..\..\..\..\_bin\ -b %QTDIR%\bin -b %QTDIR%\plugins -b "c:\Program Files\Common Files\Merge Modules\\"
if ERRORLEVEL 1 goto ERROR

goto END

:ERROR
echo "***Fail"

:END