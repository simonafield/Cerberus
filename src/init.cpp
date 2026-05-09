#include "mod.h"
#include "config/config.h"

#include <libreborn/config.h>
#include <libreborn/log.h>

// Init
__attribute__((constructor)) static void init()
{
    if (!reborn_is_server())
    {
        IMPOSSIBLE();
    }
    init_packets();
    get_welcome_messages();
    init_webhook();
}