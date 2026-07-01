#ifndef GAME_H
#define GAME_H

#include "../shared/types.h"
#include "../shared/protocol.h"
#include "network.h"

typedef struct Game
{
    PlayerState offlinePlayer; // offline player
    CoinState offlineCoin; // coin

    NetworkClient network; // server connection
    int onlineMode; // 1 online, 0 offline
    int playerId;

    WorldStatePacket worldState;

} Game;

void Game_Init(Game *game);
void Game_Restart(Game *game);
void Game_Update(Game *game, float dt);
void Game_Draw(const Game *game);
void Game_ShutDown(Game *game);

#endif