#pragma once

#include <string>
#include <vector>

struct FNode {
	std::string _path;
	std::string _name;
	std::string _name_noext;
	std::string _ext;
	FNode* _parent;
	std::vector<FNode> _children;

	bool _isFile;
	bool _isDir;
};
