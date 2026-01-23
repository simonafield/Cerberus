#include "commands.h"
#include "../config/config.h"

#include <random>

#include <mods/misc/misc.h>
#include <mods/server/server.h>

#include <symbols/ServerSideNetworkHandler.h>
#include <symbols/Level.h>

// Generate Random Password
static std::string generate_password() {
    // Constants
    constexpr int length = 12;
    constexpr std::array characters = {
        // This is not very secure, but these passwords
        // are meant to be immediately reset.
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
    };
    // Generate
    std::string password;
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);
    for (int i = 0; i < length; i++) {
        password += characters.at(distribution(generator));
    }
    // Return
    return password;
}

// Load Commands
void add_logged_in_commands(std::vector<Command> &commands, ServerSideNetworkHandler *self, const RakNet_RakNetGUID &guid) {
    // Delete Account
    commands.push_back({
        .name = "ban",
        .requires_admin = true,
        .args = {username_arg},
        .callback = [self](const std::vector<std::string> &args) {
            // Arguments
            const std::string &username = args[0];

            // Block Banning Admins
            if (is_admin(username)) {
                return std::vector<std::string>{"Unable To Ban Administrator"};
            }

            // Remove Account
            bool valid = delete_account(username);
            // Kick Players
            const Level *level = self->level;
            if (level) {
                for (Player *other : level->players) {
                    if (misc_get_player_username_utf(other) == username) {
                        server_kick((ServerPlayer *) other);
                        valid = true;
                    }
                }
            }

            // Return
            std::string message = invalid_player;
            if (valid) {
                message = "Banned: " + username;
            }
            return std::vector{message};
        }
    });

    // Create Account
    commands.push_back({
        .name = "register",
        .requires_admin = true,
        .args = {username_arg},
        .callback = [](const std::vector<std::string> &args) {
            // Arguments
            const std::string &username = args[0];
            const std::string &password = generate_password();

            // Run
            std::string message1;
            std::string message2;
            if (!is_username_valid(username)) {
                message1 = invalid_username;
            } else if (create_account(username, password)) {
                message1 = "Created: " + username;
                message2 = generated_password + password;
            } else {
                message1 = "Unable To Create Account";
            }

            // Return
            std::vector<std::string> out;
            out.push_back(message1);
            if (!message2.empty()) {
                out.push_back(message2);
            }
            return out;
        }
    });

    // Report Player
    commands.push_back({
        .name = "report",
        .args = {username_arg, "reason"},
        .callback = [self, &guid](const std::vector<std::string> &args) {
            // Arguments
            const std::string &target = args[0];
            const std::string &reason = args[1];

            // Get Reporter
            std::string reporter;
            const Player *player = self->getPlayer(guid);
            if (player) {
                reporter = misc_get_player_username_utf(player);
            }

            // Run
            std::string message;
            const bool valid = has_account(target);
            if (valid) {
                message = "Reported: " + target;
                send_to_discord("**" + reporter + " has reported " + target + " for:** " + reason, true);
            } else {
                message = invalid_player;
            }
            return std::vector{message};
        }
    });

    // Change Password
    commands.push_back({
        .name = "password",
        .args = {"old", "new"},
        .callback = [self, &guid](const std::vector<std::string> &args) {
            // Arguments
            const std::string &old_password = args[0];
            const std::string &new_password = args[1];

            // Get Username
            std::string username;
            const Player *player = self->getPlayer(guid);
            if (player) {
                username = misc_get_player_username_utf(player);
            }

            // Run
            std::string message = "Invalid Password";
            if (change_password(username, old_password, new_password)) {
                message = "Password Changed";
            }
            return std::vector{message};
        }
    });

    // Reset Player's Password
    commands.push_back({
        .name = "reset",
        .requires_admin = true,
        .args = {username_arg},
        .callback = [](const std::vector<std::string> &args) {
            // Arguments
            const std::string &username = args[0];
            const std::string &password = generate_password();

            // Run
            std::string message1 = "Unable To Reset Password";
            std::string message2;
            if (change_password(username, password)) {
                message1 = "Reset Password: " + username;
                message2 = generated_password + password;
            }

            // Return
            std::vector<std::string> out;
            out.push_back(message1);
            if (!message2.empty()) {
                out.push_back(message2);
            }
            return out;
        }
    });
}