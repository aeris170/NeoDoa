#include "MenuBar.hpp"

#include <imgui.h>
#include <tinyfiledialogs.h>

#include <Core.hpp>
#include <Log.hpp>
#include <Angel.hpp>
#include <Texture.hpp>

#include "GUI.hpp"

MenuBar::MenuBar(GUI& owner) :
	gui(owner),
	aboutSection(*this) {}

void MenuBar::Begin() {}

void MenuBar::Render() {
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Project")) {
			RenderProjectSubMenu();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Scene")) {
			RenderSceneSubMenu();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help")) {
			RenderHelpSubMenu();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// due to how imgui works, this must be outside the begin/end menubar :(
	aboutSection.RenderAboutPopup();
}

void MenuBar::End() {}

void MenuBar::RenderProjectSubMenu() {
	GUI& gui = this->gui;
	if (ImGui::MenuItem("New Project", "Ctrl+Shift+N")) {
		if (tinyfd_messageBox("Warning", "You may have unsaved changes. Are you sure you want to create a new project?", "yesno", "warning", 0)) {
			const char* path = tinyfd_selectFolderDialog("Select a folder for New Project", "");
			if (path) {
				const char* name = nullptr;
				bool badName = true;
				while (badName) {
					name = tinyfd_inputBox("Enter a name for the New Project", "Enter a name for the New Project", "New Project");
					badName = name == nullptr || std::string(name) == "";
					if (badName) {
						tinyfd_messageBox("Warning", "Projects cannot be unnamed.", "ok", "warning", 1);
					}
				}
				gui.CloseProject();
				gui.CreateNewProject(path, name);
				gui.SaveProjectToDisk();
				DOA_LOG_INFO("Succesfully created a new project named %s at %s", name, name);
			}
		}
	}
	if (ImGui::MenuItem("Open Project...", "Ctrl+Shift+O")) {
		if (tinyfd_messageBox("Warning", "You may have unsaved changes. Are you sure you want to open another project?", "yesno", "warning", 0)) {
			static const char* const types[] = { "*.doa" };
			gui.core->Angel()->_scriptLoaderMutex.lock();
			const char* path = tinyfd_openFileDialog("Select Project File", nullptr, 1, types, "NeoDoa Project Files", 0);
			gui.core->Angel()->_scriptLoaderMutex.unlock();
			if (path) {
				gui.CloseProject();
				gui.OpenProjectFromDisk(path);
			}
		}
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Save Project", "Ctrl+Shift+S")) {
		gui.SaveProjectToDisk();
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Exit", "Alt+F4")) {
		gui.core->Stop();
	}
}

void MenuBar::RenderSceneSubMenu() {
	GUI& gui = this->gui;
	if (ImGui::MenuItem("New Scene", "Ctrl+N", nullptr, gui.HasOpenProject())) {
		gui.CreateNewScene("", "New Scene");
	}
	if (ImGui::BeginMenu("Open Scene...", gui.HasOpenProject())) {
		Assets& assets = gui.openProject->Assets();
		for (auto& uuid : assets.SceneAssetIDs()) {
			AssetHandle sceneAsset = assets.FindAsset(uuid);
			if (ImGui::MenuItem(sceneAsset.Value().File()->Name().c_str(), nullptr, nullptr)) {
				gui.openProject->OpenScene(sceneAsset);
			}
		}
		ImGui::EndMenu();
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
		if(gui.HasOpenScene()) {
			gui.openProject->SaveOpenSceneToDisk();
		}
	}
}

void MenuBar::RenderHelpSubMenu() {
	if (ImGui::MenuItem(AboutSection::ABOUT_BUTTON_TEXT)) {
		aboutSection.ab = true;
	}
}

// Inner struct: About Section
MenuBar::AboutSection::AboutSection(MenuBar& owner) :
	mb(owner),
	neodoaBanner(CreateTexture("!!neodoa_banner!!", "Images/social.png")),
	licences({
		{ "NeoDoa", R"(NeoDoa Public Licence

	> Copyright(C)[2022][Doga Oruc]

	> NeoDoa Public Licence
	> TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

	1. Do whatever you like with the original work, just don't be a dick.

	    Being a dick includes - but is not limited to - the following instances:

		1a. Outright copyright infringement - Don't just copy this and change the name.
		1b. Selling the unmodified original with no work done what - so - ever, that's REALLY being a dick.
		1c. Modifying the original work to contain hidden harmful content. That would make you a PROPER dick.

	2. If you become rich through modifications, related works / services, or supporting the original work,
	share the love. Only a dick would make loads off this work and not buy the original work's
	creator(s) a pint.

	3. Software is provided with no warranty. Asking for help won't make you a dick, but asking someone to
	write your code for you makes you a DONKEY dick. If you happen to solve your problem before any help arrives,
	you would submit the fix back to regain your status of non-dick.)"},
		{ "AngelScript", R"(AngelCode Scripting Library
	Copyright © 2003 - 2020 Andreas Jönsson

	This software is provided 'as-is', without any express or implied warranty.In no event will the authors be
	held liable for any damages arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose, including commercial applications, and
	to alter itand redistribute it freely, subject to the following restrictions :

	The origin of this software must not be misrepresented; you must not claim that you wrote the original software.
	If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

	Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

	This notice may not be removed or altered from any source distribution.)"},
		{ "Assimp", R"(Open Asset Import Library (assimp)

	Copyright (c) 2006-2016, assimp team
	All rights reserved.

	Redistribution and use of this software in source and binary forms,
	with or without modification, are permitted provided that the
	following conditions are met:

	* Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
	  following disclaimer.

	* Redistributions in binary form must reproduce the above
	  copyright notice, this list of conditions and the
	  following disclaimer in the documentation and/or other
	  materials provided with the distribution.

	* Neither the name of the assimp team, nor the names of its
	  contributors may be used to endorse or promote products
	  derived from this software without specific prior
	  written permission of the assimp team.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



	******************************************************************************

	AN EXCEPTION applies to all files in the ./test/models-nonbsd folder.
	These are 3d models for testing purposes, from various free sources
	on the internet. They are - unless otherwise stated - copyright of
	their respective creators, which may impose additional requirements
	on the use of their work. For any of these models, see
	<model-name>.source.txt for more legal information. Contact us if you
	are a copyright holder and believe that we credited you inproperly or
	if you don't want your files to appear in the repository.


	******************************************************************************

	Poly2Tri Copyright (c) 2009-2010, Poly2Tri Contributors
	http://code.google.com/p/poly2tri/

	All rights reserved.
	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.
	* Neither the name of Poly2Tri nor the names of its contributors may be
	  used to endorse or promote products derived from this software without specific
	  prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
	PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.)"},
		{ "EnTT", R"(The MIT License (MIT)

	Copyright (c) 2017-2020 Michele Caini

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copy of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copy or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.)" },
		{ "GLEW", R"(The OpenGL Extension Wrangler Library
	Copyright (C) 2002-2007, Milan Ikits <milan ikits[]ieee org>
	Copyright (C) 2002-2007, Marcelo E. Magallon <mmagallo[]debian org>
	Copyright (C) 2002, Lev Povalahev
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.
	* The name of the author may be used to endorse or promote products
	  derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
	THE POSSIBILITY OF SUCH DAMAGE.


	Mesa 3-D graphics library
	Version:  7.0

	Copyright (C) 1999-2007  Brian Paul   All Rights Reserved.

	Permission is hereby granted, free of charge, to any person obtaining a
	copy of this software and associated documentation files (the "Software"),
	to deal in the Software without restriction, including without limitation
	the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
	BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
	AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
	CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


	Copyright (c) 2007 The Khronos Group Inc.

	Permission is hereby granted, free of charge, to any person obtaining a
	copy of this software and/or associated documentation files (the
	"Materials"), to deal in the Materials without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Materials, and to
	permit persons to whom the Materials are furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Materials.

	THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.)" },
		{ "GLFW", R"(Copyright (c) 2002-2006 Marcus Geelnard

	Copyright (c) 2006-2019 Camilla Löwy

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would
	   be appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not
	   be misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	   distribution.)" },
		{ "GLM", R"(================================================================================
	OpenGL Mathematics (GLM)
	--------------------------------------------------------------------------------
	GLM is licensed under The Happy Bunny License or MIT License

	================================================================================
	The Happy Bunny License (Modified MIT License)
	--------------------------------------------------------------------------------
	Copyright (c) 2005 - G-Truc Creation

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	Restrictions:
	 By making use of the Software for military purposes, you choose to make a
	 Bunny unhappy.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.

	================================================================================
	The MIT License
	--------------------------------------------------------------------------------
	Copyright (c) 2005 - G-Truc Creation

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.)" },
		{ "IconFontCppHeaders", R"(Copyright (c) 2017 Juliette Foucaut and Doug Binks

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.)" },
		{ "Dear ImGui", R"(The MIT License (MIT)

	Copyright (c) 2014-2020 Omar Cornut

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.)" },
		{ "STBI", R"(This software is available under 2 licenses -- choose whichever you prefer.
	------------------------------------------------------------------------------
	ALTERNATIVE A - MIT License
	Copyright (c) 2017 Sean Barrett
	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
	of the Software, and to permit persons to whom the Software is furnished to do
	so, subject to the following conditions:
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
	------------------------------------------------------------------------------
	ALTERNATIVE B - Public Domain (www.unlicense.org)
	This is free and unencumbered software released into the public domain.
	Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
	software, either in source code form or as a compiled binary, for any purpose,
	commercial or non-commercial, and by any means.
	In jurisdictions that recognize copyright laws, the author or authors of this
	software dedicate any and all copyright interest in the software to the public
	domain. We make this dedication for the benefit of the public at large and to
	the detriment of our heirs and successors. We intend this dedication to be an
	overt act of relinquishment in perpetuity of all present and future rights to
	this software under copyright law.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
	ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.)" },
		{ "TinyFileDialogs", R"(This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.
	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:
	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software.  If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.)" },
		{ "TinyXML2", R"(This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must
	not claim that you wrote the original software. If you use this
	software in a product, an acknowledgment in the product documentation
	would be appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and
	must not be misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.)" },
		{ "EZEasing", R"(Zamazingo Licence[2021 - 2022 Doga Oruc]

	From now on, the term "library owner" refers to "Doga Oruc[aeris170]"

	Redistribution and use in source and binary forms,
	with or without modification, are permitted provided
	that the following conditions are met:

		1.  Redistributions of source code must retain this
			copyright notice in whole. Without any modification(s).

		2.	Redistributions in binary form must reproduce this
			copyright notice in whole. Without any modification(s).

		3.	Redistributions	must acknowledge the fact that this library
			is created and maintained by the library owner.

		4.  Redistributions must give credit if this library is used in
			production of a software.

		5.	In no event shall the library owner be liable for any direct,
			indirect, incidental, special, exemplary, or consequential damages
			(including, but not limited to, procurement of substitute goods or
			services; loss of use, data, or profits; or business interruption)

	In the event that YOU, the library user, accept these terms, you are free
	to use this software free of charge, with or without modifications. Have fun : )" }
	}) {}

void MenuBar::AboutSection::RenderAboutPopup() {
	GUI& gui = mb.get().gui;
	if (ab) {
		ImGui::OpenPopup(ABOUT_POPUP_TITLE_TEXT);
		ab_open = true;
	}

	auto center = ImGui::GetMainViewport()->GetCenter();
	static ImVec2 size{ 500, 400 };
	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowPos({ center.x - size.x / 2, center.y - size.y / 2 });

	if (ImGui::BeginPopupModal(ABOUT_POPUP_TITLE_TEXT, &ab_open, ImGuiWindowFlags_NoResize)) {
		ImGui::PushFont(gui.GetFont());
		ImGui::TextColored({ 0.7, 0.7, 0.7, 1 }, PRODUCT_NAME);
		ImGui::PopFont();
		ImGui::Image((void*)neodoaBanner.lock()->_glTextureID, { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().x / 2 }, { 0, 1 }, { 1, 0 });
		ImGui::Text(PRODUCT_DESCRIPTION);
		ImGui::Dummy({ 0, 20 });
		if (ImGui::Button(LIBS_BUTTON_TEXT, { ImGui::GetContentRegionAvail().x, 30 })) {
			ImGui::OpenPopup(LIBS_POPUP_TITLE_TEXT);
			lib_open = true;
		}

		static ImVec2 libsSize{ 800, 600 };
		ImGui::SetNextWindowSize(libsSize);
		ImGui::SetNextWindowPos({ center.x - libsSize.x / 2, center.y - libsSize.y / 2 });
		if (ImGui::BeginPopupModal(LIBS_POPUP_TITLE_TEXT, &lib_open, ImGuiWindowFlags_NoResize)) {
			RenderLicenceNotices();
			ImGui::EndPopup();
		}
		ImGui::EndPopup();
	} else {
		ab = false;
	}
}

void MenuBar::AboutSection::RenderLicenceNotices() {
	GUI& gui = mb.get().gui;
	for (auto& [name, licence] : licences) {
		ImGui::PushFont(gui.GetFont());
		ImGui::TextColored({ 0.7, 0.7, 0.7, 1 }, name.c_str());
		ImGui::PopFont();
		auto title = (std::string("License###") + name);
		if (ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)) {
			ImGui::Text(licence.c_str());
		}
	}
}
// Inner struct: About Section