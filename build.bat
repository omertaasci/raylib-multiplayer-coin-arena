@echo off
REM Bu dosya offline client tarafını derler.
REM Server, network ve database dosyaları bu aşamada derlenmez.
echo Building offline client...
gcc ^
client/main.c ^
client/game.c ^
client/player.c ^
client/coin.c ^
client/ui.c ^
-o client.exe ^
-I client ^
-I shared ^
-I C:\raylib\raylib\src ^
-L C:\raylib\raylib\src ^
-lraylib -lopengl32 -lgdi32 -lwinmm
if %errorlevel% neq 0 (
echo Build failed.
pause
exit /b
)
echo Build successful.
echo Starting client...
client.exe
pause