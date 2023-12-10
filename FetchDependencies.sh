if [ -z "$1" ]; then
    echo "No triplet supplied, defaulting to x64-windows"
	platform="x64-windows"
else
	platform=$1
fi

mkdir vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
chmod +x bootstrap-vcpkg.sh
./bootstrap-vcpkg.sh
./vcpkg integrate install

echo "Installing AngelScript"
./vcpkg install angelscript[addons] --triplet $platform --recurse

echo "Installing Assimp"
./vcpkg install assimp --triplet $platform

echo "Installing ImGui"
./vcpkg install imgui[core,docking-experimental,glfw-binding,opengl3-binding] --triplet $platform --recurse
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

echo "Installing LunaSVG"
./vcpkg install lunasvg --triplet $platform

echo "Installing STB"
./vcpkg install stb --triplet $platform

echo "Installing TinyXML2"
./vcpkg install tinyxml2 --triplet $platform

./vcpkg integrate install
read -rsp $'Press any key to continue...\n' -n1 key