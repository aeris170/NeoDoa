#include <Editor/MenuBar.hpp>

#include <stb_image.h>

#include <Utility/ConstexprConcat.hpp>

#include <Engine/Core.hpp>
#include <Engine/Log.hpp>
#include <Engine/Angel.hpp>
#include <Engine/Texture.hpp>
#include <Engine/TextureDeserializer.hpp>

#include <Editor/GUI.hpp>
#include <Editor/Icons.hpp>
#include <Editor/Strings.hpp>
#include <Editor/GUICommand.hpp>
#include <Editor/ImGuiExtensions.hpp>

MenuBar::MenuBar(GUI& owner) noexcept :
    gui(owner),
    aboutSection(*this) {}

bool MenuBar::Begin() noexcept { return true; }

void MenuBar::Render() noexcept {
	GUI& gui = this->gui;

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            RenderFileSubMenu();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            RenderEditSubMenu();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Assets")) {
            RenderAssetsSubMenu();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            RenderHelpSubMenu();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // due to how imgui works, these must be outside the begin/end menubar :(
    aboutSection.RenderAboutPopup();
}

void MenuBar::End() noexcept {}

void MenuBar::RenderFileSubMenu() noexcept {
    GUI& gui = this->gui;
	if (ImGui::MenuItem("New Scene", GUI::Shortcuts::NewSceneShortcut, nullptr, gui.HasOpenProject())) {
		FNode* currentFolder = gui.GetAssetManager().GetCurrentFolder();
		assert(currentFolder != nullptr);
		gui.ShowNewSceneAssetModal(*currentFolder);
	}
	if (ImGui::BeginMenu("Open Scene...", gui.HasOpenProject())) {
		const auto& assets = gui.CORE->GetAssets();
		for (const auto& uuid : assets->SceneAssetIDs()) {
			AssetHandle sceneAsset = assets->FindAsset(uuid);
			if (ImGui::MenuItem(sceneAsset.Value().File().Name().data(), nullptr, nullptr)) {
				gui.OpenScene(sceneAsset);
			}
		}
		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Save Scene", GUI::Shortcuts::SaveSceneShortcut, nullptr, gui.HasOpenScene())) {
		gui.SaveScene();
	}
	ImGui::Separator();
    if (ImGui::MenuItem("New Project", GUI::Shortcuts::NewProjectShortcut)) {
		//gui.ShowNewProjectModal();
    }
    if (ImGui::MenuItem("Open Project...", GUI::Shortcuts::OpenProjectShortcut)) {
		//gui.ShowOpenProjectModal();
    }
    if (ImGui::MenuItem("Save Project", GUI::Shortcuts::SaveProjectShortcut, nullptr, gui.HasOpenProject())) {
        gui.SaveProjectToDisk();
    }
    if (ImGui::MenuItem("Close Project", GUI::Shortcuts::CloseProjectShortcut, nullptr, gui.HasOpenProject())) {
        gui.CloseProject();
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Exit", GUI::Shortcuts::ExitProgramShortcut)) {
        gui.CORE->Stop();
    }
}

void MenuBar::RenderEditSubMenu() noexcept {
    GUI& gui = this->gui;

    std::string undoText = ICON_FA_ARROW_ROTATE_LEFT " Undo";
    if (gui.CanUndoLastCommand()) {
        undoText += " - ";
        undoText += dynamic_cast<const GUICommand&>(gui.GetCommandHistory().PeekUndoStack()).GetDescription();
    }
    if (ImGui::MenuItem(undoText.c_str(), GUI::Shortcuts::UndoShortcut, nullptr, gui.CanUndoLastCommand())) {
        gui.UndoLastCommand();
    }

    std::string redoText = ICON_FA_ARROW_ROTATE_RIGHT " Redo";
    if (gui.CanRedoLastCommand()) {
        redoText += " - ";
        redoText += dynamic_cast<const GUICommand&>(gui.GetCommandHistory().PeekRedoStack()).GetDescription();
    }
    if (ImGui::MenuItem(redoText.c_str(), GUI::Shortcuts::RedoShortcut, nullptr, gui.CanRedoLastCommand())) {
        gui.RedoLastCommand();
    }

	ImGui::Separator();

	if (ImGui::MenuItem(WindowStrings::UndoRedoHistoryWindowTitle)) {
		gui.GetUndoRedoHistory().Show();
	}
}

void MenuBar::RenderAssetsSubMenu() noexcept {
    GUI& gui = this->gui;
}

void MenuBar::RenderHelpSubMenu() noexcept {
    if (ImGui::MenuItem(AboutSection::ABOUT_BUTTON_TEXT)) {
        aboutSection.ab = true;
    }
}

// Inner struct: About Section
MenuBar::AboutSection::AboutSection(MenuBar& owner) noexcept :
    mb(owner),
    licences({
        { "NeoDoa", reinterpret_cast<const char*>(u8R"(# SOFTWARE LICENSE AGREEMENT
This Software License Agreement ("Agreement") is made and entered into as of 2024-02-10 ("Effective Date") by and between Doğa Oruç, a private person, having its principal place of business in Türkiye ("Licensor"), and Licensee, either a private person, a registered company, or a partnership, having its principal place of business anywhere ("Licensee").

WHEREAS, the Licensor owns certain software that it desires to license to the Licensee;

WHEREAS, Licensee desires to use such software under the terms and conditions set forth herein.

NOW, THEREFORE, in consideration of the mutual promises contained herein and for other good and valuable consideration, the parties agree as follows:

## 1. Definition of Software:
The term "Software" refers to the **NeoDoa**, including any updates, modifications, or associated documentation provided by the Licensor.

## 2. Grant of License:
Subject to the terms and conditions of this Agreement, the Licensor grants the Licensee a non-exclusive, non-transferable license to use the Software. To apply, reach out to the Licensor.

## 3. Derivative Works:
All modifications and derivative works must be submitted as Pull Requests to the [official **NeoDoa** GitHub repository](https://github.com/aeris170/NeoDoa). Without supervision from the Licensor, no modifications or derivative works are allowed.

## 4. Commercial Use:
Commercial use is permitted on an application basis. To apply for commercial use, reach out to the Licensor.

## 5. Attribution:
Licensee is required to provide attribution to Licensor in the form of a fade in/out **NeoDoa** social logo staying on the screen no shorter than 5 seconds (skippable with keypresses) first thing at the start of their program.

## 6. Intellectual Property Rights:
All intellectual property rights in the Software shall remain the property of the Licensor. The Licensee does not acquire any rights to the Software except for the limited use rights specified in this Agreement.

## 7. Warranty and Liability:
The Software is provided "as is" without warranty of any kind. Licensor shall not be liable for any damages arising out of or related to the use or inability to use the Software.

## 8. Termination:
This Agreement shall terminate automatically if the Licensee breaches any of its terms and conditions. Upon termination, Licensee must cease all use of the Software and destroy all copies.

## 9. Governing Law:
This Agreement shall be governed by and construed under the laws of Türkiye.

IN WITNESS WHEREOF, the parties have executed this Software License Agreement as of the Effective Date.

Licensor: Doğa Oruç

Licensee: Licensee)")},
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
	to use this software free of charge, with or without modifications. Have fun : )" }}) {

	stbi_set_flip_vertically_on_load(true);
	int w, h, nrChannels;
	auto* readPixels = stbi_load("Images/social.png", &w, &h, &nrChannels, STBI_rgb_alpha);

	GPUTextureBuilder builder;
	builder.SetName("!!neodoa_banner!!");
	if (readPixels) {
		std::span pixels{ reinterpret_cast<const std::byte*>(readPixels), w * h * nrChannels * sizeof(stbi_uc) };
		builder.SetWidth(w)
			.SetHeight(h)
			.SetData(DataFormat::RGBA8, pixels);
	} else {
		const Texture& texture = Texture::Missing();
		builder.SetWidth(texture.Width)
			.SetHeight(texture.Height)
			.SetData(texture.Format, texture.PixelData);
	}
	auto [tex, _] = builder.Build();
	neodoaBanner = std::move(tex.value());

	stbi_image_free(readPixels);
}

void MenuBar::AboutSection::RenderAboutPopup() noexcept {
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
        ImGui::TextColored({ 0.7f, 0.7f, 0.7f, 1.0f }, PRODUCT_NAME);
        ImGui::PopFont();
        ImGui::Image(neodoaBanner, { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().x / 2 }, { 0, 1 }, { 1, 0 });
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

void MenuBar::AboutSection::RenderLicenceNotices() noexcept {
    GUI& gui = mb.get().gui;
    for (auto& [name, licence] : licences) {
        ImGui::PushFont(gui.GetFont());
        ImGui::TextColored({ 0.7f, 0.7f, 0.7f, 1.0f }, "%s", name.c_str());
        ImGui::PopFont();
        auto title = (std::string("License###") + name);
        if (ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)) {
            ImGui::TextUnformatted(licence.c_str());
        }
    }
}
// Inner struct: About Section