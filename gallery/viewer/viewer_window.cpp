#include "viewer_window.hpp"

#include <GLFW/glfw3.h>

ViewerWindow::ViewerWindow(vkx::WindowCreateInfo const& info) :
    Window{info}
{}

void ViewerWindow::run()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
    }
}
