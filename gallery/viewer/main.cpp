#include "viewer_window.hpp"

int main()
{
    ViewerWindow win{
        vkx::WindowCreateInfo{.title        = "GLTF Vulkan Viewer",
                              .width        = 1700,
                              .height       = 900,
                              .is_maximized = true,
                              .is_resizable = true}
    };
    win.run();

    return 0;
}
