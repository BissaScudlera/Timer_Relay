@echo off
:loop
cls
echo ==== %date% %time% ====
powershell -Command "[System.IO.Ports.SerialPort]::GetPortNames()"
timeout /t 3 >nul
goto loop