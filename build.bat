@echo off

echo Building client...

gcc ^
client/main.c ^
client/game.c ^
client/player.c ^
client/coin.c ^
client/ui.c ^
client/network.c ^
-o client.exe ^
-I client ^
-I shared ^
-I C:\raylib\raylib\src ^
-L C:\raylib\raylib\src ^
-lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32

if %errorlevel% neq 0 (
    echo Client build failed.
    pause
    exit /b
)

echo Building server...

gcc ^
server/main.c ^
server/server.c ^
server/world.c ^
server/client_session.c ^
-o server.exe ^
-I server ^
-I shared ^
-lws2_32

if %errorlevel% neq 0 (
    echo Server build failed.
    pause
    exit /b
)

echo Build successful.
echo Run server.exe in one terminal, then client.exe in another terminal.

pause