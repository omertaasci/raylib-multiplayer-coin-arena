#include "game.h"
#include "player.h"
#include "coin.h"
#include "ui.h"
#include "raylib.h"

void Game_Init(Game *game)
{
    Player_Init(
        &game->player,
        1,
        SCREEN_WIDTH / 2.0f,
        SCREEN_HEIGHT / 2.0f
    );

    Coin_Init(
        &game->coin,
        10.0f,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );
}

void Game_Restart(Game *game)
{
    Game_Init(game);
}

void Game_Update(Game *game, float dt)
{
    if (IsKeyPressed(KEY_R))
    {
        Game_Restart(game);
    }

    Player_UpdateOffline(&game->player, dt);
    Player_KeepInBounds(&game->player, SCREEN_WIDTH, SCREEN_HEIGHT);

    // if detects collision increase score and respawn coin
    if (Coin_CheckPlayerCollision(&game->player, &game->coin))
    {
        game->player.score++;
        Coin_Respawn(&game->coin, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
}

void Game_Draw(const Game *game)
{
    ClearBackground(RAYWHITE);

    Coin_Draw(&game->coin);
    Player_Draw(&game->player);
    UI_DrawHud(game->player.score);
}