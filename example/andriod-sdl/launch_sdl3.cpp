#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_touch.h>
#include <GLES2/gl2.h>
#include <cmath>
#include <string>
#include <vector>
#include <array>

#include "imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_opengl3.h"

#include "font.h"
#include "Sfaui.h"

#define ENABLE_TOUCH_SCROLL 1

#if ENABLE_TOUCH_SCROLL
static struct TouchScrollCtx
{
    bool pressed = false;
    ImVec2 touch_start{};
    ImVec2 touch_last{};
    float velocity_y = 0.f;
    float inertia_y = 0.f;
    float sensitivity = 0.12f;
    float friction = 15.0f;
    float inertia_scale = 0.0008f;
} g_touch_scroll;

static void UpdateTouchInertia(ImGuiIO& io)
{
    if (!io.WantCaptureMouse)
    {
        g_touch_scroll.inertia_y = 0.f;
        return;
    }
    if (fabsf(g_touch_scroll.inertia_y) > 0.05f)
    {
        float decay = 1.f - io.DeltaTime * g_touch_scroll.friction;
        g_touch_scroll.inertia_y *= decay;
        io.MouseWheel += g_touch_scroll.inertia_y * io.DeltaTime;
    }
    else
    {
        g_touch_scroll.inertia_y = 0.f;
    }
}
#endif

namespace {
struct AppState {
    SDL_Window* window = nullptr;
    SDL_GLContext gl_ctx = nullptr;
    bool imgui_ready = false;
    bool sfaui_ok = false;
    const char* glsl_version = "#version 100";
    ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.15f, 1.0f);
};
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_WindowFlags win_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    SDL_Window* win = SDL_CreateWindow("ImGui SDL3", 0, 0, win_flags);
    if (!win) {
        SDL_Log("CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return SDL_APP_FAILURE;
    }

    SDL_GLContext glc = SDL_GL_CreateContext(win);
    SDL_GL_MakeCurrent(win, glc);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    // 保留字体加载，其余样式全部移除
    ImFontConfig cfg;
    cfg.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryTTF((void*)font_v, (int)font_v_size, 36.0f, &cfg);

    ImGui_ImplSDL3_InitForOpenGL(win, glc);
    ImGui_ImplOpenGL3_Init("#version 100");

    // 安卓私有路径替换 /sdcard，解决权限崩溃
    bool sfaui_ok = false;
    try {
        Sfaui_init("");
        sfaui_ok = true;
    } catch(...) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Sfaui 文件打开失败");
    }

    AppState* state = new AppState{};
    state->window = win;
    state->gl_ctx = glc;
    state->imgui_ready = true;
    state->sfaui_ok = sfaui_ok;
    *appstate = state;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    AppState* st = static_cast<AppState*>(appstate);
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplSDL3_ProcessEvent(event);

    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE)
        return SDL_APP_SUCCESS;

#if ENABLE_TOUCH_SCROLL
    float sw = io.DisplaySize.x;
    float sh = io.DisplaySize.y;
    switch (event->type)
    {
    case SDL_EVENT_FINGER_DOWN:
        g_touch_scroll.pressed = true;
        g_touch_scroll.touch_start.x = event->tfinger.x * sw;
        g_touch_scroll.touch_start.y = event->tfinger.y * sh;
        g_touch_scroll.touch_last = g_touch_scroll.touch_start;
        g_touch_scroll.velocity_y = 0.f;
        g_touch_scroll.inertia_y = 0.f;
        break;
    case SDL_EVENT_FINGER_MOTION:
    {
        if (!g_touch_scroll.pressed) break;
        ImVec2 cur(event->tfinger.x * sw, event->tfinger.y * sh);
        ImVec2 delta = {cur.x - g_touch_scroll.touch_last.x, cur.y - g_touch_scroll.touch_last.y};
        g_touch_scroll.touch_last = cur;
        if (io.WantCaptureMouse)
        {
            float scroll_delta = delta.y * g_touch_scroll.sensitivity * 0.1f;
            io.MouseWheel += scroll_delta;
            g_touch_scroll.velocity_y = scroll_delta / io.DeltaTime;
        }
        else
        {
            g_touch_scroll.velocity_y = 0.f;
        }
        break;
    }
    case SDL_EVENT_FINGER_UP:
        g_touch_scroll.pressed = false;
        if (io.WantCaptureMouse)
        {
            g_touch_scroll.inertia_y = g_touch_scroll.velocity_y * g_touch_scroll.inertia_scale;
        }
        else
        {
            g_touch_scroll.inertia_y = 0.f;
        }
        break;
    default: break;
    }
#endif

    // 输入法切换
    static bool last_textinput = false;
    if (io.WantTextInput != last_textinput)
    {
        last_textinput = io.WantTextInput;
        if (io.WantTextInput)
            SDL_StartTextInput(st->window);
        else
            SDL_StopTextInput(st->window);
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    AppState* st = static_cast<AppState*>(appstate);
    if (!st || !st->window || !st->gl_ctx || !st->imgui_ready)
        return SDL_APP_FAILURE;
    ImGuiIO& io = ImGui::GetIO();

#if ENABLE_TOUCH_SCROLL
    UpdateTouchInertia(io);
#endif

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // 只有初始化成功才渲染编辑器
    if(st->sfaui_ok)
        Sfaui_editing();

    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(st->clear_color.x, st->clear_color.y, st->clear_color.z, st->clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(st->window);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    (void)result;
    AppState* st = static_cast<AppState*>(appstate);
    if (!st) return;

    if(st->sfaui_ok)
        Sfaui_shutdown();
    if (st->imgui_ready) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }
    if (st->gl_ctx)
        SDL_GL_DestroyContext(st->gl_ctx);
    if (st->window)
        SDL_DestroyWindow(st->window);
    delete st;
    SDL_Quit();
}
