# Sfaui-Editor
# Project Introduction
Sfaui-Editor, short for SkyC for Android ImGui Editor, is a lightweight visual UI editor built on Dear ImGui that supports both Windows and Android platforms. It is an embeddable C++11 library. Developers no longer need to write massive ImGui rendering code manually; they can rapidly generate UI data by tweaking parameters in real time.

It comes with a full suite of native ImGui widgets including rectangles, images, text, buttons, child panels, and supports common operations such as multi-selection, insertion and deletion.
This tool serves multiple purposes: rapidly drafting ImGui interfaces and exporting corresponding code, lowering the learning curve for ImGui beginners to grasp ImGui functions, features and layout rules, and resolving the pain point of inconvenient UI editing for developers building ImGui-based mobile applications.

<p align="center"> <img src="https://github.com/user-attachments/assets/fcdb03a1-a936-458b-8f4b-b6ac1a8f0562" width="850" alt="Sfaui Editor Demo"> </p>

# Core Features & Design Principles
The editor reads and modifies parameter values of every ImGui function to replicate its rendering output automatically.
It is developed around ImGui’s fixed layout pipeline and standardized function naming convention. Since ImGui functions vary wildly in parameter count and data types, the editor draws inspiration from mRNA and tRNA translation mechanisms in biology and leverages C++ std::any (nonstd::any) to unify heterogeneous parameter storage.

```cpp
// data
   {
        "AddRect 空心矩形",
        {
            any(LLayout::AddRect),
            any(std::string("AddRect 空心矩形")),
            any(ImVec2(40,40)),
            any(ImVec2(40,40)),
            any(std::string("AddRect")),
            any((int)6),
            any(std::string("ImGui::GetWindowDrawList()->AddRect")),
            any(ImVec2(0,0)),
            any(ImVec2(100,100)),
            any((ImU32)IM_COL32(10,200,25,255)),
            any((float)10.0f),
            any((ImDrawFlags)ImDrawFlags_None),
            any((float)1.0f)
        }
    },

```
```cpp
//draw
    case AddRect:
                draw->AddRect(
                    any_cast<ImVec2>(arr[PARAMST]),
                    any_cast<ImVec2>(arr[PARAMST + 1]),
                    any_cast<ImU32>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3]),
                    any_cast<ImDrawFlags>(arr[PARAMST + 4]),
                    any_cast<float>(arr[PARAMST + 5]));
                break;
```

any erases concrete parameter types to enable unified storage for all function arguments. It also supports runtime type checking to feed modified values back to the editor’s parameter panel, drastically cutting down boilerplate code.
<p align="center"> <img src="https://github.com/user-attachments/assets/016ce36f-ebe0-4e87-9fd0-30a13c0b27f1" width="850" alt="Sfaui Editor Demo 2"> </p>

# Limitations
1.Due to the inherent characteristics of any, maybe pointer-type parameters cannot be safely validated. Mismatched value types during parameter editing cause "gene mutation" errors, resulting in crashes and runtime failures.

2.The editor enforces the exact same layout stack rules as raw ImGui. Invalid nesting (e.g., nesting BeginMainMenuBar inside another BeginMainMenuBar) will trigger ImGui debug warnings; on Android builds without debug tools, such malformed layouts lead to immediate crashes. Developers are advised to save project files frequently to avoid data loss from unexpected failures.

3.The editor only supports static layout generation. It cannot handle widgets that require conditional if branches to render correctly (e.g., ImGui::MenuItem("XXX")). It is not recommended for large monolithic UIs; split your interface into smaller modular blocks and integrate the exported code snippets separately into your project.

4.While most basic data types are supported, some parameters are not fully handled. Exported code is pseudocode that requires manual revision or AI-assisted adjustments to become fully executable.

If embedding this editor causes crashes or disrupts your project workflow, please remove all Sfaui-related source files immediately. The author apologizes for any inconvenience this may cause.
# Environment Dependencies & Integration Guide
C++11

The editor is fully embeddable into any project that runs Dear ImGui. It only exposes three simple public APIs via Sfaui.h.
 ```cpp
...
#include "Sfaui.h"

int main()
...
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    TexLoader_SetDXDevice(g_pd3dDevice, g_pd3dDeviceContext);

    Sfaui_init("./Sfaui.sam");// Bhind imgui init
...
     whlie(1){
       ...
       Sfaui_editing();

       ImGui::Render();
       }
...
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    
    Sfaui_shutdown();
...
    }
 ```
# Repository File Overview
If your project already integrates ImGui, simply download the Sfaui source bundle and CMake config — you do not need the modified ImGui folder provided here. Link against the compiled Sfaui library to finish integration.
For DX11 or OpenGL ES rendering demo implementations, navigate to the example folder for corresponding startup samples.
For Android projects using SDL + OpenGL ES rendering where ImGui fails to render content correctly, replace your backend files with imgui_impl_opengl.h / .cpp from the Sfaui folder included ImGui folder.

