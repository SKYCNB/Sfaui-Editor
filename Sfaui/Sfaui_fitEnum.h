#ifndef SFAUI_FITENUM_H
#define SFAUI_FITENUM_H
#include <typeindex>

//#include "any.hpp"
//#include "Sfaui_value.h"
#include "Sfaui_fit.h"


using namespace nonstd;

//用于检测imgui枚举，在生成代码和Setting需要用到
//适配在↓

enum FlagBit
{
    BIT_WINFLAG        = 1 << 0,
    BIT_TREEFLAG       = 1 << 1,
    BIT_COLFLAG        = 1 << 2,
    BIT_STYLEVAR_VEC2  = 1 << 3,
    BIT_STYLEVAR_FLOAT = 1 << 4,
    BIT_IMGUICOND      = 1 << 5,
    BIT_INPUTTEXTFLAG  = 1 << 6,
    BIT_DRAGFLAG       = 1 << 7,
    BIT_DRAWFLAG       = 1 << 8,
};

struct MatchRule
{
    std::type_index type;
    std::initializer_list<int> enums;
    FlagBit targetBit;
    bool checkLastParam;
};

extern const MatchRule g_MatchRules[];
extern const int g_RuleCount;

int CalcFlagMask(std::type_index objType, int enumVal, int j, int paramTotal);
struct FlagOption
 {
     int bit;
     const char* label;
 };
 // 窗口标记 ImGuiWindowFlags
 extern const FlagOption g_WindowFlagList[];
 extern const size_t g_WindowFlagCount;
 // 树形标记 ImGuiTreeNodeFlags
 extern const FlagOption g_TreeFlagList[];
 extern const size_t g_TreeFlagCount;
 // 样式颜色 ImGuiCol
 extern const FlagOption g_ImGuiColList[];
 extern const size_t g_ImGuiColCount;
 // 浮点型 StyleVar ImGuiStyleVar
 extern const FlagOption g_StyleVarFloatList[];
 extern const size_t g_StyleVarFloatCount;
 // ImVec2 型 StyleVar ImGuiStyleVar
 extern const FlagOption g_StyleVarVec2List[];
 extern const size_t g_StyleVarVec2Count;
 // 窗口条件 ImGuiCond
 extern const FlagOption g_ImGuiCondList[];
 extern const size_t g_ImGuiCondCount;
 // 输入框标记 ImGuiInputTextFlags
 extern const FlagOption g_InputTextFlagList[];
 extern const size_t g_InputTextFlagCount;
 // 拖拽标记 ImGuiDragFlags
 extern const FlagOption g_DragFlagList[];
 extern const size_t g_DragFlagCount;
 // 绘制圆角标记 ImDrawFlags
 extern const FlagOption g_ImDrawFlagList[];
 extern const size_t g_ImDrawFlagCount;

#endif