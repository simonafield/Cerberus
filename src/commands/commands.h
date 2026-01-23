#pragma once

#include <string>
#include <vector>
#include <functional>

// Command Description
struct Command {
    std::string name;
    bool requires_admin = false;
    std::vector<std::string> args = {};
    std::function<std::vector<std::string>(const std::vector<std::string> &)> callback;
};

// Common Arguments
static constexpr const char *username_arg = "username";
// Common Errors
static constexpr const char *invalid_player = "Invalid Player";
static constexpr const char *invalid_username = "Invalid Username";
static constexpr const char *generated_password = "Generated Password: ";

// Check Username
bool is_username_valid(std::string username);

// Load Commands
struct ServerSideNetworkHandler;
struct RakNet_RakNetGUID;
void add_common_commands(std::vector<Command> &commands, ServerSideNetworkHandler *self);
void add_logged_out_commands(std::vector<Command> &commands, ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid);
void add_logged_in_commands(std::vector<Command> &commands, ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid);