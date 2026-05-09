#include "commands.h"
#include "../config/config.h"

#include <mods/misc/misc.h>

#include <symbols/ServerSideNetworkHandler.h>
#include <symbols/Level.h>

// Bullet Point
static constexpr const char *bullet = "• ";

// Load Commands
void add_common_commands(std::vector<Command> &commands, ServerSideNetworkHandler *self)
{
    // Generate Help
    commands.push_back({.name = "help",
                        .callback = [&commands](MCPI_UNUSED const std::vector<std::string> &args)
                        {
                            std::vector<std::string> ret = {"All Commands:"};
                            for (const Command &command : commands)
                            {
                                std::string line = bullet + command.name;
                                for (const std::string &arg : command.args)
                                {
                                    line += " <" + arg + ">";
                                }
                                ret.push_back(line);
                            }
                            return ret;
                        }});

    // List Players
    // https://github.com/MCPI-Revival/minecraft-pi-reborn/blob/master/symbols/src/level/Level.def
    // https://github.com/MCPI-Revival/minecraft-pi-reborn/blob/master/symbols/src/entity/player/Player.def
    commands.push_back({.name = "list",
                        .callback = [self](MCPI_UNUSED const std::vector<std::string> &args)
                        {
                            std::vector<std::string> ret = {"All Players:"};
                            const Level *level = self->level;
                            if (level)
                            {
                                for (const Player *other : level->players)
                                {
                                    std::string line = bullet + misc_get_player_username_utf(other);
                                    ret.push_back(line);
                                }
                            }
                            return ret;
                        }});

    // Get Seed
    // https://github.com/MCPI-Revival/minecraft-pi-reborn/blob/master/symbols/src/level/Level.def
    commands.push_back({.name = "seed",
                        .callback = [self](MCPI_UNUSED const std::vector<std::string> &args)
                        {
                            std::vector<std::string> ret = {"Seed:"};
                            const Level *level = self->level;
                            if (level)
                            {
                                int seed = level->data.seed;
                                ret.push_back(std::to_string(seed));
                            }
                            return ret;
                        }});

    // Get Time
    // https://github.com/MCPI-Revival/minecraft-pi-reborn/blob/master/symbols/src/level/Level.def
    // https://github.com/MCPI-Revival/minecraft-pi-reborn/blob/master/symbols/src/level/LevelData.def
    commands.push_back({.name = "time",
                        .callback = [self](MCPI_UNUSED const std::vector<std::string> &args)
                        {
                            std::vector<std::string> ret = {"Time:"};
                            const Level *level = self->level;
                            if (level)
                            {
                                int time = level->data.time;
                                ret.push_back(std::to_string(time));
                            }
                            return ret;
                        }});
}
