#include "config.h"

#include <fstream>
#include <sstream>
#include <unistd.h>
#include <csignal>

#include <libreborn/util/exec.h>
#include <libreborn/util/string.h>
#include <libreborn/patch.h>

#include <symbols/Gui.h>

// Configuration
struct Webhook final : ConfigFile
{
    std::string url;
    // Load/Save
    void clear() override
    {
        url = "";
    }
    void do_load(std::ifstream &) override;
    [[nodiscard]] bool check_load() const override;
    [[nodiscard]] bool can_save() const override
    {
        return false;
    }
    // Name
    [[nodiscard]] const char *get_name() const override
    {
        return "Discord Webhook";
    }
    [[nodiscard]] const char *get_file() const override
    {
        return "webhook.txt";
    }
};

// Get URL
void Webhook::do_load(std::ifstream &file)
{
    // Load URL
    std::getline(file, url);
}
bool Webhook::check_load() const
{
    return !url.empty();
}
static const Webhook &get_config()
{
    static Webhook config;
    static bool loaded = false;
    if (!loaded)
    {
        config.load();
    }
    return config;
}

// Make Webhook
static std::string escape(const std::string &input)
{
    std::ostringstream escaped;
    for (const char c : input)
    {
        switch (c)
        {
        case '\"':
            escaped << "\\\"";
            break;
        case '\\':
            escaped << "\\\\";
            break;
        case '\b':
            escaped << "\\b";
            break;
        case '\f':
            escaped << "\\f";
            break;
        case '\n':
            escaped << "\\n";
            break;
        case '\r':
            escaped << "\\r";
            break;
        case '\t':
            escaped << "\\t";
            break;
        default:
            escaped << c;
        }
    }
    return escaped.str();
}
static std::string make_json(const std::string &message, const bool can_ping)
{
    const Webhook &config = get_config();
    // Suppress Embeds
    out += "\"flags\": 4, ";
    // Content
    out += "\"content\": \"";
    out += escape(message);
    out += '\"';
    // Return
    out += '}';
    return out;
}
// Send Message
void send_to_discord(const std::string &message, const bool can_ping)
{
    const Webhook &config = get_config();
    // Get JSON
    const std::string json = make_json(message, can_ping);
    const std::string &url = config.url;
    // Send
    if (fork() == 0)
    {
        const char *const argv[] = {
            "curl",
            "--silent",
            "--show-error",
            "--fail",
            "--output", "/dev/null",
            "--request", "POST",
            "--header", "Content-Type: application/json",
            "--data", json.c_str(),
            url.c_str(),
            nullptr};
        safe_exec(argv);
    }
}

// Logging
static void Gui_addMessage_injection(Gui_addMessage_t original, Gui *gui, const std::string &text)
{
    static bool recursing = false;
    if (!recursing)
    {
        recursing = true;
        const std::string safe_message = from_cp437(text);
        send_to_discord(safe_message, false);
        original(gui, text);
        recursing = false;
    }
    else
    {
        original(gui, text);
    }
}

// Init
void init_webhook()
{
    signal(SIGCHLD, SIG_IGN);
    get_config();
    send_to_discord("**Server Started!**", false);
    // Logging
    overwrite_calls(Gui_addMessage, Gui_addMessage_injection);
}
