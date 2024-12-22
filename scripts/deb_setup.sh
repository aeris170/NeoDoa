#!/bin/bash

if ! command -v apt 2>&1 >/dev/null
then
    echo "apt could not be found. This script is for Debian and Ubuntu derivates."
    exit 1
fi

CYAN="\033[1;36m"
RESET="\033[0m"

sudo apt update
sudo apt install clang cmake curl gcc-14 libstdc++-14-dev tar unzip zip -y
echo

echo -e "${CYAN}Select clang from below...${RESET}"
sudo update-alternatives --config cc
sudo update-alternatives --config c++
echo

echo -e "${CYAN}Make sure clang version >= 18.1.3${RESET}"
clang -v
