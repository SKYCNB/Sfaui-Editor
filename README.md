# Sfaui-Editor

项目简介
 
Sfaui-Editor 全称为 SkyC for andriod ImGui editor , 一款基于 Dear ImGui 开发、适配 Windows / Android 双平台的轻量可视化UI编辑器。是一个c++11的可嵌入项目，无需手写大量ImGui绘制代码，通过实时调整参数快速生成界面数据 ，能为imgui的ui项目快速打草稿，生成代码，也是为开发imgui的Ui项目的新手快速接触imgui的函数和功能，排版等，专门辅助使用手机编辑器的开发者在imgui的ui项目上编辑困难的短板。



<p align="center">
<img src="https://github.com/user-attachments/assets/fcdb03a1-a936-458b-8f4b-b6ac1a8f0562" width="850" alt="Sfaui编辑器演示">
</p>
 

特性和原理
编辑器获取和修改imgui的每个函数的参数数据来自动生成imgui的函数效果。
本编辑器是按照imgui排版流水的规律，以及它统一的函数名称开发而来。由于Imgui的函数参数数量和类型不统一,所以编辑器按照生物的 mRNA , tRNA 之间的翻译操作的灵感，使用c++的any库，解决了参数不统一的问题，
内置多种ImGui原生控件：矩形图片、文本、按钮、子面板、等，有多选，插入，删除等操作。

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
