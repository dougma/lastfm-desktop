rem @echo off
rem Requires Wix 3.0.x
rem Run with current directory same as .cmd location

rem set VCREDISTDIR=C:\Program Files\Microsoft Visual Studio 8\SDK\v2.0\BootStrapper\Packages\vcredist_x86\
set VCREDISTDIR=C:\Program Files\Common Files\Merge Modules\

echo WIX = %WIX%
echo QTDIR = %QTDIR%
echo VCREDISTDIR = %VCREDISTDIR%

"%WIX%"\bin\candle boffin.wxs
if ERRORLEVEL 1 goto ERROR

"%WIX%"\bin\light -ext WixUIExtension boffin.wixobj -b ..\..\..\..\_bin\ -b %QTDIR%\bin -b %QTDIR%\plugins -b "%VCREDISTDIR%\"
if ERRORLEVEL 1 goto ERROR

goto END

:ERROR
echo "***Fail"

:END