#include "iga_exec.h"
#include "imgui.h"
#include "implot.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <stdio.h>
#include <chrono>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <GLFW/glfw3native.h>
#include "tracy/Tracy.hpp"

#include <windows.h>
#include <fmt/format.h>

#include "versioning/version.h"
#include "iga_time.h"


namespace iga::exec {

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void iconify(GLFWwindow* window, int iconified)
{
    //__debugbreak();
}

void IdleBySleeping(State::FpsIdling& ioIdling)
{
    ioIdling.isIdling = false;
    if ((ioIdling.fpsIdle > 0.f) && ioIdling.enableIdling) {
        double beforeWait  = iga::time::clock_seconds();
        double waitTimeout = 1. / (double)ioIdling.fpsIdle;

        glfwWaitEventsTimeout(waitTimeout);

        double afterWait        = iga::time::clock_seconds();
        double waitDuration     = (afterWait - beforeWait);
        double waitIdleExpected = 1. / ioIdling.fpsIdle;
        ioIdling.isIdling       = (waitDuration > waitIdleExpected * 0.9);
    }
}


GLFWwindow* setup(std::string_view app_name)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::exit(1);
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    const std::string release_type = version::release_type();
    std::string app_version_suffix = {};
    if (!release_type.empty()) {
        app_version_suffix = fmt::format("-{}", version::release_type());
    }
    const auto app_title = fmt::format("{} v{}{}", app_name, version::version(), app_version_suffix);
    auto window          = glfwCreateWindow(1280, 720, app_title.c_str(), nullptr, nullptr);
    if (!window) {
        std::exit(1);
    }

    auto icon
        = LoadImage(GetModuleHandleA(nullptr), "MAINICON", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_DEFAULTSIZE);
    auto hWnd = glfwGetWin32Window(window);
    SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
    SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)icon);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    glfwSetWindowIconifyCallback(window, iconify);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
    // io.ConfigFlags |= ImGuiConfigFlags_EnablePowerSavingMode;
    //  io.ConfigViewportsNoAutoMerge = true;
    //  io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular
    // ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return window;
}

void destroy(GLFWwindow* window)
{
    glfwDestroyWindow(window);
    window = nullptr;
    glfwTerminate();
}

void exec(GLFWwindow* window, State* state, ExecHooks& hooks, void* data)
{
    static ImVec4 background_colour = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(window)) {
        IdleBySleeping(state->idler);
        glfwPollEvents();

        if (hooks.preframe) hooks.preframe(data);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        hooks.render(data);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(background_colour.x * background_colour.w, background_colour.y * background_colour.w,
                     background_colour.z * background_colour.w, background_colour.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to
        // paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        if (hooks.postframe) hooks.postframe(data);

        glfwSwapBuffers(window);
        FrameMark;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}


} // namespace iga::exec
