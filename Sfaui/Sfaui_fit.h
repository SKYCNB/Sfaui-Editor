#ifndef SFAUI_FIT_H
#define SFAUI_FIT_H

//#include "any.hpp"
#include "Sfaui_value.h"

//#include "Sfaui_fitType.h"
#define IM_STR_BUF 256

struct CtrlCategory
{
    std::string name;
    std::vector<std::string> list;
};
extern std::map<std::string, std::vector<nonstd::any>> ViewList_tRNA;
extern std::map<std::string, std::vector<nonstd::any>> LaoutList_tRNA;
extern std::vector<CtrlCategory> PrintLaoutList;
extern std::vector<CtrlCategory> PrintViewList;


int DrawNewUi();


enum LView
{
    //========== 文本类 ==========
    Text,
    TextWrapped,
    LabelText,
    BulletText,
    Bullet,
    TextDisabled,

    //========== 按钮类 ==========
    Button,
    SmallButton,
    InvisibleButton,
    ArrowButton,
    CheckboxFlags,
    RadioButtonEx,

    //========== 输入框类 ==========
    InputText,
    InputTextMultiline,
    InputInt,
    InputFloat,
    InputFloat3,
    InputFloat4,
    InputInt2,
    InputInt3,
    InputInt4,
    InputDouble,

    //========== 选择勾选类 ==========
    Checkbox,
    RadioButton,
    Combo,
    ComboPreviewValue,
    ListBox,
    ListBoxHeader,

    //========== 滑块进度类 ==========
    SliderInt,
    SliderFloat,
    SliderInt2,
    SliderInt3,
    SliderInt4,
    SliderFloat2,
    SliderFloat3,
    SliderFloat4,
    VSliderInt,
    VSliderFloat,
    ProgressBar,

    //========== 树形列表类 ==========
    TreeNode,
    TreeNodeEx,
    CollapsingHeader,
    CollapsingHeaderEx,

    //========== 颜色图形类 ==========
    ColorEdit4,
    ColorEdit3,
    ColorPicker4,
    ColorPicker3,
    PlotLines,
    PlotHistogram,

    //========== 弹窗提示类 ==========
    Tooltip,
    SetTooltip,
    OpenPopup,
    CloseCurrentPopup,
    PopupContextItem,
    PopupContextWindow,

    //========== 菜单工具栏类 ==========
    Menu,
    MenuItem,
    MenuItemEx,

    //========== 拖拽调节类 ==========
    DragInt,
    DragInt2,
    DragInt3,
    DragInt4,
    DragFloat,
    DragFloat2,
    DragFloat3,
    DragFloat4,
    DragDouble,
};

extern LView FINALLYVIEW;
// 布局&绘制&窗口枚举（全局/布局/绘制接口）
enum LLayout
{
    //========== 内部布局容器类 ==========
    BeginChild,
    BeginGroup,
    Columns,
    ColumnsEnd,
    NextColumn,
    Separator,
    SeparatorText,
    NewLine,
    SameLine,
    Spacing,
    Dummy,

    //========== 窗口Begin创建系列 ==========
    Begin,
    BeginModal,
    BeginPopup,
    BeginPopupModal,
    BeginTooltip,
    BeginMainMenuBar,
    BeginMenuBar,
    BeginMenu,

    //========== 矩形绘制接口 ==========
    AddRect,
    AddRectFilled,
    AddRectFilledMultiColor,
    AddRectShadow,

    //========== 圆形椭圆绘制接口 ==========
    AddCircle,
    AddCircleFilled,
    AddEllipse,
    AddEllipseFilled,
    //========== 三角形多边形绘制接口 ==========
    AddTriangle,
    AddTriangleFilled,
    AddQuad,
    AddQuadFilled,
    AddPolyline,
    AddConvexPolyFilled,
    AddPolygon,
    AddPolygonFilled,

    //========== 线条曲线绘制接口 ==========
    AddLine,
    AddBezierCurve,
    AddBezierCurveQuadratic,
    AddPathLineTo,
    AddPathArcTo,
    AddPathBezierCurveTo,
  
    //========== 顶层文字绘制接口 ==========
    AddText,
    AddTextClipped,
    AddTextWrapped,

    //========== 图片贴图绘制接口 ==========
    AddImage,
    AddImageQuad,
    AddImageRounded,

    //========== 路径绘制全套接口 ==========
    PathClear,
    PathMoveTo,
    PathArcTo,
    PathArcToFast,
    PathBezierCurveTo,
    PathEllipseTo,
    PathClosePolygon,
    PathStroke,
    PathFill,
    PathFillConvex,

    //========== 绘制列表状态控制 ==========
    PushStyleColor,
    PopStyleColor,
    PushStyleVar_ImVec2,
    PushStyleVar_Float,
    PopStyleVar,
    PushClipRect,
    PopClipRect,
    PushTextureID,
    PopTextureID,
    SetPixelPerfectMode,
    Flags,

    //========== 全局窗口定位控制 ==========
    SetNextWindowPos,
    SetNextWindowSize,
    SetNextWindowConstraints,
    SetNextWindowContentSize,
    SetNextWindowCollapsed,
   


    End,
    EndModal,
    EndPopup,
    EndPopupModal,
    EndTooltip,
    EndMainMenuBar,
    EndMenuBar,
   // EndMenu,
    EndChild,
    EndGroup,
    PathEllipticalArcTo,
    PathLineTo,
    SfauiLayer,
    SfauiLayerEnd
};

extern LLayout FINALLYLAYOUT;




#endif