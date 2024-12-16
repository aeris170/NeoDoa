#!/bin/bash

ensure_system_packages () {
echo -e "${WHITE}Processing packages...${RESET}"
echo -e "${WHITE}----------------------${RESET}"

# Find all transitive dependencies of each package and put them into a list,
transitive_dependencies=()
for package in "${package_names[@]}"; do
    result=($(./vcpkg depend-info $package --format=dot | grep -oP '^"([a-z0-9\-]*)";' | sed 's/"//' | sed 's/";//'))
    for i in "${result[@]}"; do
        transitive_dependencies+=("$i")
    done
done
unique_transitive_dependencies_list=($(printf "%s\n" "${transitive_dependencies[@]}" | sort -u))

# Iterate over each package (or dependency) name and extract system dependencies from portfiles
required_system_packages=()
for package in "${unique_transitive_dependencies_list[@]}"; do
    echo -e "Processing portfile for: ${CYAN}$package${RESET}"

    # Define the portfile path
    file="./ports/$package/portfile.cmake"

    # Check if file exists
    if [ ! -f "$file" ]; then
        echo -e "\e[1A\e[K${RED}File not found for package ${CYAN}$package${RESET}!"
        echo -e "---------------------------------------------------"
        continue
    fi

    # Process the file:
    # - Remove the literal '\n' strings by replacing '\n' with an empty string.
    # - Remove multiple spaces by collapsing them into one.
    processed_content=$(cat "$file" | sed 's/\\n//g' | tr -s ' ')

    # Extract packages after "apt install"
    apt_packages=($(echo "$processed_content" | grep -oP 'apt install \K([^.\\"]+)' | sed 's/"$//'))

    # Extract packages after "apt-get install"
    apt_get_packages=($(echo "$processed_content" | grep -oP 'apt-get install \K([^.\\"]+)' | sed 's/"$//'))

    # Output the extracted packages
    if [ -z "$apt_packages" ] && [ -z "$apt_get_packages" ]; then
        echo -e "\e[1A\e[K${CYAN}$package${RESET} has no system package dependencies."
    else
        echo -e "\e[1A\e[K${CYAN}$package${RESET} requires system packages to be available: "

        for apt_package in "${apt_packages[@]}"; do
            echo -e "${YELLOW}$apt_package${RESET}"
            required_system_packages+=("$apt_package")
        done
        for apt_get_package in "${apt_get_packages[@]}"; do
            echo -e "${YELLOW}$apt_get_package${RESET}"
            required_system_packages+=("$apt_get_package")
        done

        # Install packages
        #if [ -n "$apt_get_packages" ]; then
        #    if ! sudo apt-get -qq install -y "$apt_get_packages"; then
        #        echo "Failed to install $apt_get_packages"
        #    fi
        #fi
        #if [ -n "$apt_install_packages" ]; then
        #    if ! sudo apt-get -qq install -y "$apt_install_packages"; then
        #        echo "Failed to install $apt_install_packages"
        #    fi
        #fi
    fi
    echo "---------------------------------------------------"
done
echo

echo -e "${WHITE}Overall required system packages — THEY ARE MANDATORY:${RESET}"
unique_required_system_packages=($(printf "%s\n" "${required_system_packages[@]}" | sort -u))
for package in "${unique_required_system_packages[@]}"; do
    echo -e "${YELLOW}$package${RESET}"
done
echo

if [ -z ${system_packages_user_choice} ]; then
    read -p "Would you like to install them using apt? [Y/n]: " system_packages_user_choice
fi
echo

if [[ $system_packages_user_choice == "Y" || $system_packages_user_choice == "y" ]]; then
    echo -e "Installing..."
    for package in "${unique_required_system_packages[@]}"; do
        _=$(sudo apt-get install ${package} -y -qq 2>&1 | tee aptOutput.log)
        if [ ${PIPESTATUS[0]} -ne 0 ]; then
            echo -e "${RED}Error during system package installation with apt.${RESET}"
            cat aptOutput.log
            rm aptOutput.log
            cd ..
        fi
        rm aptOutput.log
        echo "Done. Check for errors."
    done
else
    echo -e "${RED}YOU WILL HAVE PROBLEMS IF REQUIRED SYSTEM PACKAGES ARE MISSING!${RESET}"
    read -p "Would you still not like to install them? [Y/n]: " system_packages_user_choice
    if [[ $system_packages_user_choice == "Y" || $system_packages_user_choice == "y" ]]; then
        echo -e "Installing..."
        for package in "${unique_required_system_packages[@]}"; do
            _=$(sudo apt-get install ${package} -y -qq 2>&1 | tee aptOutput.log)
            if [ ${PIPESTATUS[0]} -ne 0 ]; then
                echo -e "${RED}Error during system package installation with apt.${RESET}"
                cat aptOutput.log
                rm aptOutput.log
                cd ..
            fi
            rm aptOutput.log
            echo "Done. Check for errors."
        done
    fi
fi
}

WHITE="\033[1;37m"
CYAN="\033[1;36m"
YELLOW="\033[1;33m"
GREEN="\033[1;32m"
RED="\033[1;31m"
RESET="\033[0m"

# List of package names (real packages from vcpkg.sh)
package_names=(
    "angelscript[addons]" 
    "argparse" 
    "assimp" 
    "cppzmq" 
    "entt" 
    "eventpp" 
    "glew" 
    "glfw3" 
    "glm" 
    "icu" 
    "imgui[core,docking-experimental,glfw-binding,sdl2-binding,opengl3-binding,vulkan-binding]" 
    "imguizmo" 
    "lunasvg" 
    "stb" 
    "tinyxml2"
)

# Check if clonemode argument is provided
if [ -z "$1" ]; then
    echo -e "${YELLOW}No clonemode supplied, defaulting to https${RESET}"
    clonemode="https"
else
    clonemode=$1
fi

# Check if vcpkg path argument is provided
if [ -z "$2" ]; then
    echo -e "${YELLOW}No vcpkg path supplied, defaulting to ./vcpkg${RESET}"
    path="./vcpkg"
else
    path=$2
fi

echo -e "${WHITE}Clonemode: $clonemode${RESET}"
echo -e "${WHITE}vcpkg path: $path${RESET}"
echo

# Check if the directory exists
if [ -d "$path" ]; then
    # If the directory exists, go inside and pull the latest changes
    echo -e "${WHITE}Directory already exists: $path${RESET}"
    echo -e "${WHITE}Changing to directory: $path${RESET}"
    echo

    cd "$path"

    echo -e "${WHITE}Pulling latest vcpkg changes...${RESET}"
    git pull 2>&1 | tee pullOutput.log
    if [ ${PIPESTATUS[0]} -ne 0 ]; then
        echo -e "${RED}Error during git pull.${RESET}"
        cat pullOutput.log
        rm pullOutput.log
        cd ..
        exit 1
    fi
    rm pullOutput.log
    echo

    # Run the bootstrap script
    if [ -f "./bootstrap-vcpkg.sh" ]; then
        echo -e "${WHITE}Running bootstrap-vcpkg.sh...${RESET}"
        ./bootstrap-vcpkg.sh -disableMetrics 2>&1 | tee bootstrapOutput.log
        if [ ${PIPESTATUS[0]} -ne 0 ]; then
            echo -e "${RED}Error during bootstrap.${RESET}"
            cat bootstrapOutput.log
            rm bootstrapOutput.log
            cd ..
            exit 1
        fi
        rm bootstrapOutput.log
    else
        echo -e "${RED}Bootstrap script not found!${RESET}"
        cd ..
        exit 1
    fi
    echo

    ensure_system_packages
    echo

    # Update required packages
    echo -e "${WHITE}Updating required vcpkg packages...${RESET}"
    ./vcpkg upgrade --no-dry-run 2>&1 | tee updateOutput.log
    if [ ${PIPESTATUS[0]} -ne 0 ]; then
        echo -e "${RED}Error during vcpkg update.${RESET}"
        cat updateOutput.log
        rm updateOutput.log
        cd ..
        exit 1
    fi
    rm updateOutput.log
    echo
else
    # If the directory doesn't exist, go to the parent directory and clone the repo
    echo -e "${WHITE}Directory does not exist: $path${RESET}"
    echo -e "${WHITE}Changing to parent directory and cloning vcpkg...${RESET}"
    cd "$(dirname "$path")"
    if [ "$clonemode" = "ssh" ]; then
        git clone git@github.com:microsoft/vcpkg.git "$(basename "$path")" 2>&1 | tee cloneOutput.log
    elif [ "$clonemode" = "https" ]; then
        git clone https://github.com/Microsoft/vcpkg.git "$(basename "$path")" 2>&1 | tee cloneOutput.log
    else
        echo -e "${RED}Incorrect clonemode! Expected https or ssh, got something else${RESET}"
        cd ..
        exit 1
    fi

    # Check if clone was successful
    if [ ${PIPESTATUS[0]} -ne 0 ]; then
        echo -e "${RED}Error during git clone.${RESET}"
        cat cloneOutput.log
        rm cloneOutput.log
        cd ..
        exit 1
    fi
    rm cloneOutput.log
    echo

    # Change to the newly cloned vcpkg directory
    cd "$path"

    # Run the bootstrap script
    if [ -f "./bootstrap-vcpkg.sh" ]; then
        echo -e "${WHITE}Running bootstrap-vcpkg.sh...${RESET}"
        ./bootstrap-vcpkg.sh -disableMetrics 2>&1 | tee bootstrapOutput.log
        if [ ${PIPESTATUS[0]} -ne 0 ]; then
            echo -e "${RED}Error during bootstrap.${RESET}"
            cat bootstrapOutput.log
            rm bootstrapOutput.log
            cd ..
            exit 1
        fi
        rm bootstrapOutput.log
    else
        echo -e "${RED}Bootstrap script not found!${RESET}"
        cd ..
        exit 1
    fi
    echo

    ensure_system_packages
    echo

    # Install required packages
    echo -e "${WHITE}Installing required vcpkg packages...${RESET}"
    ./vcpkg install "${package_names[@]}" --recurse 2>&1 | tee installOutput.log
    if [ ${PIPESTATUS[0]} -ne 0 ]; then
        echo -e "${RED}Error during vcpkg install.${RESET}"
        cat installOutput.log
        rm installOutput.log
        cd ..
        exit 1
    fi
    rm installOutput.log
    echo
fi

# Integrate vcpkg
echo -e "${WHITE}Integrating vcpkg...${RESET}"
./vcpkg integrate install 2>&1 | tee integrateOutput.log
if [ ${PIPESTATUS[0]} -ne 0 ]; then
    echo -e "${RED}Error during vcpkg integrate.${RESET}"
    cat integrateOutput.log
    rm integrateOutput.log
    cd ..
    exit 1
fi
rm integrateOutput.log
echo

# Notify completion
echo -e "${GREEN}vcpkg setup complete!${RESET}"
