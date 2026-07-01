#include "world.h"
#include <stdlib.h>
#include <string.h>

#define WORLD_WIDTH 900
#define WORLD_HEIGHT 600

static int RandomRange(int min, int max)
{
    return min + rand() % (max - min + 1);
}

static void RespawnCoin(ServerWorld *world)
{
    int r = (int)world->coin.radius;

    world->coin.x = (float)RandomRange(r, WORLD_WIDTH - r);
    world->coin.y = (float)RandomRange(r, WORLD_HEIGHT - r);
}

static int CheckPlayerCoinCollision(ServerPlayer *player, ServerCoin *coin)
{
    float dx = player->x - coin->x; // diffirence between player x and coin x
    float dy = player->y - coin->y; // diffirence between player y and coin y

    float distanceSquared = dx * dx + dy * dy;
    float radiusSum = player->radius + coin->radius;

    return distanceSquared <= radiusSum * radiusSum;
}

// make sure that player doesnt leave the map
static void KeepPlayerInBounds(ServerPlayer *player)
{
    if (player->x < player->radius) // left wall
    {
        player->x = player->radius;
    }

    if (player->x > WORLD_WIDTH - player->radius) // right wall
    {
        player->x = WORLD_WIDTH - player->radius ;
    }

    if (player->y < player->radius) // top wall
    {
        player->y = player->radius;
    }

    if (player->y > WORLD_HEIGHT - player->radius) // bottom wall
    {
        player->y = WORLD_HEIGHT - player->radius;
    }  
}


void World_Init(ServerWorld *world)
{
    memset(world, 0, sizeof(ServerWorld));

    world->nextPlayerId = 1;
    world->coin.radius = 10.0f;
    world->coin.active = 1;
    RespawnCoin(world);
}

int World_AddPlayer(ServerWorld *world)
// searches for empty player slot
// when finds empty spot creates a player
// gives init values
// returns player id
{
    for (int i = 0; i < MAX_PLAYERS; i++) // checks every player slot
    {
        if (!world->players[i].active)
    {
        ServerPlayer *player = &world->players[i];

        player->id = world->nextPlayerId;
        world->nextPlayerId++;

        player->x = 100.0f + i * 40.0f;
        player->y = 300.0f;
        player->radius = 20.0f;
        player->speed = 250.0f;
        player->score = 0;
        player->active = 1;

        memset(&player->input, 0, sizeof(InputPacket));

        return player->id;
    }
    }
    return -1;
}

void World_RemovePlayer(ServerWorld *world, int playerId)
{
    for (int i = 0; i < MAX_PLAYERS; i++) // checks every player slot
    {
        if (world->players[i].active && world->players[i].id == playerId) // checks if this is the correct active player
        {
            world->players[i].active = 0; // disactivate
            return;
        }
    }
}

void World_SetInput(ServerWorld *world, int playerId, InputPacket input)
// finds the correct player
// updates player input data
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (world->players[i].active && world->players[i].id == playerId)
        // checks if this is the correct active player
        {
            world->players[i].input = input;
            return;
        }
    }
}

void World_Update(ServerWorld *world, float dt)
// updates all active players
// moves players using input data
// keeps players inside map bounds
// checks coin collision and updates score
{
    for (int i = 0; i < MAX_PLAYERS; i++) // checks all player slots
    {
        ServerPlayer *player = &world->players[i];
        if (!player->active) // skips inactive players
        {
            continue;
        }
        if (player->input.up) // moves player up
        {
            player->y -= player->speed * dt;
        }
        if (player->input.down) // moves player down
        {
            player->y += player->speed * dt;
        }
        if (player->input.left) // moves player left
        {
            player->x -= player->speed * dt;
        }
        if (player->input.right) // moves player right
        {
            player->x += player->speed * dt;
        }
        KeepPlayerInBounds(player); // prevents leaving map
        if (world->coin.active && CheckPlayerCoinCollision(player, &world->coin))
        // checks if player touched the coin
        {
            player->score++;
            RespawnCoin(world);
        }
    }
}

void World_BuildState(ServerWorld *world, WorldStatePacket *outState)
// creates game state packet
// copies coin and player data
// sends only active players
{
    memset(outState, 0, sizeof(WorldStatePacket)); // clears old packet data

    outState->coin.x = world->coin.x;
    outState->coin.y = world->coin.y;
    outState->coin.radius = world->coin.radius;
    outState->coin.active = world->coin.active;
    // copies coin data

    int count = 0;

    for (int i = 0; i < MAX_PLAYERS; i++) // checks all player slots
    {
        if (!world->players[i].active)
        {
        continue;
        }

        outState->players[count].id = world->players[i].id;
        outState->players[count].x = world->players[i].x;
        outState->players[count].y = world->players[i].y;
        outState->players[count].radius = world->players[i].radius;
        outState->players[count].score = world->players[i].score;
        outState->players[count].active = 1;
        // copies player data into packet

        count++;
    }
    outState->player_count = count; // saves active player count
}