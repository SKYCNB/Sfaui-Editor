# Sfaui-Editor

# 项目简介
 
Sfaui-Editor 全称为 SkyC for andriod ImGui editor , 一款基于 Dear ImGui 开发、适配 Windows / Android 双平台的轻量可视化UI编辑器。是一个c++11的可嵌入项目，无需手写大量ImGui绘制代码，通过实时调整参数快速生成界面数据 ，内置多种ImGui原生控件：矩形图片、文本、按钮、子面板、等，有多选，插入，删除等操作。
能为imgui的ui项目快速打草稿，生成代码，也是为开发imgui的Ui项目的新手快速接触imgui的函数和功能，排版等，专门辅助使用手机编辑器的开发者在imgui的ui项目上编辑困难的短板。



<p align="center">
<img src="https://github.com/user-attachments/assets/fcdb03a1-a936-458b-8f4b-b6ac1a8f0562" width="850" alt="Sfaui编辑器演示">
</p>
 

# 特性和原理
编辑器获取和修改imgui的每个函数的参数数据来自动生成imgui的函数效果。
本编辑器是按照imgui排版流水的规律，以及它统一的函数名称开发而来。由于Imgui的函数参数数量和类型不统一,所以编辑器按照生物的 mRNA , tRNA 之间的翻译操作的灵感，使用c++的any库，解决了参数不统一的问题
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

用any把参数类型抹除，就能添加进函数里，any也能检测类型，回调给编辑器的参数修改，大大减少代码量。

<p align="center">
<img src="https://github.com/user-attachments/assets/016ce36f-ebe0-4e87-9fd0-30a13c0b27f1" width="850" alt="Sfaui编辑器演示2">
</p>

<span style="color:red;">缺点：1.由于any的特性，不能检测指针类型，和如果返回类型与函数对应的类型不匹配，会“基因突变”，（造成闪退，崩溃等问题）
                               2.编辑器生成的布局规则和imgui的布局规则是一样的，如果在编辑器中搞一个错误的布局顺序会导致imgui弹出错误提示，比如BeginMainMenuBar里创建BeginMainMenuBar等不规范排版，在安卓上有些编辑器没有debug更是直接闪退，所以开发者也许要时刻记得保存项目避免突然闪退和崩溃带来的损失
如果开发者在内嵌编辑器后闪退或程序崩溃，影响和危害个人项目，请尽快把编辑器相关的东西删去，这里编辑器作者为各位开发者感到非常的抱歉</span>

# 环境依赖和使用

- C++11
编辑器是可嵌入项目，凡是能运行imgui的地方，都可以使用Sfaui-ed，编辑器只公开了三个函数接口，开发者只需调用Sfaui.h,和三个函数即可
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

# 仓库文件说明
如果开发者们的项目中能运行imgui，直接下载Sfaui文件整合包和cmake即可，不需要下载这里的imgui文件，最后链接Sfaui库即可，如需要dx11或opengles渲染方式使用Sfaui的实例，请进入 example 文件夹寻找对应实例

如果开发者在安卓平台上使用Sdl - opengles 的渲染方式，且imgui不能显示画面，可以下载这里imgui文件夹里的 imgui_impl_opengl.h/.cpp文件

