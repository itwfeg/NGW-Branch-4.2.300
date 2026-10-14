#!/bin/sh -f
# rem command to remove requirement for sudo to have a password
sudo echo "$USER ALL=(ALL) NOPASSWD:ALL" | sudo tee -a /etc/sudoers
sudo cat /etc/sudoers | grep "$USER"
