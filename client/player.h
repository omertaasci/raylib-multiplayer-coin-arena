#ifndef PLAYER_H
#define PLAYER_H

#include "../shared/types.h"

// Function to initialize a player with given id and position
void Player_Init(PlayerState *player, int id, float x, float y);

// Function to update the player's state based on input and time delta
void Player_UpdateOffline(PlayerState *player, float dt);

// Function to make sure player is not leaving the screen
void Player_KeepInBounds(PlayerState *player, int screenWidth, int screenHeight);

// Draws the player on the screen
void Player_Draw(const PlayerState *player);


#endif