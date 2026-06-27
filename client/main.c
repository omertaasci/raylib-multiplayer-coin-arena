#include "raylib.h"
#include "game.h"
#include "network.h"
#include "../shared/types.h"

int main(void)
{
    Network_InitSystem();
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib Multiplayer Coin Arena");
    SetTargetFPS(60);

    Game game;
    Game_Init(&game);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        Game_Update(&game, dt);

        BeginDrawing();
        Game_Draw(&game);
        EndDrawing();
    }
    Game_ShutDown(&game);
    CloseWindow();

    Network_ShutdownSystem();

    return 0;
}