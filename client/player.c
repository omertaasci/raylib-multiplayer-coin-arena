#include "player.h"
#include "raylib.h"

void Player_Init(PlayerState *player, int id, float x, float y) 
{
    player->id = id; // player id
    player->position.x = x; // start x pos
    player->position.y = y; // start y pos
    player->radius = 20.0f; // player radius
    player->speed = 250.0f; // speed per second
    player->score = 0; // start score
    player->activate = 1; // player active
}

void Player_UpdateOffline(PlayerState * player, float dt)
{
    if (IsKeyDown(KEY_W))
    {
        player->position.y += player->speed * dt;
    }
    
}