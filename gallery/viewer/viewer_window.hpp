#pragma once

#include <vkx/context.hpp>
#include <vkx/window.hpp>

class ViewerWindow : public vkx::Window
{
public:
    ViewerWindow(vkx::WindowCreateInfo const& info);

    void run() override;

private:
    std::unique_ptr<vkx::Context> m_context;
};
