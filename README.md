# Sfaui-Editor

项目简介
 
Sfaui-Editor 全称为 SkyC for andriod ImGui editor , 一款基于 Dear ImGui 开发、适配 Windows / Android 双平台的可视化UI编辑器。是一个c++11的可嵌入项目，能为imgui的ui项目快速打草稿，生成代码，也是为开发imgui的Ui项目的新手快速接触imgui的函数和功能，排版等，专门辅助使用手机编辑器的开发者在imgui的ui项目上编辑困难的短板



<p align="center">
<img src="https://github.com/user-attachments/assets/fcdb03a1-a936-458b-8f4b-b6ac1a8f0562" width="850" alt="Sfaui编辑器演示">
</p>
 
Sfaui Editor 是自研可视化拖拽UI编辑器，无需手写大量ImGui绘制代码，通过拖拽控件、实时调整参数快速生成界面数据；内置撤销/重做系统、图层管理、二进制存档读写，原生支持DX11桌面端、OpenGL ES移动端渲染。
 
核心特性
 
1. 可视化拖拽画布
内置多种ImGui原生控件：矩形图片、文本、按钮、子面板、分组、四边形贴图等，拖拽即可创建实例，支持网格吸附、图层显隐、多选操作。
2. 完整历史操作栈
商业级Undo/Redo撤回前进，新增、修改、删除、移动控件全部记录；新操作自动清空重做栈，完全符合编辑器逻辑。
3. 二进制轻量化存档
自定义二进制 .Sfaui.cofing 工程文件，存储所有控件坐标、UV、颜色、图层序号；读写速度快，体积远小于JSON，适配移动端低性能设备。
4. 跨平台兼容
- Windows：DX11渲染后端，VS2022编译
- Android：GLES2.0/3.0，NDK交叉编译
5. 控件参数统一解析架构
使用 std::any 泛型容器存储各类参数（ImVec2、ImVec4、纹理ID、数值），统一switch渲染分发，可快速拓展自定义新控件。
 
环境依赖
 
- C++17
- ImGui 1.92.6
- SDL3 / SDL2
- nonstd::any
- CMake 3.16+ / MSBuild
 
快速使用
 
1. 克隆仓库
 
bash
  
git clone https://github.com/SKYCNB/Sfaui-Editor.git
 
 
2. Windows编译：使用VS2022打开CMake工程，直接构建运行
3. Android编译：使用TinaIDE导入NDK项目，打包APK
4. 编辑器操作：拖拽左侧控件至画布，调整参数后保存二进制工程文件，在游戏/渲染引擎中加载渲染
 
文件说明
 
-  All_mRNA ：所有画布控件实例数据容器
-  LVBuild ：图层层级数组
-  WasChose ：控件选中状态标记
-  Visibility ：控件显隐控制
-  Sfaui_fit.h ：控件适配、绘制分发核心逻辑
- 二进制读写： SaveSet  / 加载解析函数
 
开发规划
 
完善批量多选编辑功能
新增渐变控件、自定义曲线UI
优化安卓端内存占用，修复大工程卡顿
增加工程版本兼容，支持新旧存档互通
 
作者
 
SKYCNB
