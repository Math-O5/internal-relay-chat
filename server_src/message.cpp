#include "message.h"

void msg_inicio(int status) {
    if(status == CONNECT) {
        system("clear");
        printf("  [ ]   ==========================\n");
        printf("  [+]   Welcome Relay Chat Server.\n");
        printf("  [ ]   ==========================\n");
        fflush(stdout);
    } else {
        system("clear");
        printf("  [ ]   ==========================\n");
        printf("  [x]   The  Relay Chat Server IS NOT RUNN.\n");
        printf("  [ ]   ==========================\n");
        fflush(stdout);
    }
}