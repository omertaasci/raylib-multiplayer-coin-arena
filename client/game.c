#include "game.h"
#include "player.h"
#include "coin.h"
#include "ui.h"
#include "raylib.h"

static void DrawOnlineWorld(const Game *game);
static InputPacket BuildInputPacket(void);

void Game_Init(Game *game)
{
    Player_Init(
        &game->offlinePlayer,
        1,
        SCREEN_WIDTH / 2.0f,
        SCREEN_HEIGHT / 2.0f
    );

    Coin_Init(
        &game->offlineCoin,
        10.0f,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    NetworkClient_Init(&game->network);

    game->onlineMode = NetworkClient_Connect(
        &game->network,
        "127.0.0.1",
        SERVER_PORT
    );

    if (game->onlineMode)
    {
        game->offlinePlayer.id = game->network.player_id;
    }
}

void Game_ShutDown(Game *game)
{
    NetworkClient_Close(&game->network);
}

void Game_Restart(Game *game)
{
    Game_Init(game);
}

void Game_Update(Game *game, float dt)
{
    if (game->onlineMode)
    {
        InputPacket input = BuildInputPacket();
        NetworkClient_SendInput(&game->network, input);

        WorldStatePacket state;

        if (NetworkClient_ReceiveWorldState(&game->network, &state))
        {
            game->worldState = state;
        }
        return;
    }
    
    Player_UpdateOffline(&game->offlinePlayer, dt);
    Player_KeepInBounds(&game->offlinePlayer, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (Coin_CheckPlayerCollision(&game->offlinePlayer, &game->offlineCoin))
    {
        game->offlinePlayer.score++;
        Coin_Respawn(&game->offlineCoin, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
}

void Game_Draw(const Game *game)
{
    ClearBackground(RAYWHITE);

    if (game->onlineMode)
    {
        DrawOnlineWorld(game);
        DrawText("Online multiplayer", 20, 20, 24, DARKGREEN);
        DrawText(TextFormat("Your player id: %d", game->playerId), 20, 55, 20, BLACK);
    }
    else
    {
        Coin_Draw(&game->offlineCoin);
        Player_Draw(&game->offlinePlayer);
        UI_DrawHud(game->offlinePlayer.score, 0, game->offlinePlayer.id);
    }

    DrawFPS(20, 90);
}

static void DrawOnlineWorld(const Game *game)
{
    if (game->worldState.coin.active)
    {
        DrawCircle(
        (int)game->worldState.coin.x,
        (int)game->worldState.coin.y,
        game->worldState.coin.radius,
        GOLD
    );
    }

    for (int i = 0; i < game->worldState.player_count; i++)
    {
        NetPlayerState player = game->worldState.players[i];
        Color color = RED;
        if (player.id == game->playerId)
        {
            color = BLUE;
        }
        DrawCircle((int)player.x, (int)player.y, player.radius, color);
        DrawText(TextFormat("P%d", player.id), (int)player.x - 10, (int)player.y - 35, 16, BLACK);
        DrawText(TextFormat("%d", player.score), (int)player.x - 8, (int)player.y + 25, 16, BLACK);
    }
}

static InputPacket BuildInputPacket(void)
{
    InputPacket input;
    input.up = IsKeyDown(KEY_W);
    input.down = IsKeyDown(KEY_S);
    input.left = IsKeyDown(KEY_A);
    input.right = IsKeyDown(KEY_D);
    return input;
}
