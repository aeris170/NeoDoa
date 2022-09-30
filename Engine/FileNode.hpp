#pragma once

#include <string>
#include <vector>
#include <filesystem>

struct Project;
struct FNode;

struct FNodeCreationParams {
	const Project* owner{ nullptr }; /* optional, if the file in question is owned by a project, set it*/
	FNode* parent{ nullptr }; /* flexible, nullptr denotes root node, non-nullptr denotes non-root nodes*/
	std::string name{}; /* mandatory, name of file/folder */
	std::string ext{}; /* optional, only applicable to files */
	std::string content{}; /* optional, only applicable to files */
	bool isDirectory{ false };
};

struct FNode {

	FNode(const FNodeCreationParams& params) noexcept;
	FNode(FNodeCreationParams&& params) noexcept;

	~FNode() noexcept;
	FNode(const FNode& other) noexcept = delete;
	FNode(FNode&& other) noexcept;
	FNode& operator=(const FNode& other) = delete;
	FNode& operator=(FNode&& other) noexcept;

	std::filesystem::path Path() const;
	std::filesystem::path AbsolutePath() const;
	std::filesystem::path FolderPath() const;

	std::string Name();
	const std::string& Name() const;
	void ChangeName(std::string_view name);
	void ChangeName(const std::string& name);
	void ChangeName(std::string&& name);

	std::string Extension();
	const std::string& Extension() const;
	void ChangeExtension(std::string_view extension);
	void ChangeExtension(const std::string& extension);
	void ChangeExtension(std::string&& extension);

	bool ReadContent() const;
	void ModifyContent(std::string_view content);
	void ModifyContent(const std::string& content);
	void ModifyContent(std::string&& content);
	std::string DisposeContent() const;

	bool IsFile() const;
	bool IsDirectory() const;

	const Project* OwningProject() const;
	FNode* RootNode() const;
	FNode* ParentNode() const;
	void MoveUnder(FNode& directory);
	void Delete();

	std::vector<FNode*> Children();
	const std::vector<FNode*>& Children() const;
	auto begin();
	auto end();

	static FNode HollowCopy(const FNode& other);

	FNode* CreateChildFile(FNodeCreationParams&& params);
	FNode* CreateChildFolder(FNodeCreationParams&& params);
	bool DeleteChildNode(FNode* child);

private:
	const Project* owner{ nullptr };
	FNode* parent{ nullptr };

	std::string name{};
	std::string ext{};
	std::string fullName{}; /* defined as name + ext (for example "hello" + ".txt") */
	mutable std::string content{}; /* files contents, can be disposed, can be stored... */

	bool isDirectory{ false };

	std::vector<FNode*> children{};

	static void FixPointers(FNode& node);

	friend struct Asset;
	friend struct Assets;

	inline void __pointersInvalidated(Project* newOwner) {
		owner = newOwner;
		for (auto child : children) {
			child->__pointersInvalidated(newOwner);
		}
	}
};