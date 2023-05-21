#pragma once

struct ImGuiRenderCommand {

    virtual ~ImGuiRenderCommand() = 0;
    virtual void operator() (float delta) = 0;
};
