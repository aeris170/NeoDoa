#include "ProjectSerializer.hpp"

#include <tinyxml2.h>

#include "Project.hpp"
#include "Scene.hpp"

std::string SerializeProject(const Project& project) {
	tinyxml2::XMLPrinter printer;

	printer.PushComment("WARNING!! This file is not for editing! Don't!");
	printer.OpenElement("project");
	{
		printer.OpenElement("name");
		{
			printer.PushText(project._name.c_str());
		}
		printer.CloseElement(); // name close
		printer.OpenElement("startup");
		{
			if (project._startupScene) {
				printer.PushText(project._startupScene->_path.c_str());
			} else {
				printer.PushText("NULL");
			}
		}
		printer.CloseElement(); // name close
	}
	printer.CloseElement(); // project close
	return printer.CStr();
}
