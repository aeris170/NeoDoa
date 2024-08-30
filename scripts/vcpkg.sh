if [ -z "$1" ]; then
    echo "No triplet supplied, defaulting to x64-windows"
	platform="x64-windows"
else
	platform=$1
fi

if [ -z "$2" ]; then
    echo "No clonemode supplied, defaulting to https"
	clonemode="https"
else
	clonemode=$2
fi

if [ -z "$3" ]; then
    echo "No vcpkg path supplied, defaulting to ./vcpkg"
	path="./vcpkg"
else
	path=$3
fi

mkdir -p $path
cd $path
if [ "$clonemode" = "https" ]; then
    git clone https://github.com/Microsoft/vcpkg.git .
elif [ "$clonemode" = "ssh" ]; then
    git clone git@github.com:microsoft/vcpkg.git .
else
	echo '\033[0;31mIncorrect clonemode! Expected https or ssh got something else!'
	exit
fi

git pull
chmod +x bootstrap-vcpkg.sh
./bootstrap-vcpkg.sh
./vcpkg integrate install

echo "Installing AngelScript"
./vcpkg install angelscript[addons] --triplet $platform --recurse

echo "Installing argparse"
./vcpkg install argparse --triplet $platform

echo "Installing Assimp"
./vcpkg install assimp --triplet $platform

echo "Installing ImGui"
# Base ImGui package list
imgui_packages="core,docking-experimental,glfw-binding,sdl2-binding,opengl3-binding,vulkan-binding"

# Add DirectX bindings if the triplet is x64-windows
if [ "$platform" = "x64-windows" ]; then
    imgui_packages="$imgui_packages,dx11-binding,dx12-binding"
fi

# Install the ImGui packages
./vcpkg install imgui[$imgui_packages] --triplet $platform --recurse
./vcpkg install imguizmo --triplet $platform

echo "Installing EnTT"
./vcpkg install entt --triplet $platform

echo "Istalling Eventpp"
./vcpkg install eventpp --triplet $platform

echo "Installing GLEW"
./vcpkg install glew --triplet $platform

echo "Installing GLFW"
./vcpkg install glfw3 --triplet $platform

echo "Installing GLM"
./vcpkg install glm --triplet $platform

echo "Installing ICU"
./vcpkg install icu --triplet $platform

echo "Installing LunaSVG"
./vcpkg install lunasvg --triplet $platform

echo "Installing STB"
./vcpkg install stb --triplet $platform

echo "Installing TinyXML2"
./vcpkg install tinyxml2 --triplet $platform

echo "Installing ZeroMQ (cppzmp)"
./vcpkg install cppzmq --triplet $platform

./vcpkg upgrade

./vcpkg integrate install