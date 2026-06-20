#include "player.h"
#include "raylib.h"

void Player_Init(PlayerState *player, int id, float x, float y) // Initialize player
{
    player->id = id; // player id
    player->position.x = x; // start x pos
    player->position.y = y; // start y pos
    player->radius = 20.0f; // player radius
    player->speed = 250.0f; // speed per second
    player->score = 0; // start score
    player->activate = 1; // player active
}

// player movement
void Player_UpdateOffline(PlayerState * player, float dt)
{
    if (IsKeyDown(KEY_W))
    {
        player->position.y -= player->speed * dt;
    }

    if (IsKeyDown(KEY_S))
    {
        player->position.y += player->speed * dt;
    }
    
    if (IsKeyDown(KEY_A))
    {
        player->position.x -= player->speed * dt;
    }

    if (IsKeyDown(KEY_D))
    {
        player->position.x += player->speed * dt;
    }
}

// prevents player from leaving the screen
void Player_KeepInBounds(PlayerState *player, int screenWidth, int screenHeight)
{
    if (player->position.x < player->radius)
    {
        player->position.x = player->radius;
    }
    
    if (player->position.x > screenWidth - player->radius)
    {
        player->position.x = screenWidth - player->radius;
    }

    if (player->position.y < player->radius)
    {
        player->position.y = player->radius;
    }

    if (player->position.y > screenHeight - player->radius)
    {
        player->position.y = screenHeight - player->radius;
    }
}

// draw player
void Player_Draw(const PlayerState *player)
{
    Vector2 drawPosition = {
        player->position.x,
        player->position.y
    };

    DrawCircleV(drawPosition, player->radius, BLUE);
}