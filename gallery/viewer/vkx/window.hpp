#pragma once

#include <string>

struct GLFWwindow;

namespace vkx
{
    struct WindowCreateInfo
    {
        std::string title;
        int width{0};
        int height{0};
        bool is_maximized{false};
        bool is_resizable{false};
    };

    class Window
    {
    public:
        Window(WindowCreateInfo const& info);
        ~Window();

        virtual void run() = 0;

    protected:
        virtual void on_mouse_press(int button, int action, int mods, double x, double y);
        virtual void on_mouse_move(double x, double y);
        virtual void on_key_press(int key, int scancode, int action, int mods);
        virtual void on_window_size(int width, int height);
        virtual void on_framebuffer_size(int width, int height);
        virtual void on_char(unsigned int codepoint);
        virtual void on_close();

        GLFWwindow* m_window{nullptr};
    };
} // namespace vkx
