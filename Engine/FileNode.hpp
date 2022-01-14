#pragma once

#include <string>
#include <vector>

struct FNode {

	static FNode GetFolderPath(const FNode* const file);

	std::string _path{};
	std::string _name{};
	std::string _name_noext{};
	std::string _ext{};
	FNode* _parent{ nullptr };
	std::vector<FNode> _children{};

	bool _isFile{ false };
	bool _isDir{ false };
};
