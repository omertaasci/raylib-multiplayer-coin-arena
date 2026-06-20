#include "coin.h"
#include "raylib.h"

void Coin_Init(CoinState *coin, float radius, int screenWidth, int screenHeight)
{
    coin->radius = radius; // sets the coin size to radius
    coin->activate = 1; // means coin is there

    Coin_Respawn(coin, screenWidth, screenHeight); // calls the func to put coin somewhere random on the screen
}

void Coin_Respawn(CoinState *coin, int screenWidth, int screenHeight)
{
    // picks a random x,y position for the coin

    // uses coin radius to make sure coin doesnt go off the edges
    int minX = (int)coin->radius;
    int maxX = screenWidth - (int)coin->radius;

    int minY = (int)coin->radius;
    int maxY = screenHeight - (int)coin->radius;

    coin->position.x = (float)GetRandomValue(minX, maxX);
    coin->position.y = (float)GetRandomValue(minY, maxY);
}

int Coin_CheckPlayerCollision(const PlayerState *player, const CoinState *coin)
{
    if (!coin->activate)
    {
        return 0;
    }
    
    // difference between player and coin positions
    float dx = player->position.x - coin->position.x; 
    float dy = player->position.y - coin->position.y;

    float distanceSquared = dx * dx + dy * dy;
    float radiusSum = player->radius + coin->radius;

    return distanceSquared <= radiusSum * radiusSum;
}

void Coin_Draw(const CoinState *coin)
{
    // if coin is active it appears on screen in gold color

    if (!coin->activate)
    {
        return;
    }

    Vector2 drawPosition = {
        coin->position.x,
        coin->position.y
    };

    DrawCircleV(drawPosition, coin->radius, GOLD);
}