#include "FileNode.hpp"

#include <fstream>
#include <cassert>

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
	fullName(std::move(name + ext)),
	content(std::move(params.content)),
	isDirectory(std::move(params.isDirectory)) {}
FNode::~FNode() noexcept {
	for (auto& child : children) {
		delete child;
	}
}
FNode::FNode(FNode&& other) noexcept :
	owner(std::exchange(other.owner, nullptr)),
	parent(std::exchange(other.parent, nullptr)),
	name(std::move(other.name)),
	ext(std::move(other.ext)),
	fullName(std::move(other.fullName)),
	content(std::move(other.content)),
	isDirectory(std::exchange(other.isDirectory, false)),
	children(std::move(other.children)) {
	FixPointers(*this);
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
	FixPointers(*this);
	return *this;
}

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

bool FNode::ReadContent() const {
	if (!owner) {
		DOA_LOG_ERROR("FNode::ReadContent cannot read contents of a non-owned file node!");
		DOA_LOG_ERROR("\tnode must have a valid owning project!");
		return false;
	}
	if (isDirectory) {
		DOA_LOG_ERROR("FNode::ReadContent read contents of a folder!");
		DOA_LOG_ERROR("\tnode must be a file!");
		return;
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
	assert(file.is_open(), "file should be open by default");
	file << this->content;
	file.close();
}
std::string FNode::DisposeContent() const {
	std::string data = std::move(content); // move
	content = "";
	return data; // move or possibly NRVO
}

bool FNode::IsFile() const { return !IsDirectory(); }
bool FNode::IsDirectory() const { return isDirectory; }

const Project* FNode::OwningProject() const { return owner; }
FNode* FNode::RootNode() const { return owner ? &const_cast<Project*>(owner)->Assets().Root() : nullptr; }
FNode* FNode::ParentNode() const { return parent; }
void FNode::MoveUnder(FNode& directory) {
	if (!directory.IsDirectory()) {
		DOA_LOG_ERROR("FNode::MoveUnder cannot move under something that isn't a directory!");
		DOA_LOG_ERROR("\tnode must be a directory");
		return;
	}
	parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());
	parent = &directory;
	parent->children.push_back(this);

	if (owner) {
		std::filesystem::current_path(owner->Workspace());
		std::filesystem::rename(Path(), directory.Path() / fullName);
	}
}

std::vector<FNode*> FNode::Children() { return children; }
const std::vector<FNode*>& FNode::Children() const { return children; }
auto FNode::begin() { return children.begin(); }
auto FNode::end() { return children.end(); }

FNode FNode::HollowCopy(const FNode& other) {
	return {{ other.OwningProject(), other.ParentNode(), other.Name(), other.Extension(), "", other.IsDirectory() }};
}

FNode* FNode::CreateChildFileFor(FNode& node, FNodeCreationParams&& params) {
	if (!node.owner) {
		DOA_LOG_ERROR("FNode::CreateChildFileFor not supported for non-owned nodes, node must have a valid owning project!");
		return nullptr;
	}
	if (params.name.empty()) {
		DOA_LOG_ERROR("FNode::CreateChildFileFor params.name is empty, files must have names!");
		return nullptr;
	}

	params.owner = node.owner;
	params.parent = &node;
	params.isDirectory = false;

	std::filesystem::current_path(node.owner->Workspace());
	std::string fullName = (params.name + params.ext);
	std::filesystem::path pathToFile(params.parent ? params.parent->Path() / fullName : fullName);

	std::ofstream file(pathToFile, std::ofstream::trunc | std::ofstream::binary);
	assert(file.is_open(), "file should be open by default");
	file << params.content;
	file.flush();
	file.close();

	FNode* ptr = new FNode(std::move(params));
	node.children.push_back(ptr);
	return ptr;
}
FNode* FNode::CreateChildFolderFor(FNode& node, FNodeCreationParams&& params) {
	if (!node.owner) {
		DOA_LOG_ERROR("FNode::CreateChildFolderFor not supported for non-owned nodes, node must have a valid owning project!");
		return nullptr;
	}
	if (params.name.empty()) {
		DOA_LOG_ERROR("FNode::CreateChildFileFor params.name is empty, directories must have names!");
		return nullptr;
	}

	params.owner = node.owner;
	params.parent = &node;
	params.ext.clear();
	params.content.clear();
	params.isDirectory = true;

	std::filesystem::current_path(node.owner->Workspace());
	std::filesystem::create_directory(params.name);

	FNode* ptr = new FNode(std::move(params));
	node.children.push_back(ptr);
	return ptr;
}

void FNode::FixPointers(FNode& node) {
	for (auto& child : node.children) {
		child->parent = &node;
		FixPointers(*child);
	}
}