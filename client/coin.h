#ifndef COIN_H
#define COIN_H 

#include "../shared/types.h"

// set initial values of coin
void Coin_Init(CoinState *coin, float radius, int screenWidth, int screenHeight);

// moves coin to random position
void Coin_Respawn(CoinState *coin, int screenWidth, int screenHeight);

// checks player and coin collision
int Coin_CheckPlayerCollision(const PlayerState *player, const CoinState *coin);

// draws the coin to screen
void Coin_Draw(const CoinState *coin);

#endif