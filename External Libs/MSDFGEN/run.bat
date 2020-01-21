@echo off
SETLOCAL ENABLEDELAYEDEXPANSION
for /l %%x in (33, 1, 126) do (
	cmd /c exit %%x
	set t=!=exitcodeAscii!
	echo !t!
	msdfgen.exe msdf -font C:\Windows\Fonts\arialbd.ttf %%x -o !t!.png -size 32 32 -pxrange 4 -autoframe
)
pause