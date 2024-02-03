#pragma once

#include <string>
#include <vector>

struct ProjectData {
    bool IsValid{ false };
    bool IsFavourite{ false };
    std::string Name{};
    std::string AbsolutePath{};
    std::string LastOpened{};
};

using ProjectDataCollection = std::vector<ProjectData>;

struct ProjectDataCollectionSerializationResult {
    bool erred{ false };
    ProjectDataCollection pdc{};
};

bool CheckProjectValidity(const ProjectData& data) noexcept;
std::string SerializeProjectDataCollection(const ProjectDataCollection& pdc) noexcept;
ProjectDataCollectionSerializationResult DeserializeProjectDataCollection(std::string_view data) noexcept;