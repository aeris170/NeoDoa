#include "FileNode.hpp"

#include <fstream>
#include <cassert>

#include "Log.hpp"
#include "Core.hpp"
#include "Assets.hpp"
#include "Project.hpp"

FNode::FNode(const FNodeCreationParams& params) noexcept :
    owner(params.owner),
    parent(params.parent),
    name(params.name),
    ext(params.ext),
    fullName(name + ext),
    content(params.content),
    isDirectory(params.isDirectory) {}
FNode::FNode(FNodeCreationParams&& params) noexcept :
    owner(std::exchange(params.owner, nullptr)),
    parent(std::exchange(params.parent, nullptr)),
    name(std::move(params.name)),
    ext(std::move(params.ext)),
    fullName(name + ext),
    content(std::move(params.content)),
    isDirectory(params.isDirectory) {}
FNode::FNode(FNode&& other) noexcept :
    owner(std::exchange(other.owner, nullptr)),
    parent(std::exchange(other.parent, nullptr)),
    name(std::move(other.name)),
    ext(std::move(other.ext)),
    fullName(std::move(other.fullName)),
    content(std::move(other.content)),
    isDirectory(std::exchange(other.isDirectory, false)),
    children(std::move(other.children)) {
    __onMove(this);
}
FNode& FNode::operator=(FNode&& other) noexcept {
    owner = std::exchange(other.owner, nullptr);
    parent = std::exchange(other.parent, nullptr);
    name = std::move(other.name);
    ext = std::move(other.ext);
    fullName = std::move(other.fullName);
    content = std::move(other.content);
    isDirectory = std::exchange(other.isDirectory, false);
    children = std::move(other.children);
    __onMove(this);
    return *this;
}
bool FNode::operator==(const FNode& other) const noexcept { return this->Path() == other.Path(); }

std::filesystem::path FNode::Path() const {
    if (parent) {
        return parent->Path() / fullName;
    } else {
        return fullName;
    }
}
std::filesystem::path FNode::AbsolutePath() const {
    if (!owner) {
        DOA_LOG_ERROR("FNode::AbsolutePath cannot calculate absolute path of non-owned file node!");
        DOA_LOG_ERROR("\tnode must have a valid owning project!");
        return std::filesystem::path();
    }

    if (parent) {
        return owner->Workspace() / parent->Path() / fullName;
    } else {
        return owner->Workspace() / fullName;
    }
}
std::filesystem::path FNode::FolderPath() const {
    if (parent) {
        return parent->Path();
    } else {
        auto pos = fullName.find_last_of(std::filesystem::path::preferred_separator);
        if (pos != -1) {
            return fullName.substr(0, pos);
        } else {
            DOA_LOG_ERROR("FNode::FolderPath cannot calculate folder path!");
            DOA_LOG_ERROR("\tnode must have:");
            DOA_LOG_ERROR("\t\teither a valid parent");
            DOA_LOG_ERROR("\t\tor an actual path in it's name");
            return std::filesystem::path();
        }
    }
}

std::string FNode::Name() { return name; }
const std::string& FNode::Name() const { return name; }
void FNode::ChangeName(std::string_view name) { ChangeName(std::string(name)); }
void FNode::ChangeName(const std::string& name) { ChangeName(std::string(name)); }
void FNode::ChangeName(std::string&& name) {
    std::string oldFullName = fullName;
    this->name = std::move(name);
    fullName = this->name + ext;

    if (owner) {
        std::filesystem::current_path(owner->Workspace());
        auto parentPath = Path().parent_path();
        std::filesystem::rename(parentPath / oldFullName, parentPath / fullName);
    }
}

std::string FNode::Extension() { return ext; }
const std::string& FNode::Extension() const { return ext; }
void FNode::ChangeExtension(std::string_view extension) { ChangeExtension(std::string(extension)); }
void FNode::ChangeExtension(const std::string& extension) { ChangeExtension(std::string(extension)); }
void FNode::ChangeExtension(std::string&& extension) {
    if (isDirectory) {
        DOA_LOG_ERROR("FNode::ChangeExtension cannot change extension of a folder!");
        DOA_LOG_ERROR("\tnode must be a file!");
        return;
    }

    std::string oldFullName = fullName;
    ext = std::move(extension);
    fullName = name + ext;

    if (owner) {
        std::filesystem::current_path(owner->Workspace());
        auto parentPath = Path().parent_path();
        std::filesystem::rename(parentPath / oldFullName, parentPath / fullName);
    }
}

std::string FNode::FullName() { return fullName; }
const std::string& FNode::FullName() const { return fullName; }

std::string_view FNode::Content() const { return content; }
bool FNode::HasContent() const { return !content.empty(); }
bool FNode::ReadContent() const {
    if (!owner) {
        DOA_LOG_ERROR("FNode::ReadContent cannot read contents of a non-owned file node!");
        DOA_LOG_ERROR("\tnode must have a valid owning project!");
        return false;
    }
    if (isDirectory) {
        DOA_LOG_ERROR("FNode::ReadContent read contents of a folder!");
        DOA_LOG_ERROR("\tnode must be a file!");
        return false;
    }

    std::filesystem::current_path(owner->Workspace());
    std::ifstream file(Path(), std::ifstream::in | std::ifstream::binary);

    if (file.is_open()) {
        std::getline(file, content, '\0');
        file.close();
        return true;
    }
    return false;
}
void FNode::ModifyContent(std::string_view content) { ModifyContent(std::string(content)); }
void FNode::ModifyContent(const std::string& content) { ModifyContent(std::string(content)); }
void FNode::ModifyContent(std::string&& content) {
    if (!owner) {
        DOA_LOG_ERROR("FNode::ModifyContent cannot modify contents of a non-owned file node!");
        DOA_LOG_ERROR("\tnode must have a valid owning project!");
        return;
    }
    if (isDirectory) {
        DOA_LOG_ERROR("FNode::ModifyContent cannot modify contents of a folder!");
        DOA_LOG_ERROR("\tnode must be a file!");
        return;
    }

    std::filesystem::current_path(owner->Workspace());
    this->content = std::move(content);
    std::ofstream file(Path(), std::ofstream::trunc | std::ofstream::binary);
    assert(file.is_open()); /* file should be open by default */
    file << this->content;
    file.close();
}
std::string FNode::DisposeContent() const {
    std::string data = std::move(content); // move
    content = "";
    return data; // move or possibly NRVO
}

void FNode::MoveUnder(FNode& directory) {
    if (!directory.IsDirectory()) {
        DOA_LOG_ERROR("FNode::MoveUnder cannot move under something that isn't a directory!");
        DOA_LOG_ERROR("\tnode must be a directory");
        return;
    }
    auto found = std::ranges::find(parent->children, *this);
    if (found != parent->children.end()) {
        found = parent->children.erase(found);
    }
    parent = &directory;
    parent->children.emplace_back(std::move(*this));

    if (owner) {
        std::filesystem::current_path(owner->Workspace());
        std::filesystem::rename(Path(), directory.Path() / fullName);
    }
}
void FNode::Delete() {
    if (parent == nullptr) { return; }

    parent->DeleteChildNode(this);
}

uintmax_t FNode::Size() const { return std::filesystem::file_size(AbsolutePath()); }

bool FNode::IsFile() const { return !IsDirectory(); }
bool FNode::IsDirectory() const { return isDirectory; }

bool FNode::HasOwningProject() const { return OwningProject() != nullptr; }
const Project* FNode::OwningProject() const { return owner; }

bool FNode::HasRootNode() const { return RootNode() != nullptr; }
FNode* FNode::RootNode() const { return owner ? &Core::GetCore()->Assets()->Root() : nullptr; }

bool FNode::HasParentNode() const { return ParentNode() != nullptr; }
FNode* FNode::ParentNode() const { return parent; }

std::vector<FNode>& FNode::Children() { return children; }
const std::vector<FNode>& FNode::Children() const { return children; }
auto FNode::begin() { return children.begin(); }
auto FNode::end() { return children.end(); }

FNode FNode::HollowCopy(const FNode& other) {
    return { { other.OwningProject(), other.ParentNode(), other.Name(), other.Extension(), "", other.IsDirectory() } };
}

FNode* FNode::CreateChildFile(FNodeCreationParams&& params) {
    if (!owner) {
        DOA_LOG_ERROR("FNode::CreateChildFileFor not supported for non-owned nodes, node must have a valid owning project!");
        return nullptr;
    }
    if (params.name.empty()) {
        DOA_LOG_ERROR("FNode::CreateChildFileFor params.name is empty, files must have names!");
        return nullptr;
    }

    params.owner = owner;
    params.parent = this;
    params.isDirectory = false;

    std::filesystem::current_path(owner->Workspace());
    { /* check for duplicate file names and act accordingly */
        bool found = false;
        int appendCount = 0;
        do {
            found = false;
            for (const auto& child : children) {
                if (child.IsFile() && child.name == params.name) {
                    found = true;
                    break;
                }
            }
            if (found) {
                appendCount++;
                if (appendCount == 1) {
                    params.name.append(".1");
                } else {
                    auto pos = params.name.find_last_of(".");
                    params.name = params.name.substr(0, pos + 1).append(std::to_string(appendCount));
                }
            }
        } while (found);
    }
    std::string fullName = params.name + params.ext;
    std::ofstream file(params.parent->Path() / fullName, std::ofstream::trunc | std::ofstream::binary);
    assert(file.is_open()); /* file should be open by default */
    file << params.content;
    file.flush();
    file.close();

    return &children.emplace_back(std::move(params));
}
FNode* FNode::CreateChildFolder(FNodeCreationParams&& params) {
    if (!owner) {
        DOA_LOG_ERROR("FNode::CreateChildFolderFor not supported for non-owned nodes, node must have a valid owning project!");
        return nullptr;
    }
    if (params.name.empty()) {
        DOA_LOG_ERROR("FNode::CreateChildFileFor params.name is empty, directories must have names!");
        return nullptr;
    }

    params.owner = owner;
    params.parent = this;
    params.ext.clear();
    params.content.clear();
    params.isDirectory = true;

    { /* check for duplicate file names and act accordingly */
        bool found = false;
        int appendCount = 0;
        do {
            found = false;
            for (const auto& child : children) {
                if (child.IsDirectory() && child.name == params.name) {
                    found = true;
                    break;
                }
            }
            if (found) {
                appendCount++;
                if (appendCount == 1) {
                    params.name.append(".1");
                } else {
                    auto pos = params.name.find_last_of(".");
                    params.name = params.name.substr(0, pos + 1).append(std::to_string(appendCount));
                }
            }
        } while (found);
    }

    std::filesystem::current_path(owner->Workspace());
    bool success = std::filesystem::create_directory(params.parent->Path() / params.name);
    if (success) {
        return &children.emplace_back(std::move(params));
    } else {
        return nullptr;
    }

}
bool FNode::DeleteChildNode(FNode* child) {
    if (!owner) {
        DOA_LOG_ERROR("FNode::DeleteChildNode not supported for non-owned nodes, node must have a valid owning project!");
        return false;
    }
    if (child == nullptr) {
        DOA_LOG_ERROR("FNode::DeleteChildNode child must not be nullptr!");
        return false;
    }

    if (this != child->parent) {
        return false;
    }
    auto pos = std::ranges::find(children, *child);
    assert(pos != children.end()); /* Something is wrong, child has this as parent but this does not have child as a child! */

    std::filesystem::current_path(owner->Workspace());
    std::filesystem::remove_all(child->Path());
    child->ext += ".id";
    child->fullName += ".id";
    std::filesystem::remove_all(child->Path());

    children.erase(pos);

    return true;
}
FNode& FNode::FindChild(const std::filesystem::path& path) {
    auto itr = path.begin();
    FNode* search{ this };
    do {
        auto result = std::ranges::find_if(search->children, [&itr](const FNode& element) { return element.FullName() == itr->string(); });
        if (result != search->children.end()) {
            /* ps: explicit call to operator*() is redundant but &*result */
            /* looked weird as hell so I decided to write it this way */
            search = &(result.operator*());
            ++itr;
        } else {
            break;
        }
    } while (itr != path.end());
    return *search;
}