#pragma once

#include <string>
#include <vector>
#include <any>

struct ScriptComponentData {

    std::string name;
    std::vector<std::any> data;
};