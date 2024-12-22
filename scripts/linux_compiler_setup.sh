#!/bin/bash

CYAN="\033[1;36m"
RESET="\033[0m"

sudo apt install cmake gcc-14 libstdc++-14-dev clang -y

echo -e "${CYAN}Select clang from below...${RESET}"
sudo update-alternatives --config cc
sudo update-alternatives --config c++
echo

echo -e "${CYAN}Make sure clang version >= 18.1.3${RESET}"
clang -v