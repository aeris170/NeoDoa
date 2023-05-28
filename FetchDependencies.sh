mkdir vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install

echo "Installing AngelScript"
./vcpkg install angelscript[addons] --triplet x64-windows --recurse

echo "Installing Assimp"
./vcpkg install assimp --triplet x64-windows

echo "Installing ImGui"
./vcpkg install imgui[core,docking-experimental,glfw-binding,opengl3-binding] --triplet x64-windows --recurse
./vcpkg install imguizmo --triplet x64-windows

echo "Installing EnTT"
./vcpkg install entt --triplet x64-windows

echo "Installing GLEW"
./vcpkg install glew --triplet x64-windows

echo "Installing GLFW"
./vcpkg install glfw3 --triplet x64-windows

echo "Installing GLM"
./vcpkg install glm --triplet x64-windows

echo "Installing LunaSVG"
./vcpkg install lunasvg --triplet x64-windows

echo "Installing STB"
./vcpkg install stb --triplet x64-windows

echo "Installing TinyXML2"
./vcpkg install tinyxml2 --triplet x64-windows

./vcpkg integrate install
read -rsp $'Press any key to continue...\n' -n1 key