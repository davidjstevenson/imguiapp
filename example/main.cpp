
#include "versioning/version.h"

#include "imguiapp/application.h"
#include "imguiapp/fonts/fonts.h"
#include "imguiapp/imgui_exec.h"
#include "imguiapp/imgui_scope.h"
#include "imguiapp/imgui_stl.h"
#include "imguiapp/imgui_debug.h"
#include "imguiapp/imgui_comp.h"

#include <fmt/format.h>


class Application
{
public:
    std::shared_ptr<iga::fonts::Fonts> fonts;
    iga::comp::MenuItem menu;
    ImGuiID dockspace_id;
    iga::app::DebugWindows debug_windows;
    iga::exec::State iga_state;

    void pre_frame() { fonts->update(); }
    void post_frame() {}

    void render_frame()
    {
        auto font = iga::fonts::Font(fonts->OpenSans, 18);

        iga::app::render_dockspace(&menu, &dockspace_id);
        iga::app::render_debug_windows(&debug_windows, &iga_state);

        iga::scope::Window window;
        window.Begin("Test");
    }

    void setup(GLFWwindow* window)
    {
        using iga::comp::MenuItem;

        fonts = iga::fonts::init({});

        // clang-format off
        menu.name = "MenuBar";
        menu.data = MenuItem::Menu{{
            {"File", MenuItem::Menu {{
                {"Quit", MenuItem::Action{
                    [](void* window) { glfwSetWindowShouldClose((GLFWwindow*)window, 1); },
                    window
                }}
            }}},
#ifdef INCLUDE_DEBUG_FEATURES
            {"Debug", iga::app::create_debug_menu(debug_windows)},
#endif
        }};
        // clang-format on


        iga::app::init_default_style();
    }

    void teardown(GLFWwindow* window) {}
};


int main()
{
    fmt::println("{}", version::version());

    Application app;
    iga::exec::run(&app.iga_state, &app);

    return 0;
}
