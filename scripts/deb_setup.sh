#!/bin/bash

if ! command -v apt 2>&1 >/dev/null; then
    echo "apt could not be found. This script is for Debian and Ubuntu derivates."
    exit 1
fi

CYAN="\033[1;36m"
RESET="\033[0m"

echo -e "Performing package repository update..."
sudo apt-get -qq update
echo -e "Done."
echo

echo -e "Installing essentials..."
sudo apt-get -qq install -y build-essential clang cmake curl gcc-14 libstdc++-14-dev tar unzip zip
sudo apt-get -qq install -y libltdl-dev # Undocumented requirement of libxcrypt, Ubuntu CI runner fails without this.
sudo update-alternatives --set cc $(update-alternatives --list cc | grep clang)
sudo update-alternatives --set c++ $(update-alternatives --list c++ | grep clang++)
echo -e "Done."

echo -e "${CYAN}Make sure clang version >= 18.1.3${RESET}"
c++ -v 2>&1 | grep --color=always -P "version\s\K.*?\s"
