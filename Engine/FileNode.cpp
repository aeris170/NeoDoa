#include "FileNode.hpp"

FNode FNode::GetFolderPath(const FNode* const file) {
	if (file->_isDir) { return *file; }
	if (file->_parent) { return *file->_parent; }

	FNode rv = *file;
	std::size_t found = rv._path.find_last_of("/\\");
	rv._path = rv._path.substr(0, found);
	rv._isDir = true;
	rv._isFile = false;
	return rv;
}
