#ifndef WORLD_H
#define WORLD_H

#include "../shared/protocol.h"

typedef struct ServerPlayer
{
    int id;
    float x;
    float y;
    float radius;
    float speed;
    int score;
    int active;
    InputPacket input;
} ServerPlayer;

typedef struct ServerCoin
{
    float x;
    float y;
    float radius;
    int active;
} ServerCoin;

typedef struct ServerWorld
{
    ServerPlayer players[MAX_PLAYERS];
    ServerCoin coin;
    int nextPlayerId;
} ServerWorld;

void World_Init(ServerWorld *world);
int World_AddPlayer(ServerWorld *world);
int World_RemovePlayer(ServerWorld *world, int playerId);
int World_SetInput(ServerWorld *world, int playerId, InputPacket input);
void World_Update(ServerWorld *world, float dt);
void World_BuildState(ServerWorld *world, WorldStatePacket *outState);

#endif