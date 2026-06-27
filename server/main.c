#include "server.h"
#include "../shared/protocol.h"

int main(void)
{
    Server_Run(SERVER_PORT);
    return 0;
}