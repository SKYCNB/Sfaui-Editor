#include "Sfaui_fitEnum.h"

const MatchRule g_MatchRules[] =
{
    {
        typeid(LLayout),
        {(int)LLayout::Begin, (int)LLayout::BeginChild},
        BIT_WINFLAG,
        false
    },
    {
        typeid(LLayout),
        {(int)LLayout::PushStyleColor},
        BIT_COLFLAG,
        false
    },
    {
        typeid(LLayout),
        {(int)LLayout::PushStyleVar_ImVec2},
        BIT_STYLEVAR_VEC2,
        false
    },
    {
        typeid(LLayout),
        {(int)LLayout::PushStyleVar_Float},
        BIT_STYLEVAR_FLOAT,
        false
    },
    {
        typeid(LLayout),
        {(int)LLayout::SetNextWindowPos, (int)LLayout::SetNextWindowSize},
        BIT_IMGUICOND,
        false
    },
    {
        typeid(LView),
        {(int)LView::TreeNodeEx, (int)LView::CollapsingHeader},
        BIT_TREEFLAG,
        false
    },
    {
        typeid(LView),
        {
            (int)LView::InputInt, (int)LView::InputFloat, (int)LView::InputFloat3,
            (int)LView::InputFloat4, (int)LView::InputInt2, (int)LView::InputInt3,
            (int)LView::InputInt4, (int)LView::InputDouble
        },
        BIT_INPUTTEXTFLAG,
        true
    },
    {
        typeid(LView),
        {
            (int)LView::DragInt, (int)LView::DragInt2, (int)LView::DragInt3,
            (int)LView::DragInt4, (int)LView::DragFloat, (int)LView::DragFloat2,
            (int)LView::DragFloat3, (int)LView::DragFloat4, (int)LView::DragDouble
        },
        BIT_DRAGFLAG,
        true
    },
    {
      typeid(LLayout),
      {
          (int)LLayout::AddRect
      },
      BIT_DRAWFLAG,
      false
  }
};
const int g_RuleCount = sizeof(g_MatchRules) / sizeof(MatchRule);

int CalcFlagMask(std::type_index objType, int enumVal, int j, int paramTotal)
{
    int mask = 0;
    for (int i = 0; i < g_RuleCount; i++)
    {
        const auto& r = g_MatchRules[i];
        if (r.type != objType) continue;

        bool enumMatch = false;
        for (int e : r.enums)
        {
            if (e == enumVal)
            {
                enumMatch = true;
                break;
            }
        }
        if (!enumMatch) continue;

        if (r.checkLastParam && j != paramTotal-1)
            continue;

        mask |= r.targetBit;
    }
    return mask;
}


const FlagOption g_WindowFlagList[] = {
    { ImGuiWindowFlags_NoTitleBar,         "NoTitleBar 无标题栏" },
    { ImGuiWindowFlags_NoResize,           "NoResize 禁止缩放" },
    { ImGuiWindowFlags_NoMove,             "NoMove 禁止拖动" },
    { ImGuiWindowFlags_NoScrollbar,        "NoScrollbar 隐藏滚动条" },
    { ImGuiWindowFlags_NoScrollWithMouse,  "NoScrollWithMouse 滚轮不滚动" },
    { ImGuiWindowFlags_NoCollapse,         "NoCollapse 禁用折叠" },
    { ImGuiWindowFlags_NoBackground,       "NoBackground 无背景" },
    { ImGuiWindowFlags_NoSavedSettings,    "NoSavedSettings 不保存位置大小" },
};
const size_t g_WindowFlagCount = IM_ARRAYSIZE(g_WindowFlagList);

const FlagOption g_TreeFlagList[] = {
    { ImGuiTreeNodeFlags_OpenOnArrow,      "OpenOnArrow 仅点箭头展开" },
    { ImGuiTreeNodeFlags_OpenOnDoubleClick,"OpenOnDoubleClick 双击展开" },
    { ImGuiTreeNodeFlags_DefaultOpen,      "DefaultOpen 默认展开" },
    { ImGuiTreeNodeFlags_Leaf,             "Leaf 叶子节点(无箭头)" },
    { ImGuiTreeNodeFlags_Selected,         "Selected 高亮选中" },
    { ImGuiTreeNodeFlags_SpanFullWidth,    "SpanFullWidth 占满整行宽度" },
};
const size_t g_TreeFlagCount = IM_ARRAYSIZE(g_TreeFlagList);

const FlagOption g_ImGuiColList[] = {
    { ImGuiCol_WindowBg,               "WindowBg 普通窗口背景" },
    { ImGuiCol_ChildBg,                "ChildBg 子窗口背景" },
    { ImGuiCol_PopupBg,                "PopupBg 弹窗/菜单/提示框背景" },
    { ImGuiCol_Text,                   "Text 普通文本" },
    { ImGuiCol_TextDisabled,           "TextDisabled 禁用文本" },
    { ImGuiCol_FrameBg,                "FrameBg 输入框/复选框/滑块 控件底色" },
    { ImGuiCol_FrameBgHovered,         "FrameBgHovered 控件底色(悬浮)" },
    { ImGuiCol_FrameBgActive,          "FrameBgActive 控件底色(激活/点击)" },
    { ImGuiCol_Button,                 "Button 按钮底色" },
    { ImGuiCol_ButtonHovered,          "ButtonHovered 按钮底色(悬浮)" },
    { ImGuiCol_ButtonActive,           "ButtonActive 按钮底色(按下)" },
    { ImGuiCol_CheckMark,              "CheckMark 勾选标记/单选圆点" },
    { ImGuiCol_SliderGrab,             "SliderGrab 滑块把手" },
    { ImGuiCol_SliderGrabActive,       "SliderGrabActive 滑块把手(按下)" },
    { ImGuiCol_Border,                 "Border 通用边框" },
    { ImGuiCol_BorderShadow,           "BorderShadow 边框阴影" },
    { ImGuiCol_Separator,              "Separator 分隔线" },
    { ImGuiCol_SeparatorHovered,       "SeparatorHovered 分隔线(悬浮)" },
    { ImGuiCol_SeparatorActive,        "SeparatorActive 分隔线(点击)" },
    { ImGuiCol_TitleBg,                "TitleBg 标题栏(默认)" },
    { ImGuiCol_TitleBgActive,          "TitleBgActive 标题栏(激活)" },
    { ImGuiCol_TitleBgCollapsed,       "TitleBgCollapsed 标题栏(折叠)" },
    { ImGuiCol_Header,                 "Header 折叠面板/树节点/菜单项(默认)" },
    { ImGuiCol_HeaderHovered,          "HeaderHovered 折叠面板(悬浮)" },
    { ImGuiCol_HeaderActive,           "HeaderActive 折叠面板(按下)" },
    { ImGuiCol_TextSelectedBg,         "TextSelectedBg 输入框选中文本背景" },
    { ImGuiCol_ScrollbarBg,            "ScrollbarBg 滚动条背景" },
    { ImGuiCol_ScrollbarGrab,          "ScrollbarGrab 滚动条滑块(默认)" },
    { ImGuiCol_ScrollbarGrabHovered,    "ScrollbarGrabHovered 滚动条滑块(悬浮)" },
    { ImGuiCol_ScrollbarGrabActive,    "ScrollbarGrabActive 滚动条滑块(按下)" },
    { ImGuiCol_ResizeGrip,             "ResizeGrip 窗口缩放手柄(默认)" },
    { ImGuiCol_ResizeGripHovered,      "ResizeGripHovered 窗口缩放手柄(悬浮)" },
    { ImGuiCol_ResizeGripActive,       "ResizeGripActive 窗口缩放手柄(按下)" },
    { ImGuiCol_InputTextCursor,        "InputTextCursor 输入框光标" },
    { ImGuiCol_PlotLines,              "PlotLines 折线图线条" },
    { ImGuiCol_PlotLinesHovered,       "PlotLinesHovered 折线图线条(悬浮)" },
    { ImGuiCol_PlotHistogram,          "PlotHistogram 直方图柱体" },
    { ImGuiCol_PlotHistogramHovered,   "PlotHistogramHovered 直方图柱体(悬浮)" },
    { ImGuiCol_DragDropTarget,         "DragDropTarget 拖拽目标边框高亮" },
    { ImGuiCol_DragDropTargetBg,       "DragDropTargetBg 拖拽目标背景高亮" },
};
const size_t g_ImGuiColCount = IM_ARRAYSIZE(g_ImGuiColList);


// float 单值样式变量（全部补齐 + 高频靠前）
const FlagOption g_StyleVarFloatList[] = {
    { ImGuiStyleVar_Alpha,               "Alpha 全局透明度 float" },
    { ImGuiStyleVar_DisabledAlpha,       "DisabledAlpha 禁用透明度 float" },
    { ImGuiStyleVar_WindowRounding,      "WindowRounding 窗口圆角 float" },
    { ImGuiStyleVar_WindowBorderSize,    "WindowBorderSize 窗口边框 float" },
    { ImGuiStyleVar_ChildRounding,       "ChildRounding 子窗口圆角 float" },
    { ImGuiStyleVar_ChildBorderSize,     "ChildBorderSize 子窗口边框 float" },
    { ImGuiStyleVar_PopupRounding,       "PopupRounding 弹窗圆角 float" },
    { ImGuiStyleVar_PopupBorderSize,     "PopupBorderSize 弹窗边框 float" },
    { ImGuiStyleVar_FrameRounding,       "FrameRounding 控件圆角 float" },
    { ImGuiStyleVar_FrameBorderSize,     "FrameBorderSize 控件边框宽度 float" },
    { ImGuiStyleVar_IndentSpacing,       "IndentSpacing 缩进间距 float" },
    { ImGuiStyleVar_ScrollbarSize,       "ScrollbarSize 滚动条宽度 float" },
    { ImGuiStyleVar_ScrollbarRounding,   "ScrollbarRounding 滚动条圆角 float" },
    { ImGuiStyleVar_ScrollbarPadding,    "ScrollbarPadding 滚动条内边距 float" },
    { ImGuiStyleVar_GrabMinSize,         "GrabMinSize 滑块最小尺寸 float" },
    { ImGuiStyleVar_GrabRounding,        "GrabRounding 滑块圆角 float" },
    { ImGuiStyleVar_ImageRounding,       "ImageRounding 图片圆角 float" },
    { ImGuiStyleVar_ImageBorderSize,     "ImageBorderSize 图片边框宽度 float" },
    { ImGuiStyleVar_TreeLinesSize,       "TreeLinesSize 树连线粗细 float" },
    { ImGuiStyleVar_TreeLinesRounding,   "TreeLinesRounding 树连线圆角 float" },
    { ImGuiStyleVar_SeparatorTextBorderSize, "SeparatorTextBorderSize 分隔文字边框 float" },
};
const size_t g_StyleVarFloatCount = IM_ARRAYSIZE(g_StyleVarFloatList);

// ImVec2 向量样式变量（全部补齐 + 高频靠前）
const FlagOption g_StyleVarVec2List[] = {
    { ImGuiStyleVar_WindowPadding,       "WindowPadding 窗口内边距 ImVec2" },
    { ImGuiStyleVar_WindowMinSize,       "WindowMinSize 窗口最小尺寸 ImVec2" },
    { ImGuiStyleVar_WindowTitleAlign,     "WindowTitleAlign 窗口标题对齐 ImVec2" },
    { ImGuiStyleVar_FramePadding,        "FramePadding 控件内边距 ImVec2" },
    { ImGuiStyleVar_ItemSpacing,         "ItemSpacing 控件间距 ImVec2" },
    { ImGuiStyleVar_ItemInnerSpacing,    "ItemInnerSpacing 控件内部间距 ImVec2" },
    { ImGuiStyleVar_ButtonTextAlign,     "ButtonTextAlign 按钮文字对齐 ImVec2" },
    { ImGuiStyleVar_SelectableTextAlign,  "SelectableTextAlign 选择项文字对齐 ImVec2" },
    { ImGuiStyleVar_SeparatorTextAlign,   "SeparatorTextAlign 分隔文字对齐 ImVec2" },
    { ImGuiStyleVar_SeparatorTextPadding,"SeparatorTextPadding 分隔文字内边距 ImVec2" },
};
const size_t g_StyleVarVec2Count = IM_ARRAYSIZE(g_StyleVarVec2List);

// ImGuiCond 窗口条件枚举
const FlagOption g_ImGuiCondList[] = {
    { ImGuiCond_None,          "Cond_None 始终生效" },
    { ImGuiCond_Once,          "Cond_Once 仅本次生效" },
    { ImGuiCond_FirstUseEver,  "Cond_FirstUseEver 首次打开生效" },
    { ImGuiCond_Appearing,     "Cond_Appearing 显示时生效" },
};
const size_t g_ImGuiCondCount = IM_ARRAYSIZE(g_ImGuiCondList);

// 输入框标志位 ImGuiInputTextFlags
const FlagOption g_InputTextFlagList[] = {
    { ImGuiInputTextFlags_ReadOnly,        "ReadOnly 只读" },
    { ImGuiInputTextFlags_Password,        "Password 密码隐藏" },
    { ImGuiInputTextFlags_CharsDecimal,    "CharsDecimal 仅小数" },
    { ImGuiInputTextFlags_CharsHexadecimal,"CharsHexadecimal 仅十六进制" },
    { ImGuiInputTextFlags_CharsUppercase,  "CharsUppercase 自动大写" },
    { ImGuiInputTextFlags_NoHorizontalScroll,"NoHorizontalScroll 禁止横向滚动" },
    { ImGuiInputTextFlags_AllowTabInput,   "AllowTabInput 允许Tab" },
};
const size_t g_InputTextFlagCount = IM_ARRAYSIZE(g_InputTextFlagList);

// 拖拽控件标志位 ImGuiDragFlags

const FlagOption g_DragFlagList[] = {
    { 1 << 0,  "NoHorizontal 禁止水平拖拽" }
};
const size_t g_DragFlagCount = IM_ARRAYSIZE(g_DragFlagList);

// ImDrawFlags 绘制矩形/圆角绘制标记（AddRect、AddRectFilled专用）
const FlagOption g_ImDrawFlagList[] = {
    { ImDrawFlags_None,                "DrawFlags_None 无特殊标记" },
    { ImDrawFlags_RoundCornersTopLeft, "RoundCornersTopLeft 仅左上角圆角" },
    { ImDrawFlags_RoundCornersTopRight,"RoundCornersTopRight 仅右上角圆角" },
    { ImDrawFlags_RoundCornersAll,     "RoundCornersAll 四个角全部圆角" },
    { ImDrawFlags_Closed,              "Closed 闭合线条(线条绘制专用)" },
};
const size_t g_ImDrawFlagCount = IM_ARRAYSIZE(g_ImDrawFlagList);
