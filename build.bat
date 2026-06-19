@echo off

REM main.c dosyasını game.exe olarak derle
gcc main.c -o game.exe -I C:\raylib\raylib\src -L C:\raylib\raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm

REM Eğer hata varsa buraya girer
if %errorlevel% neq 0 (
    echo Derleme hatasi oldu.
    pause
    exit
)

REM Hata yoksa oyunu çalıştırır
game.exe

pause