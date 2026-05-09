# Cerberus
This is the utility and moderation mod for the official server.

It supports:
* Forwarding Messages To A Discord Webhook
* Basic Commands
* Custom Welcome Messages

## Configuration
All configuration files should be placed in the cerberus directory.

### welcome.txt
The contents of this file will be displayed to every player when joining.

### webhook.txt
This contains the Discord webhook.

It consists of two lines:
1. The Webhook URL
2. Who Should Be Pinged
   * Format: <user|role>:<id>
