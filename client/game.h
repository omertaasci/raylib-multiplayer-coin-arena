#ifndef GAME_H
#define GAME_H

#include "../shared/types.h"

typedef struct Game
{
    PlayerState player; // offline player
    CoinState coin; // coin
} Game;

void Game_Init(Game *game);
void Game_Restart(Game *game);
void Game_Update(Game *game, float dt);
void Game_Draw(const Game *game);

#endif