#include "mod.h"
#include "config/config.h"

#include <libreborn/patch.h>
#include <libreborn/util/string.h>
#include <libreborn/util/util.h>

#include <mods/chat/chat.h>

#include <symbols/RakNet_RakNetGUID.h>
#include <symbols/ServerSideNetworkHandler.h>
#include <symbols/ChatPacket.h>
#include <symbols/ReadyPacket.h>
#include <symbols/Packet.h>
#include <symbols/MessagePacket.h>
#include <symbols/RakNetInstance.h>
#include <symbols/Level.h>

// Handle Connection
void tell(const ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, const std::string &message)
{
    MessagePacket *packet = MessagePacket::allocate();
    ((Packet *)packet)->constructor();
    packet->vtable = MessagePacket::VTable::base;
    packet->message.constructor();
    packet->message.Assign(to_cp437(message).c_str());
    packet->reliability = RELIABLE_ORDERED;
    self->rak_net_instance->sendTo(guid, *(Packet *)packet);
    packet->destructor_deleting();
}
static void ServerSideNetworkHandler_onReady_ClientGeneration_injection(ServerSideNetworkHandler_onReady_ClientGeneration_t original, ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid)
{
    // Call Original Method
    original(self, guid);

    // Send Welcome Message
    const std::vector<std::string> &welcome = get_welcome_messages();
    for (const std::string &line : welcome)
    {
        tell(self, guid, line);
    }
    const int count = self->level ? int(self->level->players.size()) : 0;
    std::string message = "There ";
    if (count == 1)
    {
        message += "is " + std::to_string(count) + " player";
    }
    else
    {
        message += "are " + std::to_string(count) + " players";
    }
    message += " online.";
    tell(self, guid, message);
}

// Handle Chat Commands
HOOK(ServerSideNetworkHandler_handle_ChatPacket_injection, void, (ServerSideNetworkHandler * server_side_network_handler, const RakNet_RakNetGUID &guid, ChatPacket *chat_packet))
{
    const bool ret = handle_command(server_side_network_handler, guid, chat_packet->message);
    if (!ret)
    {
        // Call Original Method
        real_ServerSideNetworkHandler_handle_ChatPacket_injection()(server_side_network_handler, guid, chat_packet);
    }
}

// Init
void init_packets()
{
    // Welcome Message
    overwrite_calls(ServerSideNetworkHandler_onReady_ClientGeneration, ServerSideNetworkHandler_onReady_ClientGeneration_injection);
}
