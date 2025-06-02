#pragma once

#include <string>
#include <string_view>


struct GLFWwindow;

namespace iga::exec {

void glfw_error_callback(int error, const char* description);

void iconify(GLFWwindow* window, int iconified);


struct State {
    struct FpsIdling {
        float fpsIdle     = 3.f;   // FPS when idling
        bool enableIdling = true;  // a bool to enable/disable idling
        bool isIdling     = false; // an output parameter filled by the runner
    } idler;

    std::string application_name;
};


struct ExecHooks {
    void (*preframe)(void*)  = nullptr;
    void (*render)(void*)    = nullptr;
    void (*postframe)(void*) = nullptr;
};


GLFWwindow* setup(std::string_view app_name);
void destroy(GLFWwindow* window);

void exec(GLFWwindow* window, State* state, ExecHooks& hooks, void* data);


struct Hooks {
    void (*preframe)(void*)  = nullptr;
    void (*render)(void*)    = nullptr;
    void (*postframe)(void*) = nullptr;

    void (*setup)(void*, GLFWwindow*)    = nullptr;
    void (*teardown)(void*, GLFWwindow*) = nullptr;
};


template <typename T>
void pre_frame(void* ptr)
{
    reinterpret_cast<T*>(ptr)->pre_frame();
}

template <typename T>
void post_frame(void* ptr)
{
    reinterpret_cast<T*>(ptr)->post_frame();
}

template <typename T>
void render_frame(void* ptr)
{
    reinterpret_cast<T*>(ptr)->render_frame();
}

template <typename T>
void setup(void* ptr, GLFWwindow* window)
{
    reinterpret_cast<T*>(ptr)->setup(window);
}

template <typename T>
void teardown(void* ptr, GLFWwindow* window)
{
    reinterpret_cast<T*>(ptr)->teardown(window);
}

template <typename T>
void run(State* state, T* app)
{
    Hooks hooks{&pre_frame<T>, &render_frame<T>, &post_frame<T>, &setup<T>, &teardown<T>};
    run(state, hooks, app);
}

inline void run(State* state, Hooks& hooks, void* data = nullptr)
{
    auto glfw_window = setup(state->application_name);
    if (hooks.setup) {
        hooks.setup(data, glfw_window);
    }

    ExecHooks exec_hooks = {hooks.preframe, hooks.render, hooks.postframe};
    exec(glfw_window, state, exec_hooks, data);

    if (hooks.teardown) {
        hooks.teardown(data, glfw_window);
    }

    destroy(glfw_window);
}


} // namespace iga::exec
