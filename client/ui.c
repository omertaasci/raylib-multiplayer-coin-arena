#include "ui.h"
#include "raylib.h"

void UI_DrawHud(int score, int onlineMode, int playerId)
{
    DrawText("Coin Arena - Offline Mode", 20, 20, 24, DARKBLUE);
    DrawText(TextFormat("Score: %d", score), 20, 55, 22, BLACK);

    if (onlineMode)
    {
        DrawText(TextFormat("Online | Player ID : %d", playerId), 20, 55, 22, BLACK);
    }
    else
    {
        DrawText("Offline Mode", 20, 85, 18, MAROON);
    }

    DrawText("Move: WASD | Restart: R | Exit: ESC", 20, 85, 18, DARKGRAY);
    DrawFPS(20, 115);
}