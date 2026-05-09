#pragma once

#include <string>
#include <vector>

// Configuration File
struct ConfigFile
{
    virtual ~ConfigFile() = default;

    // Load/Save
    void save() const;
    virtual void load();

    // Name
    [[nodiscard]] virtual const char *get_name() const = 0;
    [[nodiscard]] virtual const char *get_file() const = 0;
    [[nodiscard]] std::string get_path() const;

    // Data
    virtual void clear() = 0;

    // Implementation
    [[nodiscard]] virtual bool can_save() const = 0;
    virtual void do_save(std::ofstream &) const;
    virtual void do_load(std::ifstream &) = 0;
    [[nodiscard]] virtual bool check_load() const;
};

// Init
void init_webhook();

// Webhook
typedef unsigned long long snowflake;
void send_to_discord(const std::string &message, bool can_ping);

// Admin
struct Player;

// Welcome Message
const std::vector<std::string> &get_welcome_messages();
