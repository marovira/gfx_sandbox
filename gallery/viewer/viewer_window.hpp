#pragma once

#include <vkx/window.hpp>

class ViewerWindow : public vkx::Window
{
public:
    ViewerWindow(vkx::WindowCreateInfo const& info);

    void run() override;

private:
};
