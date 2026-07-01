#include "client_session.h"

void ClientSession_Init(ClientSession *session)
{
    session->socket = 0;
    session->active = 0;
    session->playerId = -1;
}