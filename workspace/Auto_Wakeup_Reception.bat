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

cd C:\workspace\URG\Release
start urgcomp.exe

timeout /T 1

cd C:\workspace\Human_Tracking\build\src\Debug
start human_trackingcomp.exe

timeout /T 1

cd C:\workspace\ReceptionCommander
start /MIN python ReceptionCommander.py

timeout /T 1

cd C:\workspace\logger
start /MIN python logger.py

timeout /T 1

cd C:\workspace\gaussian_process_multi
start /MIN python gaussian_process_multi.py

timeout /T 1

echo ### RTCポート接続 ###
call rtcon /localhost/%NAME%.host_cxt/URG0.rtc:RangeData /localhost/%NAME%.host_cxt/Human_Tracking0.rtc:range_data

timeout /T 1

//loggerに接続
call rtcon /localhost/%NAME%.host_cxt/ReceptionCommander0.rtc:logger_cmd /localhost/%NAME%.host_cxt/logger0.rtc:flag
call rtcon /localhost/%NAME%.host_cxt/Human_Tracking0.rtc:human_dataX /localhost/%NAME%.host_cxt/logger0.rtc:human_centerX
call rtcon /localhost/%NAME%.host_cxt/Human_Tracking0.rtc:human_dataY /localhost/%NAME%.host_cxt/logger0.rtc:human_centerY
call rtcon /localhost/%NAME%.host_cxt/Human_Tracking0.rtc:human_edge1x /localhost/%NAME%.host_cxt/logger0.rtc:human_edge1X
call rtcon /localhost/%NAME%.host_cxt/Human_Tracking0.rtc:human_edge1y /localhost/%NAME%.host_cxt/logger0.rtc:human_edge1Y
call rtcon /localhost/%NAME%.host_cxt/Human_Tracking0.rtc:human_edge2x /localhost/%NAME%.host_cxt/logger0.rtc:human_edge2X
call rtcon /localhost/%NAME%.host_cxt/Human_Tracking0.rtc:human_edge2y /localhost/%NAME%.host_cxt/logger0.rtc:human_edge2Y
call rtcon /localhost/%NAME%.host_cxt/ReceptionCommander0.rtc:human_num /localhost/%NAME%.host_cxt/logger0.rtc:human_in

timeout /T 1

//ReceptionCommanderに接続
call rtcon /localhost/%NAME%.host_cxt/Human_Tracking0.rtc:human_dataX /localhost/%NAME%.host_cxt/ReceptionCommander0.rtc:human_x
call rtcon /localhost/%NAME%.host_cxt/Human_Tracking0.rtc:human_dataY /localhost/%NAME%.host_cxt/ReceptionCommander0.rtc:human_y
call rtcon /localhost/%NAME%.host_cxt/Human_Tracking0.rtc:human_edge1x /localhost/%NAME%.host_cxt/ReceptionCommander0.rtc:human_edge1X
call rtcon /localhost/%NAME%.host_cxt/Human_Tracking0.rtc:human_edge1y /localhost/%NAME%.host_cxt/ReceptionCommander0.rtc:human_edge1Y
call rtcon /localhost/%NAME%.host_cxt/Human_Tracking0.rtc:human_edge2x /localhost/%NAME%.host_cxt/ReceptionCommander0.rtc:human_edge2X
call rtcon /localhost/%NAME%.host_cxt/Human_Tracking0.rtc:human_edge2y /localhost/%NAME%.host_cxt/ReceptionCommander0.rtc:human_edge2Y
call rtcon /localhost/%NAME%.host_cxt/Human_Tracking0.rtc:people_wait /localhost/%NAME%.host_cxt/ReceptionCommander0.rtc:wait_people

timeout /T 1

//gaussian_process_multiに接続
call rtcon /localhost/%NAME%.host_cxt/ReceptionCommander0.rtc:gauss_cmd /localhost/%NAME%.host_cxt/gaussian_process_multi0.rtc:flag
call rtcon /localhost/%NAME%.host_cxt/ReceptionCommander0.rtc:human_num /localhost/%NAME%.host_cxt/gaussian_process_multi0.rtc:human_in
call rtcon /localhost/%NAME%.host_cxt/logger0.rtc:filename /localhost/%NAME%.host_cxt/gaussian_process_multi0.rtc:filename


REM ### アクティベートがうまくいかないときなど、OpenRTPを起動して手動でアクティベートする
rem set PATH=%PROG%\OpenRTM-aist\1.1.2\bin\jre\bin;%PATH%
rem cd %PROG86%\OpenRTM-aist\1.1.2\utils\OpenRTP
rem start eclipse.exe

timeout /T 3
rem PAUSE


