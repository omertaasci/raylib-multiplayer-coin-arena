#ifndef TYPES_H
#define TYPES_H

// This file contains type definitions that are used across the project

// game screen width and height
#define SCREEN_WIDTH 900 
#define SCREEN_HEIGHT 600 

// Vector type to store 2d position
typedef struct Vec2 {
    float x;
    float y;
} Vec2;

// Player state struct to store player information
typedef struct PlayerState {
    int id; // player id
    Vec2 position; // player position
    float radius; // player radius
    float speed; // player speed
    int score; // player score
    int activate; // whether the player is active or not
}PlayerState;

// Coin state struct to store coin information
typedef struct CoinState {
    Vec2 position; // coin position
    float radius; // coin radius
    int activate; // whether the coin is active or not
} CoinState;

#endif