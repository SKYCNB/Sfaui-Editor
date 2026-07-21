/*
Sfaui-Editor Cross-platform ImGui UI Editor
Copyright (c) 2026 SKYCNB
MIT License
See LICENSE file for full license information.
*/


//version v1.0
//for dear imgui 1.92.6
#ifndef SFAUI_H
#define SFAUI_H

//#include "Sfaui_value.h"
#include "Sfaui_listUi.h"
#include "Sfaui_wdUi.h"


//SkyC for Android ImGui -  Sfaui
//2026 by-skyc

//Sfaui Source Repository : https://github.com/SKYCNB/Sfaui-Editor.git

// Sfaui 编辑器基于 Dear ImGui 开源库开发
// This editor is built entirely on the open-source Dear ImGui library,
// ImGui 官方相关链接 Official Links:
// 项目源码仓库 Source Repository: https://github.com/ocornut/imgui

//
//项目特性:由于sfaui追求快速见效的效果，用生物的转录和翻译来对其模拟，实现imgui函数的差异抹平
//不会对特别多的函数特例化（搞屎山）,所以有些函数在修改属性的时候参数数量可能会缺斤少量，有时可
//以严重到不能改有关键效果的参数，甚至有些参数改完后闪退，不过应该也没必要管，因为本编辑器只是
//作为一个轻量，简便草稿的编辑，项目的参数适配如果不打严谨容易"基因突变"
//用any搞的项目时非常敏感的，加上imgui的机制，比如不规范的堆叠几个控件，这样，双重闪退buff叠在
//一起，有时候闪退是不可避免的

//更新日志:
//beta v0.1 添加了弹出键盘从而能编辑编辑框,适配了一些文本控件
//以及一些id问题
//2026-6-3-23-27

//beta v0.2 适配了Imvec2按屏幕高宽值来调整,把imgui和realty的
//id分开,继续适配了一些普通控件
//2026-6-4-23-19

//beta v0.3 继续适配了一些控件，比如滑块等
//2026-6-7-22-52

//beta v0.3.2 适配了大量绘制函数，适配了IMCOL32的参数修改，解决了
//修改id或编辑框修改闪退问题
//2026-6-8-22-37

//beta v0.4 几乎适配了1.9.2大部分控件，添加图片纹理转换功能
//2026-6-9-11-50

//beta v0.5 添加了必要的功能:控件反选，多选，全选，去选, 复制，删除
//美化了编辑器悬浮窗和可以收起展开了
//2026-6-9-22-33

//beta v0.6 适配了很多imgui枚举在Setting的选项，适配了重要的颜色，样式
//控件
//2026-6-10-13-53

//beta v0.7 把复制操作作为独立的操作不在自动插值，新增了粘贴操作，插值操作
//改善了控件修改参数而闪退的问题
//2026-6-13-17-08

//beta v0.8 增加了可见功能，撤回前进功能，增加了绘制模板的指针类型的稳定性
//让修改参数更安全，还把项目的库索引重构了，开发者们只需要在自己的项目导入
//  "Sfaui.h",然后就能简单的在自己的开发场景中使用编辑器打草稿,最后更重要的
//是，新增了代码导出功能，能直接导出代码了,不过生成代码还需要开发者们进一步
//还原和修复代码才能嵌入自己项目之中
//2026-6-20-21-12

//beta v0.9 添加了Imvec2的拖拽设置功能，添加了本项目最重要的功能:保存和导入功能
//2026-7-4-21-23

//v1.0 完善了开发者在使用编辑器的各种情况，正式发布Sfaui editor v1.0
//2026-7-20

// SkyC for Android ImGui - Sfaui
// 2026 Developed by skyc

// Project Features: To achieve fast iteration results, Sfaui simulates the biological processes of transcription and translation
// to unify the differences between various ImGui functions.
// Excessive special-case handling for individual functions is avoided (to prevent messy, unmaintainable code).
// As a result, some functions may lack certain parameters when modifying properties.
// In extreme cases, key functional parameters may be missing entirely, and editing certain parameters can cause crashes.
// However, this is deemed acceptable, as this editor is only intended as a lightweight, convenient drafting tool.
// Rigorous parameter matching would introduce unnecessary overhead and lead to unpredictable "genetic mutation" style bugs.

// This project built around nonstd::any is extremely sensitive to type mismatches.
// Combined with ImGui's internal rendering mechanism, two layers of crash triggers overlap,
// making crashes unavoidable under certain edge cases.


// Beta v0.1
// Added pop-up soft keyboard support for input field editing; compatibility adjustments for several text widgets;
// Fixed multiple ID-related bugs
// 2026-06-03 23:27

// Beta v0.2
// Added screen-size-based adjustment support for ImVec2 values; isolated ImGui internal IDs from editor runtime IDs;
// Compatibility tweaks for additional common widgets
// 2026-06-04 23:19

// Beta v0.3
// Extended compatibility to more widgets such as sliders
// 2026-06-07 22:52

// Beta v0.3.2
// Added full support for drawing primitives; enabled editing for IM_COL32 color parameters;
// Fixed crashes triggered by editing widget IDs or input box content
// 2026-06-08 22:37

// Beta v0.4
// Compatible with nearly all core widgets from ImGui 1.9.2; added texture Sfaui_image conversion utility
// 2026-06-09 11:50

// Beta v0.5
// Implemented essential multi-selection operations: invert selection, multi-select, select all, clear selection, copy & delete widgets;
// Refactored editor floating panels with collapsible/expandable layout
// 2026-06-09 22:33

// Beta v0.6
// Added configurable ImGui enumeration options in the settings panel; full customization support for core color and style widgets
// 2026-06-10 13:53

// Beta v0.7
// Rewrote copy logic as an independent operation (removed automatic interpolation); added paste and interpolation tools;
// Mitigated crashes caused by widget parameter modification
// 2026-06-13 17:08

// Beta v0.8
// Implemented widget visibility toggle; undo & redo history stack; improved stability of pointer types in drawing templates
// to make parameter editing safer; fully refactored project template indexing system.
// Developers only need to import the single header "Sfaui.h" to integrate this drafting editor into their own projects.
// Most importantly, code export functionality is newly added to generate ready-to-use ImGui source code.
// Note: Minor manual adjustments and fixes are still required for exported code before integration into external projects.
// 2026-06-20 21:12

// Beta v0.9
// Added drag-and-drop visual editor for ImVec2 values; introduced the project's flagship feature: project save & load system
// 2026-07-04 21:23

//v1.0 Optimized various scenarios for developers using the editor, officially released Sfaui editor v1.0
//2026-7-20

//图片
//Sfaui_image

#include "Sfaui_image/deletey.h"
#include "Sfaui_image/copyy.h"
#include "Sfaui_image/allwhitey.h"
#include "Sfaui_image/inveri.h"
#include "Sfaui_image/offwhite.h"
#include "Sfaui_image/setim.h"
#include "Sfaui_image/puckup.h"
#include "Sfaui_image/content_duplicate_white.h"
#include "Sfaui_image/ic_dedo.h"
#include "Sfaui_image/ic_undo.h"
#include "Sfaui_image/remove_eye_white.h"
#include "Sfaui_image/menu_white.h"

//简单使用 : 
//在example文件夹中， "launch_sdl2/3.cpp"  是Sfaui editor专门为安卓平台搞的一个启动实例，为了大大还原在安卓应用上
//的体验手感，渲染方式为(SDL2/3+OpenGL)，可以直接在launch.cpp一键运行项目来体验编辑器

//虽然说起来是 "SkyC for andriod imgui" 但是开发者可以也可以在PC端上运行编辑器开发者想在自己的项目开发中使用Sfaui editor，
//仅仅使用下面的三个函数就可以把Sfaui editor带入到开发项目中，
//首先，你需要在imgui初始化后，在主循环前添加Sfaui_init()，参数是保存文件的存放路径，可以填和不填，不填默认创建新
//项目，然后再在主循环里添加Sfaui_editing()函数从而显示编辑器,对于各个的设备分辨率不同，编辑器在有些设备可能会看上去
//比较拥挤,最后在循环后，加上Sfaui_shutdown()函数，从而和imgui一起销毁

//注意!注意!，在Sfaui_editor的文件夹里，也有一套imgui文件，在这个imgui文件对 imgui_impl_opengl3.cpp 进行了一行代码添加，目的为了适配
//安卓平台,如果你是想在安卓平台而且用SDL2和OpenGLes2.0渲染方式上运行该项目,那么这里imgui文件里的imgui_impl_opengl3.cpp是必要套件,遗憾的是可能要脱离imgui对该文件的更新
//但除了这个文件以外，其他imgui文件都可以跟随imgui官方的更新
//当然，imgui_impl_opengl3.cpp也不是不可以跟谁imgui官方的更新，因为这里imgui_impl_opengl3.cpp只加了一行代码，加了  #define IMGUI_IMPL_OPENGL_ES2 1
//如果你在更新这个imgui_impl_opengl3.cpp文件后，你只需要在文件最前头照仿Sfaui editor直接加上#define IMGUI_IMPL_OPENGL_ES2 1即可
//还有，如果你在安卓开发不用SDL2+OpenGLes2.0，而是用官方实例，用#include "imgui_impl_android.h"，#include "imgui_impl_opengl3.h"，这样的话，你完全不需要下载这个Sfaui editor
//改造过的imgui，直接跟imgui官方更新就行
//还有PC平台，也是完全不用下载sfaui eidotr改造过的imgui文件，下载sfaui ediotr必要的文件和使用即可


// Basic Usage:
// Inside the example folder, "launch_sdl2/3.cpp" is a startup sample exclusively built for Android by the Sfaui Editor. It closely replicates the touch and operation experience of native Android applications,
// with a rendering backend of (SDL2/3 + OpenGL). You can run the project directly via launch.cpp to test the editor.

// Though named "SkyC for Android ImGui", developers are also able to launch the editor on PC.
// If you want to integrate the Sfaui Editor into your own project, you only need the three functions listed below:
// First, call Sfaui_init() after ImGui initialization and before entering the main loop. Its parameter specifies the storage path for project files and is optional; leaving it blank will create a new project by default.
// Next, insert Sfaui_editing() inside the main loop to render the editor interface. Due to varying device resolutions, the editor UI may appear cramped on certain devices.
// Finally, add Sfaui_shutdown() after exiting the main loop to clean up resources alongside ImGui.

// Important Notes!
// The Sfaui_editor folder contains a separate set of ImGui source files. One line of code has been added to imgui_impl_opengl3.cpp within these files for Android compatibility.
// If you intend to run the project on Android using the SDL2 + OpenGL ES 2.0 rendering pipeline, this modified imgui_impl_opengl3.cpp is mandatory.
// The downside is you will have to stop auto-updating this single file from official ImGui releases.
// All other ImGui files can still be updated normally from the official source.

// That said, imgui_impl_opengl3.cpp can still be synced with official ImGui updates. The only modification made is adding the line: #define IMGUI_IMPL_OPENGL_ES2 1
// After overwriting this file with the latest official version, simply add that define at the very top of imgui_impl_opengl3.cpp to match the Sfaui Editor's modification.

// Additional case 1: If your Android project does not use SDL2 + OpenGL ES 2.0, and instead relies on the official Android ImGui backend with
// #include "imgui_impl_android.h" and #include "imgui_impl_opengl3.h", you do NOT need the modified ImGui bundle provided by Sfaui Editor — you may use vanilla official ImGui entirely.

// Additional case 2: PC builds require none of Sfaui Editor's modified ImGui files. Simply import and use the core Sfaui Editor modules as normal.

bool Sfaui_init(std::string file ="");    
bool Sfaui_editing();
bool Sfaui_shutdown();
#endif