//专门为安卓平台搞的一个Sfaui editor使用实例。
//A Sfaui editor usage instance specifically designed for the Android platform
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_touch.h>
#include <GLES2/gl2.h>
#include <string>
#include <vector>
#include <array>


#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_notify.h"

#include "font.h"
#include "Sfaui.h"

//  触屏滑动总开关 0=关闭(原生滚动条) 1=开启手指滑屏
//Touch screen sliding master switch 0=Off (native scrollbar) 1=On finger sliding screen
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

int main(int, char**)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    const char* glsl_version = "#version 100";

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);

    SDL_Window* window = SDL_CreateWindow(
                             "ImGui 架",
                             SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             0, 0,
                             window_flags
                         );

    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return 1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImFontConfig cfg;
    cfg.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryTTF(
        (void*)font_v,
        (int)font_v_size,
        36.0f,
        &cfg
    );

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    Sfaui_init("/sdcard/i.sam"); //init Sfaui editor

    ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.15f, 1.00f);
    bool done = false;
    bool last = false;

    int DrawNewUilog = 0;

    while (!done)
    {
        SDL_Event event;
        if (io.WantTextInput != last)
        {
            last = io.WantTextInput;
            if (io.WantTextInput)
                SDL_StartTextInput();
            else
                SDL_StopTextInput();
        }
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;

#if ENABLE_TOUCH_SCROLL
            float sw = io.DisplaySize.x;
            float sh = io.DisplaySize.y;
            switch (event.type)
            {
            case SDL_FINGERDOWN:
                g_touch_scroll.pressed = true;
                g_touch_scroll.touch_start.x = event.tfinger.x * sw;
                g_touch_scroll.touch_start.y = event.tfinger.y * sh;
                g_touch_scroll.touch_last = g_touch_scroll.touch_start;
                g_touch_scroll.velocity_y = 0.f;
                g_touch_scroll.inertia_y = 0.f;
                break;
            case SDL_FINGERMOTION:
            {
                if (!g_touch_scroll.pressed) break;
                ImVec2 cur(event.tfinger.x * sw, event.tfinger.y * sh);
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
            case SDL_FINGERUP:
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
            default:
                break;
            }
#endif
        }

#if ENABLE_TOUCH_SCROLL
        UpdateTouchInertia(io);
#endif

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        Sfaui_editing(); //Draw Sfaui editor

        ImGui::Render();

        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    Sfaui_shutdown();  //shutdown Sfaui editor

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
