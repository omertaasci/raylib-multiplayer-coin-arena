#include "coin.h"
#include "raylib.h"

void Coin_Init(CoinState *coin, float radius, int screenWidth, int screenHeight)
{
    coin->radius = radius;
    coin->activate = 1;

    Coin_Respawn(coin, screenWidth, screenHeight);
}

void Coin_Respawn(CoinState *coin, int screenWidth, int screenHeight)
{
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
    
    float dx = player->position.x - coin->position.x;
    float dy = player->position.y - coin->position.y;

    float distanceSquared = dx * dx + dy * dy;
    float radiusSum = player->radius + coin->radius;

    return distanceSquared <= radiusSum * radiusSum;
}

void Coin_Draw(const CoinState *coin)
{
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