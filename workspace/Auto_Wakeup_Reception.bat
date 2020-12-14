@echo off

echo ### コンピュータ名取得 ###
FOR /F %%A IN ('hostname.exe') DO SET NAME=%%A
echo %NAME%

//echo ### NamingService起動 ###
//cd C:\Program Files (x86)\OpenRTM-aist\1.1.2\bin
//start /MIN rtm-naming.bat

timeout /T 3

echo ### Rtshellを使用準備 ###
set PATH=C:\Python27;C:\Python27\Scripts;%PATH%
rtfind localhost

rem call rtcwd localhost/

rem set RTCTREE_NAMESERVERS=localhost
rem call rtls
timeout /T 3

echo ###wake RTCs###

cd URG\Release
start urgcomp.exe

timeout /T 1

cd ..\..\Human_Tracking\build\src\Debug
start human_trackingcomp.exe

timeout /T 1

cd ..\..\..\..\ReceptionCommander
start /MIN python ReceptionCommander.py

timeout /T 1

cd ..\logger
start /MIN python logger.py

timeout /T 1

cd ..\gaussian_process_multi
start /MIN python gaussian_process_multi.py

timeout /T 1

echo ### RTCポート接続 ###
cd ..\
rtresurrect sys.rtsys
timeout /T 5
