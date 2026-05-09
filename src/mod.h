#pragma once

#include <string>

// Init
void init_packets();

// Packets
struct ServerSideNetworkHandler;
struct RakNet_RakNetGUID;
void tell(const ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, const std::string &message /* In UTF-8 */);

// Commands
bool handle_command(ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid, std::string command /* In CP-437 */);
