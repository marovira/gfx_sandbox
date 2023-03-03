#include "viewer_window.hpp"

#include <GLFW/glfw3.h>

ViewerWindow::ViewerWindow(vkx::WindowCreateInfo const& info) :
    Window{info}
{
    // Vulkan is initialised, so create the context.
    m_context = std::make_unique<vkx::Context>(m_window);
}

void ViewerWindow::run()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
    }
}
