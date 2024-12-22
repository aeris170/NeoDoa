#!/bin/bash

if ! command -v apt 2>&1 >/dev/null; then
    echo "apt could not be found. This script is for Debian and Ubuntu derivates."
    exit 1
fi

CYAN="\033[1;36m"
RESET="\033[0m"

sudo apt -qq update
sudo apt -qq install -y build-essential clang cmake curl gcc-14 libstdc++-14-dev tar unzip zip
sudo update-alternatives --set cc $(update-alternatives --list cc | grep clang)
sudo update-alternatives --set c++ $(update-alternatives --list c++ | grep clang++)
echo

echo -e "${CYAN}Make sure clang version >= 18.1.3${RESET}"
clang++ -v 2>&1 | grep --color=always -P "version\s\K.*?\s"
