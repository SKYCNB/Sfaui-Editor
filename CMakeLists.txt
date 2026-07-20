# CMakeList.txt: Sfaui 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Sfaui")

# 全局C++11（你的项目统一C++11，覆盖原C++20代码）
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 收集 ImGui 全部源码（imgui文件夹下所有h/cpp）
file(GLOB IMGUI_FILES
    "${CMAKE_SOURCE_DIR}/imgui/*.h"
    "${CMAKE_SOURCE_DIR}/imgui/*.cpp"
)

file(GLOB SFAUI_FILES
    "${CMAKE_SOURCE_DIR}/Sfaui/*.h"
    "${CMAKE_SOURCE_DIR}/Sfaui/*.cpp"
    "${CMAKE_SOURCE_DIR}/Sfaui/*.hpp"
)

# 2. 主程序源码，替换你原来的 Sfaui.cpp/h 为 main.cpp
set(MAIN_FILES
    "main.cpp"
)

# 3. 整合所有源码生成可执行程序
add_executable (Sfaui 
    ${MAIN_FILES}
    ${IMGUI_FILES}
    ${SFAUI_FILES}
  )

# ========== 新增：仅给当前Sfaui程序添加UTF8编译参数（PRIVATE，不对外扩散） ==========
target_compile_options(Sfaui PRIVATE
    # MSVC Windows：/utf-8 强制源码UTF8、字面量UTF8，自动跳过BOM
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    # GCC / Clang / Android NDK：强制源码读取、程序字符串均为UTF8
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-finput-charset=UTF-8;-fexec-charset=UTF-8>
)
# ======================================================================================

# 头文件搜索路径，main里可直接 #include "imgui.h"
target_include_directories(Sfaui PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/imgui
     ${CMAKE_SOURCE_DIR}/Sfaui
)

# Windows DX11 后端依赖库链接
if (WIN32)
target_link_libraries(Sfaui PRIVATE
    d3d11
    d3dcompiler
    dxgi
    dxguid
    kernel32
    user32
    gdi32
)
endif()

# 编译输出到根目录 out 文件夹
set_target_properties(Sfaui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out"
)