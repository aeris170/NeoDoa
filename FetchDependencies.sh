mkdir vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install angelscript
./vcpkg install assimp
./vcpkg install imgui
./vcpkg install entt
./vcpkg install glew
./vcpkg install glfw3
./vcpkg install glm
./vcpkg install lunasvg
./vcpkg install stb
./vcpkg install tinyxml2
./vcpkg integrate install