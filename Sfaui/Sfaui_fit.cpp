//在本文件中，是关于适配控件的变量，类型等功能函数的一个集合,如果开发者不跟随Sfaui editor
//关于这个文件的更新，那么，开发者也可以跟着以下教程去适配新控件
#include "Sfaui_fit.h"
#include <array>

using namespace nonstd;

/*
Sfaui Editor 新增ImGui控件适配

前置说明

1. 整体架构介绍

1. 模板库（静态固定）
 ViewList_tRNA  /  LaoutList_tRNA ：所有控件的模板元数据，保存控件枚举、配对关系、默认位置大小、函数名、参数列表、默认参数值。
 PrintViewList  /  PrintLaoutList ：编辑器左侧控件分类面板，控制展示哪些控件。
2. 运行实例库
 All_mRNA ：画布上拖拽出来的控件实例，保存每个控件独立参数，二进制保存/加载核心数据。
3. 渲染入口
 DrawNewUi() ：遍历 All_mRNA ，根据控件枚举匹配switch分支，调用原生ImGui接口实时预览画布。
4. 配套文件
 Sfaui_fit.h ：存放控件枚举  enum LView 、 enum LLayout 、类型定义、宏、掩码标识。
5. 专用工具函数
 ptr<T>() ：生成静态缓存指针，用于ImGui输入框/滑块等需要外部指针的参数，直接传 nullptr ，可能否则加载/渲染闪退。

2. 全局固定下标宏（模板vector固定结构）

cpp

#define VLENUM     0    // 控件枚举 LLayout / LView
#define PAIR       1    // 配对控件名称（Begin对应End、Push对应Pop，无配对填自身名称）
#define POS        2    // 编辑器画布默认位置 ImVec2
#define RANGE      3    // 编辑器画布默认尺寸 ImVec2
#define REALID     4    // 控件唯一标识字符串，不能重复
#define PARAMNUM   5    // ImGui原生函数总参数个数
#define FUNCTIONNMAE 6  // 生成代码对应的ImGui函数字符串
#define PARAMST    7    // 函数第一个参数起始下标，7、8、9...依次对应所有入参
 

3. 强制规则

1. 模板内每一项数据必须包裹 any(xxx) ，否则 nonstd::any 类型识别失败，加载工程闪退；
2. 成对控件（Begin/End、PushStyleColor/PopStyleColor）两端都要完整适配，配对名称互相填写；
3. 所有指针类型参数统一使用 ptr<T>() 生成静态缓存，禁止 any((Type*)nullptr) ；
4.  DrawNewUi 内switch分支参数顺序、类型必须和模板 PARAMST 往后的参数一一对应，错位直接崩溃；
5. 枚举名必须和ImGui函数名完全一致，大小写、单词不能差。

 

完整四步适配流程（新增控件通用模板）

步骤1：在左侧面板列表添加控件显示名

文件位置

 Sfaui_fit.cpp  底部  PrintViewList （普通UI控件） /  PrintLaoutList （窗口/绘制/布局控件）

操作

找到对应分类，在花括号内新增一行字符串，字符串为后续模板第一级key名称。
示例：新增普通按钮

cpp

{
    "按钮控件 / Buttons",
    {
        "Button 普通按钮",
        "SmallButton 小按钮",
        "XXX 我的新控件" // 新增这一行
    }
}
 

步骤2：头文件添加控件枚举

文件位置

 Sfaui_fit.h 

操作

1. 普通交互控件添加至  enum LView ；布局/绘制/窗口控件添加至  enum LLayout ，
且枚举标识符严格等于ImGui函数名（特别要注意，一定要在enum LLayout 或 enum LView 
末尾添加，不能插在中间，否者加载旧项目会异常闪退！！！）
3 .找到下面的
LView FINALLYVIEW = DragDouble;
LLayout FINALLYLAYOUT = PathEllipticalArcTo;
枚举变量，把值替换成对应的最末尾的枚举名（这一步不弄会导致闪退和异常）
示例：

cpp

enum LView
{
    Text,
    Button,
    InputText,
    XXX, // 新增，和ImGui::XXX完全同名
};
 

步骤3：填充控件模板元数据（核心步骤）

文件位置

 Sfaui_fit.cpp   ViewList_tRNA  /  LaoutList_tRNA 

模板固定结构拆解（逐字段解释）

cpp

{
    // 第一层key：PrintViewList/PrintLaoutList内填写的显示名称
    "XXX 我的新控件",
    {
        // 下标0 VLENUM：绑定步骤2新增的枚举
        any(LView::XXX),
        // 下标1 PAIR：配对控件名称，无配对填自身，成对控件填对应End/Pop名称
        any(std::string("XXX 我的新控件")),
        // 下标2 POS：画布拖拽默认坐标 ImVec2(x,y)
        any(ImVec2(0,0)),
        // 下标3 RANGE：画布控件默认宽高 ImVec2(w,h)
        any(ImVec2(160,24)),
        // 下标4 REALID：全局唯一标识，英文数字下划线，不可重复
        any(std::string("xxx_001")),
        // 下标5 PARAMNUM：ImGui::XXX函数总入参数量，数清楚再填
        any((int)2),
        // 下标6 FUNCTIONNMAE：代码生成用的完整函数字符串
        any(std::string("ImGui::XXX")),
        // 下标7 PARAMST 开始：依次填写函数每个入参默认值，类型和ImGui参数严格匹配
        any(std::string("控件标题")),
        any(ImVec2(100,20))
    }
},
 

特殊场景1：成对控件示例（Begin + End）

Begin模板配对字段填End名称，End模板配对填自身，两个都要写完整模板

cpp

{
    "Begin 普通窗口",
    {
        any(LLayout::Begin),
        any(std::string("End")), // 配对指向End
        any(ImVec2(0,0)),
        any(ImVec2(200,200)),
        any(std::string("win_001")),
        any((int)3),
        any(std::string("ImGui::Begin")),
        any(std::string("窗口标题")),
        any((bool*)nullptr),
        any((ImGuiWindowFlags)0)
    }
},
{
    "End",
    {
        any(LLayout::End),
        any(std::string("End")), // 自身配对
        any(ImVec2(0,0)),
        any(ImVec2(0,0)),
        any(std::string("end_001")),
        any((int)0), // End无参数
        any(std::string("ImGui::End"))
    }
},
 

特殊场景2：指针参数（InputInt/SliderFloat等）

使用 ptr 模板生成静态缓存指针，示例：

cpp

any(ptr([]()->int*{static int buf{}; return &buf;}()))
 

步骤4：在DrawNewUi()补充实时渲染分支

文件位置

 Sfaui_fit.cpp   DrawNewUi  函数内switch

操作

1. 判断枚举类型： LLayout 控件走外层 typeid(LLayout) 分支， LView 走 typeid(LView) ；
2. 添加 case 你的枚举: ；
3. 调用ImGui原生函数，参数从 arr[PARAMST]、arr[PARAMST+1]... 依次取出，使用 any_cast<类型> 强转；
4. 无参数控件直接调用空函数；
示例：

cpp

case XXX:
    ImGui::XXX(
        any_cast<std::string>(arr[PARAMST]).c_str(),
        any_cast<ImVec2>(arr[PARAMST+1])
    );
    break;
//这里有一个小提示：如果开发者想再配套控件比如Begin和End这种，嵌入绘制函数且能在控件里看到绘制效果，请在这里加一句：
 实例
       case Begin:
                ImGui::Begin(any_cast<std::string>(arr[PARAMST]).c_str(),
                    any_cast<bool*>(arr[PARAMST + 1]),
                    any_cast<ImGuiWindowFlags>(arr[PARAMST + 2]));
                draw = ImGui::GetWindowDrawList(); // 在这里添加，为了获取List

                break;
            case End:
                ImGui::End();
                break;

 

配套拓展知识点（适配Flags/掩码控件）

1. 掩码（遮掩码）作用

ImGui大量使用uint32存储多组开关（窗口标记、树节点标记、输入框标记），每一个bit代表一个功能开关，称为掩码。

2. 掩码配套数据结构

1.  FlagOption ：单条掩码定义，存储掩码值+前端显示名称；
2.  g_XXXFlagList ：掩码下拉选择列表，编辑器自动生成多选面板；
3.  MatchRule ：匹配规则表，定义哪种控件、第几个参数是掩码类型；
4.  CalcFlagMask ：自动计算控件支持的掩码类型，编辑器识别并渲染多选框。

3. 带掩码控件额外适配步骤

1. 在对应 g_XXXFlagList 数组补充全部可选掩码；
2. 在 g_MatchRules 新增匹配规则，绑定控件枚举、目标掩码BIT；
3. tRNA模板内对应掩码参数填默认值 any((int)0) ；
4. DrawNewUi分支正常any_cast转换对应枚举类型。

 

常见报错&排错手册

1. 画布运行可能闪退的情况:

- 模板 PARAMNUM 参数数量和实际PARAMST后参数个数不一致；
- 指针参数传 nullptr ，未使用 ptr<T>() ；
- DrawNewUi内参数顺序、类型和模板不匹配；
- 数据未包裹 any() ，类型读取失败。

2. 左侧面板看不到控件

-  PrintViewList / PrintLaoutList 未添加显示名称；
- 模板key字符串和面板内字符串大小写、文字不一致。

3. 控件拖入画布无预览

- 忘记在DrawNewUi添加case分支；
- 枚举值填写错误，VLENUM下标0匹配失败。

4. 保存工程重新加载参数丢失

- 参数类型未在 typeTable 注册， GetAnyType 返回 AnyType_Unknown ；
- 成对控件配对名称填写错误，层级数据解析错乱。

5. 代码生成函数名错误

-  FUNCTIONNMAE 下标6的函数字符串拼写错误。
*/




template<typename T>
T* ptr(T* bind = nullptr)
{
    static T cache{};
    return bind ? bind : &cache;
}


LView FINALLYVIEW = DragDouble;
LLayout FINALLYLAYOUT = SfauiLayerEnd;

std::map<std::string, std::vector<any>> ViewList_tRNA =
{
    //========== 文本类 ==========
    {
        "Text 普通文本",
        {
            any(LView::Text),
            any(std::string("Text 普通文本")),       //寻找要配对的键对值
            any(ImVec2(0,0)), //控件位置
            any(ImVec2(50,50)), //控件大小
            any(std::string("Text 普通文本")),  //reality id
            any((int)1),//参数数量
            any(std::string("ImGui::Text")), //funtionname
            any(std::string("text")) //imgui id(name)
        }
    },


    {   "TextWrapped 换行文本",
        {
            any(LView::TextWrapped),
            any(std::string("TextWrapped 换行文本")),
            any(ImVec2(0,30)),
            any(ImVec2(180,44)),
            any(std::string("txt_wrap_001")),
            any((int)1),
            any(std::string("ImGui::TextWrapped")),
            any(std::string("textwr"))
        }
    },
    {   "LabelText 标签文本",
        {
            any(LView::LabelText),
            any(std::string("LabelText 标签文本")),
            any(ImVec2(0,80)),
            any(ImVec2(220,22)),
            any(std::string("txt_label_001")),
            any((int)2),
            any(std::string("ImGui::LabelText")),
            any(std::string("Label")),
            any(std::string("lable"))
        }
    },
    {   "BulletText 项目符号",
        {
            any(LView::BulletText),
            any(std::string("BulletText 项目符号")),
            any(ImVec2(0,120)),
            any(ImVec2(150,22)),
            any(std::string("txt_bullettext_001")),
            any((int)1),
            any(std::string("ImGui::BulletText")),
            any(std::string("bullet"))
        }
    },
    {
        "Bullet 无文本项目符号",
        {
            any(LView::Bullet),
            any(std::string("Bullet 无文本项目符号")),
            any(ImVec2(0,0)),
            any(ImVec2(20,20)),
            any(std::string("bullet_001")),
            any((int)0),
            any(std::string("ImGui::Bullet"))
        }
    },
    {
        "TextDisabled 灰色禁用文本",
        {
            any(LView::TextDisabled),
            any(std::string("TextDisabled 灰色禁用文本")),
            any(ImVec2(0,30)),
            any(ImVec2(160,22)),
            any(std::string("text_dis_001")),
            any((int)1),
            any(std::string("ImGui::TextDisabled")),
            any(std::string("only read"))
        }
    },


    //按钮类
    {   "Button 普通按钮",
        {
            any(LView::Button),
            any(std::string("Button 普通按钮")),
            any(ImVec2(0,240)),
            any(ImVec2(110,26)),
            any(std::string("btn_norm_001")),
            any((int)2),
            any(std::string("ImGui::Button")),
            any(std::string("button")),
            any(ImVec2(0,0))
        }
    },
    {   "SmallButton 小按钮",
        {
            any(LView::SmallButton),
            any(std::string("SmallButton 小按钮")),
            any(ImVec2(120,240)),
            any(ImVec2(70,18)),
            any(std::string("btn_small_001")),
            any((int)1),
            any(std::string("ImGui::SmallButton")),
            any(std::string("small"))
        }
    },
    {   "InvisibleButton 透明按钮",
        {
            any(LView::InvisibleButton),
            any(std::string("InvisibleButton 透明按钮")),
            any(ImVec2(0,280)),
            any(ImVec2(150,30)),
            any(std::string("btn_invis_001")),
            any((int)2),
            any(std::string("ImGui::InvisibleButton")),
            any(std::string("inv_btn_01")),
            any(ImVec2(150,30))
        }
    },
    //复选框类
    {   "Checkbox 复选框",
        {
            any(LView::Checkbox),
            any(std::string("Checkbox 复选框")),
            any(ImVec2(0,320)),
            any(ImVec2(110,22)),
            any(std::string("chk_001")),
            any((int)2),
            any(std::string("ImGui::Checkbox")),
            any(std::string("ok")),
            any((bool)true)
        }
    },
    {   "CheckboxFlags 位域复选框",
        {
            any(LView::CheckboxFlags),
            any(std::string("CheckboxFlags 位域复选框")),
            any(ImVec2(130,320)),
            any(ImVec2(130,22)),
            any(std::string("chk_flag_001")),
            any((int)3),
            any(std::string("ImGui::CheckboxFlags")),
            any(std::string("choose")),
            any((int)1),
            any((int)1)
        }
    },
    {   "RadioButton 单选框",
        {
            any(LView::RadioButton),
            any(std::string("RadioButton 单选框")),
            any(ImVec2(110,350)),
            any(ImVec2(100,22)),
            any(std::string("radio_002")),
            any((int)3),
            any(std::string("ImGui::RadioButton")),
            any(std::string("i")),
            any((int)2),
            any((int)1)
        }
    },
    //输入框类
        {
            "InputText 单行输入",
            {
                any(LView::InputText),
                any(std::string("InputText 单行输入")),
                any(ImVec2(0,440)),
                any(ImVec2(180,24)),
                any(std::string("input_txt_001")),
                any((int)6),
                any(std::string("ImGui::InputText")),
                any(std::string("单行文本")),
                any(std::string("默认内容")),
                any((int)128),
                any((int)0),
                any((ImGuiInputTextCallback)nullptr),
                any((void*)nullptr)
            }
        },
        {
            "InputTextMultiline 多行输入",
            {
                any(LView::InputTextMultiline),
                any(std::string("InputTextMultiline 多行输入")),
                any(ImVec2(0,0)),
                any(ImVec2(240,100)),
                any(std::string("multi_input_001")),
                any((int)7),
                any(std::string("ImGui::InputTextMultiline")),
                any(std::string("多行文本")),
                any(ptr([]()->char* {static char buf[512]{""}; return buf; }())),
                any((size_t)512),
                    any(ImVec2(240,100)),
                    any((int)0),
                    any((ImGuiInputTextCallback)nullptr),
                    any((void*)nullptr)
            }
        },
        {
            "InputInt 整数输入",
            {
                any(LView::InputInt),
                any(std::string("InputInt 整数输入")),
                any(ImVec2(0,110)),
                any(ImVec2(180,24)),
                any(std::string("input_int_001")),
                any((int)5),
                any(std::string("ImGui::InputInt")),
                any(std::string("整数")),
                any(ptr([]()->int* {static int buf[4]{}; return buf; }())),
                any((int)1),
                    any((int)100),
                    any((ImGuiInputTextFlags)0)
            }
        },
        {
            "InputFloat 浮点输入",
            {
                any(LView::InputFloat),
                any(std::string("InputFloat 浮点输入")),
                any(ImVec2(0,140)),
                any(ImVec2(180,24)),
                any(std::string("input_float_001")),
                any((int)6),
                any(std::string("ImGui::InputFloat")),
                any(std::string("浮点")),
                any(ptr([]()->float* {static float buf[4]{}; return buf; }())),
                any((float)0.1f),
                    any((float)1.f),
                    any(std::string("%.3f")),
                    any((ImGuiInputTextFlags)0)
            }
        },
        {
            "InputFloat3 三维浮点",
            {
                any(LView::InputFloat3),
                any(std::string("InputFloat3 三维浮点")),
                any(ImVec2(220,0)),
                any(ImVec2(180,24)),
                any(std::string("input_float3_001")),
                any((int)4),
                any(std::string("ImGui::InputFloat3")),
                any(std::string("三维浮点")),
                any(ptr([]()->float* {static float buf[4]{}; return buf; }())),
                any(std::string("%.3f")),
                any((ImGuiInputTextFlags)0)
            }
        },
        {
            "InputFloat4 四维浮点",
            {
                any(LView::InputFloat4),
                any(std::string("InputFloat4 四维浮点")),
                any(ImVec2(220,30)),
                any(ImVec2(180,24)),
                any(std::string("input_float4_001")),
                any((int)4),
                any(std::string("ImGui::InputFloat4")),
                any(std::string("四维浮点")),
                any(ptr([]()->float* {static float buf[4]{}; return buf; }())),
                any(std::string("%.3f")),
                    any((ImGuiInputTextFlags)0)
            }
        },
        {
            "InputInt2 二维整数",
            {
                any(LView::InputInt2),
                any(std::string("InputInt2 二维整数")),
                any(ImVec2(220,60)),
                any(ImVec2(180,24)),
                any(std::string("input_int2_001")),
                any((int)3),
                any(std::string("ImGui::InputInt2")),
                any(std::string("二维整数")),
                any(ptr([]()->int* {static int buf[4]{}; return buf; }())),
                any((ImGuiInputTextFlags)0)
            }
        },
        {
            "InputInt3 三维整数",
            {
                any(LView::InputInt3),
                any(std::string("InputInt3 三维整数")),
                any(ImVec2(220,90)),
                any(ImVec2(180,24)),
                any(std::string("input_int3_001")),
                any((int)3),
                any(std::string("ImGui::InputInt3")),
                any(std::string("三维整数")),
                any(ptr([]()->int* {static int buf[4]{}; return buf; }())),
                any((ImGuiInputTextFlags)0)
            }
        },
        {
            "InputInt4 四维整数",
            {
                any(LView::InputInt4),
                any(std::string("InputInt4 四维整数")),
                any(ImVec2(220,120)),
                any(ImVec2(180,24)),
                any(std::string("input_int4_001")),
                any((int)3),
                any(std::string("ImGui::InputInt4")),
                any(std::string("四维整数")),
                any(ptr([]()->int* {static int buf[4]{}; return buf; }())),
                any((ImGuiInputTextFlags)0)
            }
        },
        {
            "InputDouble 双精度输入",
            {
                any(LView::InputDouble),
                any(std::string("InputDouble 双精度输入")),
                any(ImVec2(0,170)),
                any(ImVec2(180,24)),
                any(std::string("input_double_001")),
                any((int)6),
                any(std::string("ImGui::InputDouble")),
                any(std::string("双精度")),
                any(ptr([]()->double* {static double v{}; return &v; }())),
                any((double)0.1),
                    any((double)1.0),
                    any(std::string("%.6f")),
                    any((ImGuiInputTextFlags)0)
            }
        },



    //滑块类
        {
            "SliderInt 整数滑块",
            {
                any(LView::SliderInt),
                any(std::string("SliderInt 整数滑块")),
                any(ImVec2(0, 560)),
                any(ImVec2(180, 24)),
                any(std::string("slider_int_001")),
                any((int)5),
                any(std::string("ImGui::SliderInt")),
                any(std::string("整数滑块")),
                any(ptr([]()->int* {static int v{}; return &v; }())),
                any((int)0),
                    any((int)100),
                    any(std::string("%d"))
            }
        },
        {
            "SliderInt2 二维整数滑块",
            {
                any(LView::SliderInt2),
                any(std::string("SliderInt2 二维整数滑块")),
                any(ImVec2(0,0)),
                any(ImVec2(180,24)),
                any(std::string("slider_int2_001")),
                any((int)6),
                any(std::string("ImGui::SliderInt2")),
                any(std::string("二维整数")),
                any(ptr([]()->int* {static int arr[4]{}; return arr; }())),
                any((int)0),
                    any((int)100),
                    any(std::string("%d")),
                    any((int)0)
            }
        },
        {
            "SliderInt3 三维整数滑块",
            {
                any(LView::SliderInt3),
                any(std::string("SliderInt3 三维整数滑块")),
                any(ImVec2(0,30)),
                any(ImVec2(180,24)),
                any(std::string("slider_int3_001")),
                any((int)6),
                any(std::string("ImGui::SliderInt3")),
                any(std::string("三维整数")),
                any(ptr([]()->int* {static int arr[4]{}; return arr; }())),
                any((int)0),
                    any((int)100),
                    any(std::string("%d")),
                    any((int)0)
            }
        },
        {
            "SliderInt4 四维整数滑块",
            {
                any(LView::SliderInt4),
                any(std::string("SliderInt4 四维整数滑块")),
                any(ImVec2(0,60)),
                any(ImVec2(180,24)),
                any(std::string("slider_int4_001")),
                any((int)6),
                any(std::string("ImGui::SliderInt4")),
                any(std::string("四维整数")),
                any(ptr([]()->int* {static int arr[4]{}; return arr; }())),
                any((int)0),
                    any((int)100),
                    any(std::string("%d")),
                    any((int)0)
            }
        },
        {
            "SliderFloat2 二维浮点滑块",
            {
                any(LView::SliderFloat2),
                any(std::string("SliderFloat2 二维浮点滑块")),
                any(ImVec2(220,0)),
                any(ImVec2(180,24)),
                any(std::string("slider_float2_001")),
                any((int)6),
                any(std::string("ImGui::SliderFloat2")),
                any(std::string("二维浮点")),
                any(ptr([]()->float* {static float arr[4]{}; return arr; }())),
                any((float)0.0f),
                    any((float)1.0f),
                    any(std::string("%.3f")),
                    any((int)0)
            }
        },
        {
            "SliderFloat3 三维浮点滑块",
            {
                any(LView::SliderFloat3),
                any(std::string("SliderFloat3 三维浮点滑块")),
                any(ImVec2(220,30)),
                any(ImVec2(180,24)),
                any(std::string("slider_float3_001")),
                any((int)6),
                any(std::string("ImGui::SliderFloat3")),
                any(std::string("三维浮点")),
                any(ptr([]()->float* {static float arr[4]{}; return arr; }())),
                any((float)0.0f),
                    any((float)1.0f),
                    any(std::string("%.3f")),
                    any((int)0)
            }
        },
        {
            "SliderFloat4 四维浮点滑块",
            {
                any(LView::SliderFloat4),
                any(std::string("SliderFloat4 四维浮点滑块")),
                any(ImVec2(220,60)),
                any(ImVec2(180,24)),
                any(std::string("slider_float4_001")),
                any((int)6),
                any(std::string("ImGui::SliderFloat4")),
                any(std::string("四维浮点")),
                any(ptr([]()->float* {static float arr[4]{}; return arr; }())),
                any((float)0.0f),
                    any((float)1.0f),
                    any(std::string("%.3f")),
                    any((int)0)
            }
        },

        {
            "SliderFloat 浮点滑块",
            {
                any(LView::SliderFloat),
                any(std::string("SliderFloat 浮点滑块")),
                any(ImVec2(0, 600)),
                any(ImVec2(180, 24)),
                any(std::string("slider_float_001")),
                any((int)5),
                any(std::string("ImGui::SliderFloat")),
                any(std::string("浮点滑块")),
                any(ptr([]()->float* {static float v{}; return &v; }())),
                any((float)0.0f),
                    any((float)1.0f),
                    any(std::string("%.3f"))
            }
        },
        {
            "VSliderInt 垂直整数滑块",
            {
                any(LView::VSliderInt),
                any(std::string("VSliderInt 垂直整数滑块")),
                any(ImVec2(200, 560)),
                any(ImVec2(24, 80)),
                any(std::string("vslider_int_001")),
                any((int)6),
                any(std::string("ImGui::VSliderInt")),
                any(std::string("垂直整数")),
                any(ImVec2(24, 80)),
                any(ptr([]()->int* {static int v{}; return &v; }())),
                any((int)0),
                    any((int)100),
                    any(std::string("%d"))
            }
        },
        {
            "VSliderFloat 垂直浮点滑块",
            {
                any(LView::VSliderFloat),
                any(std::string("VSliderFloat 垂直浮点滑块")),
                any(ImVec2(240, 560)),
                any(ImVec2(24, 80)),
                any(std::string("vslider_float_001")),
                any((int)6),
                any(std::string("ImGui::VSliderFloat")),
                any(std::string("垂直浮点")),
                any(ImVec2(24, 80)),
                any(ptr([]()->float* {static float v{}; return &v; }())),
                any((float)0.0f),
                    any((float)1.0f),
                    any(std::string("%.3f"))
            }
        },

        {
            "Combo 下拉选择框",
            {
                any(LView::Combo),
                any(std::string("Combo 下拉选择框")),
                any(ImVec2(0,0)),
                any(ImVec2(180,24)),
                any(std::string("combo_001")),
                any((int)5),
                any(std::string("ImGui::Combo")),
                any(std::string("下拉选项")),
                any(ptr([]()->int* {static int sel = 0; return &sel; }())),
                any([]()->const char** {static const char* arr[] = {"选项1","选项2","选项3","选项4"}; return arr; }()),
                any((int)3),
                    any((int)-1)
            }
        },

        {
            "TreeNode 树形节点",
            {
                any(LView::TreeNode),
                any(std::string("TreeNode 树形节点")),
                any(ImVec2(0,0)),
                any(ImVec2(160,22)),
                any(std::string("tree_node_001")),
                any((int)1),
                any(std::string("ImGui::TreeNode")),
                any(std::string("基础树节点"))
            }
        },
        {
            "TreeNodeEx 扩展树形节点",
            {
                any(LView::TreeNodeEx),
                any(std::string("TreeNodeEx 扩展树形节点")),
                any(ImVec2(0,30)),
                any(ImVec2(160,22)),
                any(std::string("tree_ex_001")),
                any((int)2),
                any(std::string("ImGui::TreeNodeEx")),
                any(std::string("扩展树节点")),
                any((int)0)
            }
        },
        {
            "CollapsingHeader 折叠标题",
            {
                any(LView::CollapsingHeader),
                any(std::string("CollapsingHeader 折叠标题")),
                any(ImVec2(0,60)),
                any(ImVec2(180,24)),
                any(std::string("header_001")),
                any((int)2),
                any(std::string("ImGui::CollapsingHeader")),
                any(std::string("普通折叠标题")),
                any((int)0)
            }
        },
        {
            "ProgressBar 进度条",
            {
                any(LView::ProgressBar),
                any(std::string("ProgressBar 进度条")),
                any(ImVec2(0,0)),
                any(ImVec2(180,22)),
                any(std::string("progress_001")),
                any((int)3),
                any(std::string("ImGui::ProgressBar")),
                any((float)0.45f),
                any(ImVec2(180,22)),
                any(std::string("45%"))
            }
        },
        {
            "DragInt 拖拽整数",
            {
                any(LView::DragInt),
                any(std::string("DragInt 拖拽整数")),
                any(ImVec2(0,0)),
                any(ImVec2(180,24)),
                any(std::string("drag_int_001")),
                any((int)7),
                any(std::string("ImGui::DragInt")),
                any(std::string("单整数拖拽")),
                any(ptr([]()->int* {static int buf[4]{0}; return buf; }())),
                any((float)1.0f),
                    any((int)0),
                    any((int)100),
                    any(std::string("%d")),
                    any((int)0)
            }
        },
        {
            "DragInt2 拖拽二维整数",
            {
                any(LView::DragInt2),
                any(std::string("DragInt2 拖拽二维整数")),
                any(ImVec2(0,30)),
                any(ImVec2(180,24)),
                any(std::string("drag_int2_001")),
                any((int)7),
                any(std::string("ImGui::DragInt2")),
                any(std::string("二维整数")),
                any(ptr([]()->int* {static int buf[4]{0}; return buf; }())),
                any((float)1.0f),
                    any((int)0),
                    any((int)100),
                    any(std::string("%d")),
                    any((int)0)
            }
        },
        {
            "DragInt3 拖拽三维整数",
            {
                any(LView::DragInt3),
                any(std::string("DragInt3 拖拽三维整数")),
                any(ImVec2(0,60)),
                any(ImVec2(180,24)),
                any(std::string("drag_int3_001")),
                any((int)7),
                any(std::string("ImGui::DragInt3")),
                any(std::string("三维整数")),
                any(ptr([]()->int* {static int buf[4]{0}; return buf; }())),
                any((float)1.0f),
                    any((int)0),
                    any((int)100),
                    any(std::string("%d")),
                    any((int)0)
            }
        },
        {
            "DragInt4 拖拽四维整数",
            {
                any(LView::DragInt4),
                any(std::string("DragInt4 拖拽四维整数")),
                any(ImVec2(0,90)),
                any(ImVec2(180,24)),
                any(std::string("drag_int4_001")),
                any((int)7),
                any(std::string("ImGui::DragInt4")),
                any(std::string("四维整数")),
                any(ptr([]()->int* {static int buf[4]{0}; return buf; }())),
                any((float)1.0f),
                    any((int)0),
                    any((int)100),
                    any(std::string("%d")),
                    any((int)0)
            }
        },
        {
            "DragFloat 拖拽浮点",
            {
                any(LView::DragFloat),
                any(std::string("DragFloat 拖拽浮点")),
                any(ImVec2(220,0)),
                any(ImVec2(180,24)),
                any(std::string("drag_float_001")),
                any((int)7),
                any(std::string("ImGui::DragFloat")),
                any(std::string("单浮点拖拽")),
                any(ptr([]()->float* {static float buf[4]{}; return buf; }())),
                any((float)1.f),
                    any((float)0.0f),
                    any((float)1.f),
                    any(std::string("%.3f")),
                    any((int)0)
            }
        },
        {
            "DragFloat2 拖拽二维浮点",
            {
                any(LView::DragFloat2),
                any(std::string("DragFloat2 拖拽二维浮点")),
                any(ImVec2(220,30)),
                any(ImVec2(180,24)),
                any(std::string("drag_float2_001")),
                any((int)7),
                any(std::string("ImGui::DragFloat2")),
                any(std::string("二维浮点")),
                any(ptr([]()->float* {static float buf[4]{}; return buf; }())),
                any((float)1.0f),
                    any((float)0.0f),
                    any((float)1.0f),
                    any(std::string("%.3f")),
                    any((int)0)
            }
        },
        {
            "DragFloat3 拖拽三维浮点",
            {
                any(LView::DragFloat3),
                any(std::string("DragFloat3 拖拽三维浮点")),
                any(ImVec2(220,60)),
                any(ImVec2(180,24)),
                any(std::string("drag_float3_001")),
                any((int)7),
                any(std::string("ImGui::DragFloat3")),
                any(std::string("三维浮点")),
                any(ptr([]()->float* {static float buf[4]{}; return buf; }())),
                any((float)1.0f),
                    any((float)0.0f),
                    any((float)1.f),
                    any(std::string("%.3f")),
                    any((int)0)
            }
        },
        {
            "DragFloat4 拖拽四维浮点",
            {
                any(LView::DragFloat4),
                any(std::string("DragFloat4 拖拽四维浮点")),
                any(ImVec2(220,90)),
                any(ImVec2(180,24)),
                any(std::string("drag_float4_001")),
                any((int)7),
                any(std::string("ImGui::DragFloat4")),
                any(std::string("四维浮点")),
                any(ptr([]()->float* {static float buf[4]{}; return buf; }())),
                any((float)1.f),
                    any((float)0.0f),
                    any((float)1.f),
                    any(std::string("%.3f")),
                    any((int)0)
            }
        },
        {
            "DragDouble 拖拽双精度",
            {
                any(LView::DragDouble),
                any(std::string("DragDouble 拖拽双精度")),
                any(ImVec2(0,120)),
                any(ImVec2(180,24)),
                any(std::string("drag_double_001")),
                any((int)7),
                any(std::string("ImGui::DragDouble")),
                any(std::string("双精度拖拽")),
                any(ptr([]()->double* {static double v{}; return &v; }())),
                any((float)1.0f),
                    any((double)0.0),
                    any((double)1.0),
                    any(std::string("%.6f")),
                    any((int)0)
            }
        },

        {
            "ColorEdit4 颜色选择",
            {
                any(LView::ColorEdit4),
                any(std::string("ColorEdit4 颜色选择")),
                any(ImVec2(0,0)),
                any(ImVec2(180,24)),
                any(std::string("coledit4_001")),
                any((int)3),
                any(std::string("ImGui::ColorEdit4")),
                any(std::string("RGBA颜色")),
                any(ptr([]()->float* {static float buf[4]{1.f,1.f,1.f,1.f}; return buf; }())),
                any((int)0)
            }
        },
        {
            "ColorEdit3 简化颜色选择",
            {
                any(LView::ColorEdit3),
                any(std::string("ColorEdit3 简化颜色选择")),
                any(ImVec2(0,30)),
                any(ImVec2(180,24)),
                any(std::string("coledit3_001")),
                any((int)3),
                any(std::string("ImGui::ColorEdit3")),
                any(std::string("RGB颜色")),
                any(ptr([]()->float* {static float buf[4]{1.f,1.f,1.f,1.f}; return buf; }())),
                any((int)0)
            }
        },
        {
            "ColorPicker4 完整拾色器",
            {
                any(LView::ColorPicker4),
                any(std::string("ColorPicker4 完整拾色器")),
                any(ImVec2(0,60)),
                any(ImVec2(180,24)),
                any(std::string("picker4_001")),
                any((int)4),
                any(std::string("ImGui::ColorPicker4")),
                any(std::string("完整RGBA拾色器")),
                any(ptr([]()->float* {static float buf[4]{1.f,1.f,1.f,1.f}; return buf; }())),
                any((int)0),
                    any(ptr([]()->float* {static float buf[4]{1.f,1.f,1.f,1.f}; return buf; }()))
            }
        },
        {
            "ColorPicker3 简化拾色器",
            {
                any(LView::ColorPicker3),
                any(std::string("ColorPicker3 简化拾色器")),
                any(ImVec2(0,90)),
                any(ImVec2(180,24)),
                any(std::string("picker3_001")),
                any((int)3),
                any(std::string("ImGui::ColorPicker3")),
                any(std::string("简易RGB拾色器")),
                any(ptr([]()->float* {static float buf[4]{1.f,1.f,1.f,1.f}; return buf; }())),
                any((int)0)
            }
        },
        {
            "PlotLines 折线图",
            {
                any(LView::PlotLines),
                any(std::string("PlotLines 折线图")),
                any(ImVec2(0,120)),
                any(ImVec2(220,100)),
                any(std::string("plot_line_001")),
                any((int)9),
                any(std::string("ImGui::PlotLines")),
                any(std::string("折线图表")),
                any(ptr([]()->float* {static float buf[8]{0.2f,0.5f,0.3f,0.7f,0.4f,0.9f,0.1f,0.6f}; return buf; }())),
                any((int)8),
                    any((int)0),
                    any(std::string("曲线")),
                    any((float)0.0f),
                    any((float)1.0f),
                    any(ImVec2(220,100)),
                    any((int)sizeof(float))
            }
        },
        {
            "PlotHistogram 柱状图",
            {
                any(LView::PlotHistogram),
                any(std::string("PlotHistogram 柱状图")),
                any(ImVec2(0,240)),
                any(ImVec2(220,100)),
                any(std::string("plot_hist_001")),
                any((int)9),
                any(std::string("ImGui::PlotHistogram")),
                any(std::string("柱状图表")),
                any(ptr([]()->float* {static float buf[8]{0.2f,0.5f,0.3f,0.7f,0.4f,0.9f,0.1f,0.6f}; return buf; }())),
                any((int)8),
                    any((int)0),
                    any(std::string("柱图")),
                    any((float)0.0f),
                    any((float)1.0f),
                    any(ImVec2(220,100)),
                    any((int)sizeof(float))
            }
        },
        {
            "ListBox 列表框",
            {
                any(LView::ListBox),
                any(std::string("ListBox 列表框")),
                any(ImVec2(0,0)),
                any(ImVec2(180,80)),
                any(std::string("listbox_001")),
                any((int)5),
                any(std::string("ImGui::ListBox")),
                any(std::string("列表控件")),
                any(ptr([]()->int* {static int sel = 0; return &sel; }())),
                any(ptr([]()->const char** {static const char* arr[] = {"选项1","选项2","选项3","选项4"}; return arr; }())),
                any((int)4),
                    any((int)3)
            }
        },


        {
            "Menu 下拉菜单",
            {
                any(LView::Menu),
                any(std::string("Menu 下拉菜单")),
                any(ImVec2(0,0)),
                any(ImVec2(120,24)),
                any(std::string("menu_001")),
                any((int)2),
                any(std::string("ImGui::Menu")),
                any(std::string("一级菜单")),
                any((bool)true)
            }
        },
        {
            "MenuItem 菜单项",
            {
                any(LView::MenuItem),
                any(std::string("MenuItem 菜单项")),
                any(ImVec2(0,30)),
                any(ImVec2(120,24)),
                any(std::string("menuitem_001")),
                any((int)4),
                any(std::string("ImGui::MenuItem")),
                any(std::string("普通菜单项")),
                any((char*)nullptr),
                any(ptr([]()->bool* {static bool v{}; return &v; }())),
                any((bool)true)
            }
        },

        {
            "Tooltip 悬浮提示",
            {
                any(LView::Tooltip),
                any(std::string("Tooltip 悬浮提示")),
                any(ImVec2(0,0)),
                any(ImVec2(100,20)),
                any(std::string("tooltip_001")),
                any((int)1),
                any(std::string("ImGui::Tooltip")),
                any(std::string("鼠标悬浮提示内容"))
            }
        },
        {
            "SetTooltip 简易悬浮提示",
            {
                any(LView::SetTooltip),
                any(std::string("SetTooltip 简易悬浮提示")),
                any(ImVec2(0,0)),
                any(ImVec2(100,20)),
                any(std::string("set_tooltip_001")),
                any((int)1),
                any(std::string("ImGui::SetTooltip")),
                any(std::string("鼠标悬浮提示文本"))
            }
        },
   
};



//enum code pos rect function
std::map<std::string, std::vector<any>> LaoutList_tRNA =
{
    //========== 窗口Begin创建系列 ==========
    {
        "Begin 普通窗口",
        {
            any(LLayout::Begin),
            any(std::string("End")),       //是否要配对哪一个结束代码
            any(ImVec2(0,0)),
            any(ImVec2(50,50)),
            any(std::string("普通窗口")),  //reality id
            any((int)3),//参数数量
            any(std::string("ImGui::Begin")),
            any(std::string("普通窗口")), //imgui id
            any((bool*)nullptr),
            any((ImGuiWindowFlags)ImGuiWindowFlags_None)
        }
    },
    {
        "End",
        {
            any(LLayout::End),
            any(std::string("End")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("End")),
            any((int)0),
            any(std::string("ImGui::End"))
        }
    },


    {
        "BeginMainMenuBar 全局菜单栏",
        {
            any(LLayout::BeginMainMenuBar),
            any(std::string("EndMainMenuBar")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("BeginMainMenuBar")),
            any((int)0),
            any(std::string("ImGui::BeginMainMenuBar"))
        }
    },
    {
        "EndMainMenuBar",
        {
            any(LLayout::EndMainMenuBar),
            any(std::string("EndMainMenuBar")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("EndMainMenuBar")),
            any((int)0),
            any(std::string("ImGui::EndMainMenuBar"))
        }
    },

    {
        "PushClipRect 裁剪",
         {
         any(LLayout::PushClipRect),
         any(std::string("PopClipRect")),
         any(ImVec2(0,0)),
         any(ImVec2(0,0)),
         any(std::string("PushClipRect")),
         any((int)3),
         any(std::string("ImGui::PushClipRect")),
         any(ImVec2(110,110)),
         any(ImVec2(200,200)),
         any((bool)false)
          }
     },
      {
      "PopClipRect",
      {
          any(LLayout::PopClipRect),
          any(std::string("PopClipRect")),
          any(ImVec2(0,0)),
          any(ImVec2(0,0)),
          any(std::string("PopClipRect")),
          any((int)0),
          any(std::string("ImGui::PopClipRect"))
      }
     },
    
    {
        "AddCircleFilled 实心圆",
        {
            any(LLayout::AddCircleFilled),
            any(std::string("AddCircleFilled 实心圆")),
            any(ImVec2(40,40)),
            any(ImVec2(40,40)),
            any(std::string("AddCircleFilled")),
            any((int)4),
            any(std::string("ImGui::GetWindowDrawList()->AddCircleFilled")),
            any(ImVec2(0,0)),
            any((float)100.0f),
            any((ImU32)IM_COL32(10,200,25,255)),
            any((int)32)
        }
    },

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

    {
        "AddRectFilled 实心矩形",
        {
            any(LLayout::AddRectFilled),
            any(std::string("AddRectFilled 实心矩形")),
            any(ImVec2(40,40)),
            any(ImVec2(40,40)),
            any(std::string("AddRectFilled")),
            any((int)5),
            any(std::string("ImGui::GetWindowDrawList()->AddRectFilled")),
            any(ImVec2(0,0)),
            any(ImVec2(100,100)),
            any((ImU32)IM_COL32(10,200,25,255)),
            any((float)10.0f),
            any((ImDrawFlags)ImDrawFlags_None)
        }
    },
    {
        "AddRectFilledMultiColor 渐变矩形",
        {
            any(LLayout::AddRectFilledMultiColor),
            any(std::string("AddRectFilledMultiColor 渐变矩形")),
            any(ImVec2(40,40)),
            any(ImVec2(40,40)),
            any(std::string("AddRectFilledMultiColor")),
            any((int)6),
            any(std::string("ImGui::GetWindowDrawList()->AddRectFilledMultiColor")),
            any(ImVec2(0,0)),
            any(ImVec2(100,100)),
            any((ImU32)IM_COL32(10,200,25,255)),
            any((ImU32)IM_COL32(100,20,25,255)),
            any((ImU32)IM_COL32(30,20,105,255)),
            any((ImU32)IM_COL32(150,200,25,205))
        }
    },

    {
        "AddCircle 空心圆",
        {
            any(LLayout::AddCircle),
            any(std::string("AddCircle 空心圆")),
            any(ImVec2(40,40)),
            any(ImVec2(40,40)),
            any(std::string("AddCircle")),
            any((int)5),
            any(std::string("ImGui::GetWindowDrawList()->AddCircle")),
            any(ImVec2(0,0)),
            any((float)100.0f),
            any((ImU32)IM_COL32(10,200,25,255)),
            any((int)32),
            any((float)1.0f)
        }
    },

    {
        "AddEllipse 空心椭圆",
        {
            any(LLayout::AddEllipse),
            any(std::string("AddEllipse 空心椭圆")),
            any(ImVec2(40,40)),
            any(ImVec2(40,40)),
            any(std::string("AddEllipse")),
            any((int)5),
            any(std::string("ImGui::GetWindowDrawList()->AddEllipse")),
            any(ImVec2(202,176)),
            any(ImVec2(230,200)),
            any((ImU32)IM_COL32(200,20,25,255)),
            any((int)110),
            any((float)0.0f)
        }
    },
    
    {
    "PathEllipticalArcTo 椭圆弧路径",
    {
        any(LLayout::PathEllipticalArcTo),
        any(std::string("PathEllipticalArcTo 椭圆弧路径")),
        any(ImVec2(100, 100)),
        any(ImVec2(120, 80)),
        any(std::string("path_elli_arc_001")),
        any((int)6),
        any(std::string("ImGui::GetWindowDrawList()->PathEllipticalArcTo")),
        any(ImVec2(100, 100)),    
        any(ImVec2(60, 30)),     
        any((float)0.f),         
        any((float)0.f),        
        any((float)3.14 * 2.f),  
        any((int)32)             
    }
    },
    
    {
        "AddLine 直线",
        {
            any(LLayout::AddLine),
            any(std::string("AddLine 直线")),
            any(ImVec2(20,20)),
            any(ImVec2(120,90)),
            any(std::string("AddLine")),
            any((int)4),
            any(std::string("ImGui::GetWindowDrawList()->AddLine")),
            any(ImVec2(20,20)),
            any(ImVec2(120,90)),
            any((ImU32)IM_COL32(255,255,255,255)),
            any((float)2.0f)
        }
    },
    {
        "AddBezierCurve 三阶贝塞尔曲线",
        {
            any(LLayout::AddBezierCurve),
            any(std::string("AddBezierCurve 三阶贝塞尔曲线")),
            any(ImVec2(20,110)),
            any(ImVec2(200,180)),
            any(std::string("AddBezierCurve")),
            any((int)6),
            any(std::string("ImGui::GetWindowDrawList()->AddBezierCurve")),
            any(ImVec2(20,110)),
            any(ImVec2(70,60)),
            any(ImVec2(140,200)),
            any(ImVec2(200,180)),
            any((ImU32)IM_COL32(80,180,255,255)),
            any((float)2.0f)
        }
    },
    {
        "AddBezierCurveQuadratic 二阶贝塞尔曲线",
        {
            any(LLayout::AddBezierCurveQuadratic),
            any(std::string("AddBezierCurveQuadratic 二阶贝塞尔曲线")),
            any(ImVec2(20,220)),
            any(ImVec2(180,260)),
            any(std::string("AddBezierCurveQuadratic")),
            any((int)5),
            any(std::string("ImGui::GetWindowDrawList()->AddBezierCurveQuadratic")),
            any(ImVec2(20,220)),
            any(ImVec2(100,160)),
            any(ImVec2(180,260)),
            any((ImU32)IM_COL32(255,180,80,255)),
            any((float)2.0f)
        }
    },
    {
        "AddPathLineTo 路径画线",
        {
            any(LLayout::AddPathLineTo),
            any(std::string("AddPathLineTo 路径画线")),
            any(ImVec2(50,300)),
            any(ImVec2(150,360)),
            any(std::string("AddPathLineTo")),
            any((int)1),
            any(std::string("ImGui::GetWindowDrawList()->AddPathLineTo")),
            any(ImVec2(150,360))
        }
    },
    {
        "AddText 普通顶层文字",
        {
            any(LLayout::AddText),
            any(std::string("AddText 普通顶层文字")),
            any(ImVec2(40,300)),
            any(ImVec2(40,300)),
            any(std::string("AddText")),
            any((int)8),
            any(std::string("ImGui::GetWindowDrawList()->AddText")),
            any((ImFont*)nullptr),
            any((float)14.0f),
            any(ImVec2(40,300)),
            any((ImU32)IM_COL32(255,255,255,255)),
            any(std::string("顶层文字测试")),
            any((char*)nullptr),
            any((float)0.0f),
            any((ImVec4*)nullptr)
        }
    },
    {
        "AddTextClipped 裁剪文字",
        {
            any(LLayout::AddTextClipped),
            any(std::string("AddTextClipped 裁剪文字")),
            any(ImVec2(40,340)),
            any(ImVec2(180,380)),
            any(std::string("AddTextClipped")),
            any((int)8),
            any(std::string("ImGui::GetWindowDrawList()->AddTextClipped")),
            any((ImFont*)nullptr),
            any((float)14.0f),
            any(ImVec2(40,340)),
            any(ImVec2(180,380)),
            any((ImU32)IM_COL32(255,200,80,255)),
            any(std::string("被边界裁剪的文字")),
            any((char*)nullptr),
            any((ImVec2*)nullptr)
        }
    },
    {
        "AddTriangle 空心三角",
        {
            any(LLayout::AddTriangle),
            any(std::string("AddTriangle 空心三角")),
            any(ImVec2(60,50)),
            any(ImVec2(60,50)),
            any(std::string("AddTriangle")),
            any((int)5),
            any(std::string("ImGui::GetWindowDrawList()->AddTriangle")),
            any(ImVec2(60,50)),
            any(ImVec2(10,120)),
            any(ImVec2(110,120)),
            any((ImU32)IM_COL32(255,255,255,255)),
            any((float)2.0f)
        }
    },
    {
        "AddTriangleFilled 实心三角",
        {
            any(LLayout::AddTriangleFilled),
            any(std::string("AddTriangleFilled 实心三角")),
            any(ImVec2(160,50)),
            any(ImVec2(160,50)),
            any(std::string("AddTriangleFilled")),
            any((int)4),
            any(std::string("ImGui::GetWindowDrawList()->AddTriangleFilled")),
            any(ImVec2(160,50)),
            any(ImVec2(110,120)),
            any(ImVec2(210,120)),
            any((ImU32)IM_COL32(40,160,220,180))
        }
    },
    {
        "AddQuad 空心四边形",
        {
            any(LLayout::AddQuad),
            any(std::string("AddQuad 空心四边形")),
            any(ImVec2(60,150)),
            any(ImVec2(60,150)),
            any(std::string("AddQuad")),
            any((int)6),
            any(std::string("ImGui::GetWindowDrawList()->AddQuad")),
            any(ImVec2(60,150)),
            any(ImVec2(60,240)),
            any(ImVec2(130,240)),
            any(ImVec2(130,150)),
            any((ImU32)IM_COL32(255,255,255,255)),
            any((float)2.0f)
        }
    },
    {
        "AddQuadFilled 实心四边形",
        {
            any(LLayout::AddQuadFilled),
            any(std::string("AddQuadFilled 实心四边形")),
            any(ImVec2(160,150)),
            any(ImVec2(160,150)),
            any(std::string("AddQuadFilled")),
            any((int)5),
            any(std::string("ImGui::GetWindowDrawList()->AddQuadFilled")),
            any(ImVec2(160,150)),
            any(ImVec2(160,240)),
            any(ImVec2(230,240)),
            any(ImVec2(230,150)),
            any((ImU32)IM_COL32(220,100,60,180))
        }
    },
    {
        "AddPolyline 折线轮廓",
        {
            any(LLayout::AddPolyline),
            any(std::string("AddPolyline 折线轮廓")),
            any(ImVec2(60,260)),
            any(ImVec2(60,260)),
            any(std::string("AddPolyline")),
            any((int)5),
            any(std::string("ImGui::GetWindowDrawList()->AddPolyline")),
            any((ImVec2*)nullptr),
            any((int)3),
            any((ImU32)IM_COL32(255,255,255,255)),
            any((bool)false),
            any((float)2.0f)
        }
    },
    {
        "AddConvexPolyFilled 凸多边形填充",
        {
            any(LLayout::AddConvexPolyFilled),
            any(std::string("AddConvexPolyFilled 凸多边形填充")),
            any(ImVec2(160,260)),
            any(ImVec2(160,260)),
            any(std::string("AddConvexPolyFilled")),
            any((int)3),
            any(std::string("ImGui::GetWindowDrawList()->AddConvexPolyFilled")),
            any((ImVec2*)nullptr),
            any((int)4),
            any((ImU32)IM_COL32(80,220,140,180))
        }
    },
    {
        "AddPolygon 任意多边形",
        {
            any(LLayout::AddPolygon),
            any(std::string("AddPolygon 任意多边形")),
            any(ImVec2(60,360)),
            any(ImVec2(60,360)),
            any(std::string("AddPolygon")),
            any((int)4),
            any(std::string("ImGui::GetWindowDrawList()->AddPolygon")),
            any((ImVec2*)nullptr),
            any((int)5),
            any((ImU32)IM_COL32(255,255,255,255)),
            any((float)2.0f)
        }
    },
    {
    "PathEllipseTo 路径椭圆",
    {
        any(LLayout::PathEllipseTo),
        any(std::string("PathEllipseTo 路径椭圆")), 
        any(ImVec2(100, 100)),                    
        any(ImVec2(120, 80)),                    
        any(std::string("path_ellipseto_001")),    
        any((int)5),                              
        any(std::string("ImGui::GetWindowDrawList()->PathEllipseTo")),
        any(ImVec2(100, 100)),  
        any(ImVec2(60, 30)),    
        any((float)0.f),         
        any((float)3.14 * 2.f),  
        any((int)32)             
    }
    },


    {
        "BeginModal 模态窗口",
        {
            any(LLayout::BeginModal),
            any(std::string("ImGui::BeginModal")),
            any(ImVec2(50,50)),
            any(std::string("模态窗口")),
            any((bool*)nullptr),
            any((int)0)
        }
    },

    //容器
    {
        "BeginChild 子容器面板",
        {
            any(LLayout::BeginChild),
            any(std::string("EndChild")),
            any(ImVec2(0,0)),
            any(ImVec2(50,50)),
            any(std::string("子容器面板")),
            any((int)4),//恢复4个参数
            any(std::string("ImGui::BeginChild")),
            any(std::string("子容器面板")), //id
            any(ImVec2(100,100)),          //size
            any((bool)true),                //border
            any((ImGuiWindowFlags)ImGuiWindowFlags_None)                     //flags
        }
    },

    {
        "EndChild",
        {
            any(LLayout::EndChild),
            any(std::string("EndChild")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("EndChild")),
            any((int)0),
            any(std::string("ImGui::EndChild"))
        }
    },

    {
        "BeginGroup 分组容器",
        {
            any(LLayout::BeginGroup),
            any(std::string("EndGroup")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("BeginGroup")),
            any((int)0),
            any(std::string("ImGui::BeginGroup"))
        }
    },

    {
        "EndGroup",
        {
            any(LLayout::EndGroup),
            any(std::string("EndGroup")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("EndGroup")),
            any((int)0),
            any(std::string("ImGui::EndGroup"))
        }
    },

    {
        "SameLine 同行布局",
        {
            any(LLayout::SameLine),
            any(std::string("SameLine 同行布局")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("SameLine")),
            any((int)0),
            any(std::string("ImGui::SameLine"))
        }
    },
    {
        "Dummy 空白占位",
        {
            any(LLayout::Dummy),
            any(std::string("Dummy 空白占位")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("Dummy")),
            any((int)1),
            any(std::string("ImGui::Dummy")),
            any(ImVec2(100,100))
        }
    },
    {
        "Spacing 空白间距",
        {
            any(LLayout::Spacing),
            any(std::string("Spacing 空白间距")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("space_001")),
            any((int)0),
            any(std::string("ImGui::Spacing"))
        }
    },

    {
        "SeparatorText 文字分隔线",
        {
            any(LLayout::SeparatorText),
            any(std::string("SeparatorText 文字分隔线")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("SeparatorText")),
            any((int)1),
            any(std::string("ImGui::SeparatorText")),
            any(std::string("标题1"))
        }
    },

    {
        "Separator 分隔线",
        {
            any(LLayout::Separator),
            any(std::string("Separator 分隔线")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("Separator")),
            any((int)0),
            any(std::string("ImGui::Separator"))
        }
    },
    {
        "SetNextWindowPos 设置窗口位置",
        {
            any(LLayout::SetNextWindowPos),
            any(std::string("SetNextWindowPos 设置窗口位置")),
            any(ImVec2(10,10)),
            any(ImVec2(120,24)),
            any(std::string("win_pos_001")),
            any((int)3),
            any(std::string("ImGui::SetNextWindowPos")),
            any(ImVec2(10,10)),
            any((ImGuiCond)ImGuiCond_None),
            any(ImVec2(0,0))
        }
    },
    {
        "SetNextWindowSize 设置窗口大小",
        {
            any(LLayout::SetNextWindowSize),
            any(std::string("SetNextWindowSize 设置窗口大小")),
            any(ImVec2(0,40)),
            any(ImVec2(180,24)),
            any(std::string("win_size_001")),
            any((int)2),
            any(std::string("ImGui::SetNextWindowSize")),
            any(ImVec2(400,300)),
            any((ImGuiCond)ImGuiCond_None)
        }
    },
    {
        "PushStyleColor 样式颜色压栈",
        {
            any(LLayout::PushStyleColor),
            any(std::string("PopStyleColor")),
            any(ImVec2(0,0)),
            any(ImVec2(40,20)),
            any(std::string("push_color_001")),
            any((int)2),
            any(std::string("ImGui::PushStyleColor")),
            any((ImGuiCol)ImGuiCol_WindowBg),   // ImGuiCol 枚举int
            any(ImVec4(1.0f, 1.0f, 1.0f, 1.0f))// 原生ImVec4存入any，新分支自动识别调色
        }
    },
    {
        "PopStyleColor",
        {
            any(LLayout::PopStyleColor),
            any(std::string("PopStyleColor")),
            any(ImVec2(0,40)),
            any(ImVec2(180,24)),
            any(std::string("PopStyleColor")),
            any((int)0),
            any(std::string("ImGui::PopStyleColor"))
        }
    },
    {
        "PushStyleVar 样式(ImVec2)",
        {
            any(LLayout::PushStyleVar_ImVec2),
            any(std::string("PopStyleVar")),
            any(ImVec2(0,0)),
            any(ImVec2(40,20)),
            any(std::string("push_var_Imvec2_001")),
            any((int)2),
            any(std::string("ImGui::PushStyleVar")),
            any((ImGuiStyleVar)ImGuiStyleVar_FramePadding),
            any(ImVec2(8,8))
        }
    },
    {
        "PushStyleVar 样式(Float)",
        {
            any(LLayout::PushStyleVar_Float),
            any(std::string("PopStyleVar")),
            any(ImVec2(0,0)),
            any(ImVec2(40,20)),
            any(std::string("push_var_Float_001")),
            any((int)2),
            any(std::string("ImGui::PushStyleVar")),
            any((ImGuiStyleVar)ImGuiStyleVar_FrameRounding),
            any((float)0)
        }
    },
    {
        "PopStyleVar",
        {
            any(LLayout::PopStyleVar),
            any(std::string("PopStyleVar")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("PopStyleVar")),
            any((int)0),
            any(std::string("ImGui::PopStyleVar")),
        }
    },

    {
        "NewLine 换行",
        {
            any(LLayout::NewLine),
            any(std::string("NewLine 换行")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("line_001")),
            any((int)0),
            any(std::string("ImGui::NewLine"))
        }
    },
    {
        "Columns 多列布局",
        {
            any(LLayout::Columns),
            any(std::string("ColumnsEnd")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("cols_001")),
            any((int)3),
            any(std::string("ImGui::Columns")),
            any((int)1),
            any(std::string("col_id_001")),
            any((bool)true)
        }
    },
    {
        "NextColumn 切换列",
        {
            any(LLayout::NextColumn),
            any(std::string("NextColumn 切换列")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("next_col_001")),
            any((int)0),
            any(std::string("ImGui::NextColumn"))
        }
    },
    {
        "ColumnsEnd",
        {
            any(LLayout::ColumnsEnd),
            any(std::string("ColumnsEnd")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("ColumnsEnd")),
            any((int)0),
            any(std::string("ImGui::ColumnsEnd"))
        }
    },
    {  
        "PathClear 清空路径",
        {
            any(LLayout::PathClear),
            any(std::string("PathClear 清空路径")),
            any(ImVec2(0,0)),
            any(ImVec2(0,0)),
            any(std::string("path_clear_001")),
            any((int)0),
            any(std::string("ImGui::GetWindowDrawList()->PathClear"))
        }
},
{
    "PathArcTo 路径圆弧",
    {
        any(LLayout::PathArcTo),
        any(std::string("PathArcTo 路径圆弧")),
        any(ImVec2(80,80)),
        any(ImVec2(120,120)),
        any(std::string("path_arcto_001")),
        any((int)5),
        any(std::string("ImGui::GetWindowDrawList()->PathArcTo")),
        any(ImVec2(80,80)),
        any((float)40.f),
        any((float)0.f),
        any((float)3.14 * 2.f),
        any((int)32)
    }
},
{
    "PathStroke 路径描边",
    {
        any(LLayout::PathStroke),
        any(std::string("PathStroke 路径描边")),
        any(ImVec2(0,0)),
        any(ImVec2(0,0)),
        any(std::string("path_stroke_001")),
        any((int)2),
        any(std::string("ImGui::GetWindowDrawList()->PathStroke")),
        any((ImU32)IM_COL32(255,255,255,255)),
        any((float)2.f)
    }
},
{
    "PathFillConvex 填充凸路径",
    {
        any(LLayout::PathFillConvex),
        any(std::string("PathFillConvex 填充凸路径")),
        any(ImVec2(0,0)),
        any(ImVec2(0,0)),
        any(std::string("path_fill_001")),
        any((int)1),
        any(std::string("ImGui::GetWindowDrawList()->PathFillConvex")),
        any((ImU32)IM_COL32(80,160,255,180))
    }
},
{
"PathLineTo 路径画线",
{
    any(LLayout::PathLineTo),
    any(std::string("PathLineTo 路径画线")),
    any(ImVec2(0,0)),
    any(ImVec2(0,0)),
    any(std::string("path_lineto_001")),
    any((int)1),
    any(std::string("ImGui::GetWindowDrawList()->PathLineTo")),
    any(ImVec2(100,100))
}
},
     {
"AddImage 绘制图片",
{
    any(LLayout::AddImage),
    any(std::string("AddImage 绘制图片")),
    any(ImVec2(0,0)),
    any(ImVec2(128,128)),
    any(std::string("img_001")),
    any((int)6),
    any(std::string("ImGui::GetWindowDrawList()->AddImage")),
    any((ImTextureID)0),
    any(ImVec2(0,0)),
    any(ImVec2(128,128)),
    any(ImVec2(0,0)),
    any(ImVec2(1,1)),
    any((ImU32)IM_COL32_WHITE)
}
     },
{
"AddImageQuad 四边形贴图",
{
    any(LLayout::AddImageQuad),
    any(std::string("AddImageQuad 四边形贴图")),
    any(ImVec2(0,0)),
    any(ImVec2(128,128)),
    any(std::string("img_quad_001")),
    any((int)8),
    any(std::string("ImGui::GetWindowDrawList()->AddImageQuad")),
    any((ImTextureID)0),
    any(ImVec2(0,0)),
    any(ImVec2(128,0)),
    any(ImVec2(128,128)),
    any(ImVec2(0,128)),
    any(ImVec2(0,0)),
    any(ImVec2(1,0)),
    any(ImVec2(1,1)),
    any(ImVec2(0,1))
}
},
{
"AddImageRounded 圆角图片",
{
    any(LLayout::AddImageRounded),
    any(std::string("AddImageRounded 圆角图片")),
    any(ImVec2(0,0)),
    any(ImVec2(128,128)),
    any(std::string("img_round_001")),
    any((int)7),
    any(std::string("ImGui::GetWindowDrawList()->AddImageRounded")),
    any((ImTextureID)0),
    any(ImVec2(0,0)),
    any(ImVec2(128,128)),
    any(ImVec2(0,0)),
    any(ImVec2(1,1)),
    any((float)8.f),
    any((ImU32)IM_COL32_WHITE)
}
},
{
"PathArcToFast 快速圆弧",
{
    any(LLayout::PathArcToFast),
    any(std::string("PathArcToFast 快速圆弧")),
    any(ImVec2(80,80)),
    any(ImVec2(120,120)),
    any(std::string("path_arctofast_001")),
    any((int)4),
    any(std::string("ImGui::GetWindowDrawList()->PathArcToFast")),
    any(ImVec2(80,80)),
    any((float)40.f),
    any((float)0.f),
    any((float)3.14f)
}
},
//一个Sfaui的特例化控件

{
  "Sfaui图层 SfauiLayer（可以让控件整体偏移）",
{
    any(LLayout::SfauiLayer),
    any(std::string("SfauiLayerEnd")),
    any(ImVec2(80,80)),
    any(ImVec2(120,120)),
    any(std::string("SfauiLayer")),
    any((int)2),
     any(std::string("//Sfaui layer {")),
    any(ImVec2(80,80)),
    any(ImVec4(1.0f, 1.0f, 1.0f, 1.0f))
}
},

{
  "SfauiLayerEnd",
{
    any(LLayout::SfauiLayerEnd),
    any(std::string("SfauiLayerEnd")),
    any(ImVec2(80,80)),
    any(ImVec2(120,120)),
    any(std::string("SfauiLayerEnde")),
    any((int)0),
    any(std::string("//Sfaui layer }")) 
    
}
}
};



std::vector<CtrlCategory> PrintLaoutList = {
    {
        "窗口容器 / Window Begin",
        {
            "Begin 普通窗口", //适配
            "BeginMainMenuBar 全局菜单栏", //适配
            //"BeginMenuBar 窗口菜单栏",
            //"BeginMenu 菜单"
        }
    },
    
    {
        "布局排版 / Layout",
        {        
            "BeginChild 子容器面板", //适配
            "BeginGroup 分组容器", //适配
            "PushClipRect 裁剪",//适配
            "Columns 多列布局",//适配
            // "NextColumn 切换下一列",
            "Separator 分隔线", //适配
            "SeparatorText 文字分隔线", //适配
            "NewLine 换行",//适配
            "SameLine 同行布局", //适配
            "Spacing 空白间距",//适配
            "Dummy 空白占位"//适配
        }
    },
    {
        "矩形绘制 / Rect Draw",
        {
            "AddRect 空心矩形",//适配
            "AddRectFilled 实心矩形",//适配
            "AddRectFilledMultiColor 渐变矩形",//适配
        }
    },
    {
        "圆形绘制 / Circle Draw",
        {
            "AddCircle 空心圆",//适配
            "AddCircleFilled 实心圆", //适配
            "AddEllipse 空心椭圆"//适配
        }
    },
    {
        "多边形绘制 / Polygon Draw",
        {
            "AddTriangle 空心三角",//适配
            "AddTriangleFilled 实心三角",//适配
            "AddQuad 空心四边形",//适配
            "AddQuadFilled 实心四边形",//适配
            //"AddPolyline 折线轮廓",
            // "AddConvexPolyFilled 凸多边形填充",
            //"AddPolygon 任意多边形",
            //"AddPolygonFilled 任意多边形填充"
        }
    },
    {
        "线条曲线 / Line & Curve",
        {
            "AddLine 直线",//适配
            //"AddBezierCurve 三阶贝塞尔曲线",
            //"AddBezierCurveQuadratic 二阶贝塞尔曲线",
            "PathClear 清空路径",//适配         
            "PathStroke 路径描边",//适配
            "PathFillConvex 填充凸路径",//适配
            "PathArcTo 路径圆弧",//适配
            "PathEllipticalArcTo 椭圆弧路径",//适配
            "PathLineTo 路径画线",//适配
            "PathArcToFast 快速圆弧"//适配
            //"AddPathBezierCurveTo 贝塞尔路径"
        }
    },
    {
        "顶层文字 / Overlay Text",
        {
            "AddText 普通顶层文字",//适配
            //"AddTextClipped 裁剪文字",
            //"AddTextWrapped 自动换行文字"
        }
    },
    {
        "图片贴图 / Image Draw",
        {
        "AddImage 绘制图片",//适配
       // "AddImageQuad 四边形贴图",
        "AddImageRounded 圆角图片"//适配
    }
},
{
    "样式属性 / Style & State",
    {
        "SetNextWindowPos 设置窗口位置",//适配
        "SetNextWindowSize 设置窗口大小",//适配
        "PushStyleColor 样式颜色压栈",//适配
        "PushStyleVar 样式(ImVec2)",//适配
        "PushStyleVar 样式(Float)"//适配
    }
}
};

std::vector<CtrlCategory> PrintViewList = {
    {
        "文本控件 / Text Widgets",
        {
            "Text 普通文本",//适配
            "TextWrapped 换行文本",//适配
            "LabelText 标签文本",//适配
            "BulletText 项目符号",//适配
            "Bullet 无文本项目符号",//适配
            "TextDisabled 灰色禁用文本"//适配
        }
    },
    {
        "按钮控件 / Buttons",
        {
            "Button 普通按钮",//适配
            "SmallButton 小按钮",//适配
            "InvisibleButton 透明按钮",//适配
            "CheckboxFlags 位域复选框",//适配
        }
    },
    {
        "输入控件 / Inputs",
        {
            "InputText 单行输入",//适配
            "InputTextMultiline 多行输入",//适配
            "InputInt 整数输入",//适配
            "InputFloat 浮点输入",//适配
            "InputFloat3 三维浮点",//适配
            "InputFloat4 四维浮点",//适配
            "InputInt2 二维整数",//适配
            "InputInt3 三维整数",//适配
            "InputInt4 四维整数",//适配
            "InputDouble 双精度输入"//适配
        }
    },
    {
        "选择勾选 / Select & Check",
        {
            "Checkbox 复选框",//适配
            "RadioButton 单选框",//适配
            //"Combo 下拉选择框",
            //"ListBox 列表框",//适配
        }
    },
    {
        "滑块进度 / Slider & Progress",
        {
            "SliderInt 整数滑块",//适配
            "SliderFloat 浮点滑块",//适配
            "SliderInt2 二维整数滑块",//适配
            "SliderInt3 三维整数滑块",//适配
            "SliderInt4 四维整数滑块",//适配
            "SliderFloat2 二维浮点滑块",//适配
            "SliderFloat3 三维浮点滑块",//适配
            "SliderFloat4 四维浮点滑块",//适配
            "VSliderInt 垂直整数滑块",//适配
            "VSliderFloat 垂直浮点滑块",//适配
            "ProgressBar 进度条"//适配
        }
    },
    {
        "树形列表 / Tree & Collapse",
        {
            "TreeNode 树形节点",//适配
            "CollapsingHeader 折叠标题",//适配
            // "CollapsingHeaderEx 扩展折叠标题"
        }
    },
    {
        "颜色图表 / Color & Plot",
        {
            "ColorEdit4 颜色选择",//适配
            "ColorEdit3 简化颜色选择",//适配
            "ColorPicker4 完整拾色器",//适配
            "ColorPicker3 简化拾色器",//适配
            "PlotLines 折线图",//适配
            "PlotHistogram 柱状图"//适配
        }
    },
    {
        "提示弹窗 / Tooltip & Popup",
        {
            "SetTooltip 简易悬浮提示"//适配
        }
    },
    {
        "菜单工具 / Menu & Toolbar",
        {
            "MenuItem 菜单项",//适配
        }
    },
    {
        "拖拽调节 / Drag Controls",
        {
            "DragInt 拖拽整数",//适配
            "DragInt2 拖拽二维整数",//适配
            "DragInt3 拖拽三维整数",//适配
            "DragInt4 拖拽四维整数",//适配
            "DragFloat 拖拽浮点",//适配
            "DragFloat2 拖拽二维浮点",//适配
            "DragFloat3 拖拽三维浮点",//适配
            "DragFloat4 拖拽四维浮点",//适配
            "DragDouble 拖拽双精度"//适配
        }
    }
};


//动态生成ui
//已经适配:Begin BeginMainMenuBar
int DrawNewUi()
{
    //用于绘制
    ImDrawList* draw = nullptr;
    draw = ImGui::GetBackgroundDrawList();

    for (int i = 0; i < All_mRNA.size(); i++)
    {
        if (Visibility[i] == 0)
            continue;


        auto& arr = All_mRNA[i];
        auto getLV = arr[VLENUM];
        auto id = arr[REALID];  //跳转到参数部分，进行绘制
        if (!getLV.has_value()) continue;
        if (getLV.type() == typeid(LLayout)) {
            switch (any_cast<LLayout>(getLV))
            {
    
            case Begin:
                ImGui::Begin(any_cast<std::string>(arr[PARAMST]).c_str(),
                    any_cast<bool*>(arr[PARAMST + 1]),
                    any_cast<ImGuiWindowFlags>(arr[PARAMST + 2]));
                draw = ImGui::GetWindowDrawList();

                break;
            case End:
                ImGui::End();
                break;

            case BeginMainMenuBar:
                ImGui::BeginMainMenuBar();
                break;
            case EndMainMenuBar:
                ImGui::EndMainMenuBar();
                break;
            case BeginChild:
                ImGui::BeginChild(any_cast<std::string>(arr[PARAMST]).c_str(),
                    any_cast<ImVec2>(arr[PARAMST + 1]),
                    any_cast<bool>(arr[PARAMST + 2]),
                    any_cast<ImGuiWindowFlags>(arr[PARAMST + 3]));
                draw = ImGui::GetWindowDrawList();


                break;
            case EndChild:
                ImGui::EndChild();
                break;
            case BeginGroup:
                ImGui::BeginGroup();
                break;
            case EndGroup:
                ImGui::EndGroup();
                break;
            case PushClipRect:
                draw->PushClipRect(any_cast<ImVec2>(arr[PARAMST]),
                                   any_cast<ImVec2>(arr[PARAMST+1]),
                                   any_cast<bool>(arr[PARAMST+2]));
                break;
            case SameLine:
                ImGui::SameLine();
                break;
            case Dummy:
                ImGui::Dummy(any_cast<ImVec2>(arr[PARAMST]));
                break;
            case SeparatorText:
                ImGui::SeparatorText(any_cast<std::string>(arr[PARAMST]).c_str());
                break;
            case Separator:
                ImGui::Separator();
                break;
            case PathClear:
                draw->PathClear();
                break;
            case PathArcTo:
                draw->PathArcTo(
                    any_cast<ImVec2>(arr[PARAMST]),
                    any_cast<float>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3]),
                    any_cast<int>(arr[PARAMST + 4])
                );
                break;
            case PathStroke:
                draw->PathStroke(
                    any_cast<ImU32>(arr[PARAMST]),
                    any_cast<float>(arr[PARAMST + 1])
                );
                break;
            case PathFillConvex:
                draw->PathFillConvex(any_cast<ImU32>(arr[PARAMST]));
                break;
            case AddCircleFilled:
                draw->AddCircleFilled(
                    any_cast<ImVec2>(arr[PARAMST]),
                    any_cast<float>(arr[PARAMST + 1]),
                    any_cast<ImU32>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]));
                break;
            case AddCircle:

                draw->AddCircle(
                    any_cast<ImVec2>(arr[PARAMST]),
                    any_cast<float>(arr[PARAMST + 1]),
                    any_cast<ImU32>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<float>(arr[PARAMST + 4]));
                break;
            case AddEllipse:
                draw->AddEllipse(
                    any_cast<ImVec2>(arr[PARAMST]),
                    any_cast<ImVec2>(arr[PARAMST + 1]),
                    any_cast<ImU32>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<float>(arr[PARAMST + 4]));
                break;


            case AddRect:
                draw->AddRect(
                    any_cast<ImVec2>(arr[PARAMST]),
                    any_cast<ImVec2>(arr[PARAMST + 1]),
                    any_cast<ImU32>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3]),
                    any_cast<ImDrawFlags>(arr[PARAMST + 4]),
                    any_cast<float>(arr[PARAMST + 5]));
                break;
            case AddRectFilled:
                draw->AddRectFilled(
                    any_cast<ImVec2>(arr[PARAMST]),
                    any_cast<ImVec2>(arr[PARAMST + 1]),
                    any_cast<ImU32>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3]),
                    any_cast<ImDrawFlags>(arr[PARAMST + 4]));
                break;
            case AddRectFilledMultiColor:
                draw->AddRectFilledMultiColor(
                    any_cast<ImVec2>(arr[PARAMST]),
                    any_cast<ImVec2>(arr[PARAMST + 1]),
                    any_cast<ImU32>(arr[PARAMST + 2]),
                    any_cast<ImU32>(arr[PARAMST + 3]),
                    any_cast<ImU32>(arr[PARAMST + 4]),
                    any_cast<ImU32>(arr[PARAMST + 5]));
                break;
            case AddLine:
                draw->AddLine(
                    any_cast<ImVec2>(arr[PARAMST + 0]),
                    any_cast<ImVec2>(arr[PARAMST + 1]),
                    any_cast<ImU32>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3])
                );
                break;
            case AddText:
                draw->AddText(
                    any_cast<ImFont*>(arr[PARAMST + 0]),
                    any_cast<float>(arr[PARAMST + 1]),
                    any_cast<ImVec2>(arr[PARAMST + 2]),
                    any_cast<ImU32>(arr[PARAMST + 3]),
                    any_cast<std::string>(arr[PARAMST + 4]).c_str(),
                    any_cast<char*>(arr[PARAMST + 5]),
                    any_cast<float>(arr[PARAMST + 6]),
                    any_cast<ImVec4*>(arr[PARAMST + 7])
                );
                break;
            case AddTriangle:
                draw->AddTriangle(
                    any_cast<ImVec2>(arr[PARAMST + 0]),
                    any_cast<ImVec2>(arr[PARAMST + 1]),
                    any_cast<ImVec2>(arr[PARAMST + 2]),
                    any_cast<ImU32>(arr[PARAMST + 3]),
                    any_cast<float>(arr[PARAMST + 4])
                );
                break;
            case AddTriangleFilled:
                draw->AddTriangleFilled(
                    any_cast<ImVec2>(arr[PARAMST + 0]),
                    any_cast<ImVec2>(arr[PARAMST + 1]),
                    any_cast<ImVec2>(arr[PARAMST + 2]),
                    any_cast<ImU32>(arr[PARAMST + 3])
                );
                break;
            case AddQuad:
                draw->AddQuad(
                    any_cast<ImVec2>(arr[PARAMST + 0]),
                    any_cast<ImVec2>(arr[PARAMST + 1]),
                    any_cast<ImVec2>(arr[PARAMST + 2]),
                    any_cast<ImVec2>(arr[PARAMST + 3]),
                    any_cast<ImU32>(arr[PARAMST + 4]),
                    any_cast<float>(arr[PARAMST + 5])
                );
                break;
            case AddQuadFilled:
                draw->AddQuadFilled(
                    any_cast<ImVec2>(arr[PARAMST + 0]),
                    any_cast<ImVec2>(arr[PARAMST + 1]),
                    any_cast<ImVec2>(arr[PARAMST + 2]),
                    any_cast<ImVec2>(arr[PARAMST + 3]),
                    any_cast<ImU32>(arr[PARAMST + 4])
                );
                break;
            case AddPolyline:
                draw->AddPolyline(
                    any_cast<ImVec2*>(arr[PARAMST + 0]),
                    any_cast<int>(arr[PARAMST + 1]),
                    any_cast<ImU32>(arr[PARAMST + 2]),
                    any_cast<bool>(arr[PARAMST + 3]),
                    any_cast<float>(arr[PARAMST + 4])
                );
                break;
            case AddConvexPolyFilled:
                draw->AddConvexPolyFilled(
                    any_cast<ImVec2*>(arr[PARAMST + 0]),
                    any_cast<int>(arr[PARAMST + 1]),
                    any_cast<ImU32>(arr[PARAMST + 2])
                );
                break;
                
            case PathEllipticalArcTo:
            {
                
                draw->PathEllipticalArcTo(
                    any_cast<ImVec2>(arr[PARAMST]),
                    any_cast<ImVec2>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3]),
                    any_cast<float>(arr[PARAMST + 4]),
                    any_cast<int>(arr[PARAMST + 5])
                );
            }
            break;
          

            case PathLineTo:
                draw->PathLineTo(any_cast<ImVec2>(arr[PARAMST]));
                break;
      
            case SetNextWindowPos:
                ImGui::SetNextWindowPos(
                    any_cast<ImVec2>(arr[PARAMST + 0]),
                    any_cast<int>(arr[PARAMST + 1]),
                    any_cast<ImVec2>(arr[PARAMST + 2])
                );
                break;
            case SetNextWindowSize:
                ImGui::SetNextWindowSize(
                    any_cast<ImVec2>(arr[PARAMST + 0]),
                    any_cast<int>(arr[PARAMST + 1])
                );
                break;
            case PushStyleColor:
                ImGui::PushStyleColor(
                    any_cast<ImGuiCol>(arr[PARAMST + 0]),
                    any_cast<ImVec4>(arr[PARAMST + 1])
                );
                break;
            case PopStyleColor:
                ImGui::PopStyleColor();
                break;

            case PushStyleVar_ImVec2:
                ImGui::PushStyleVar(
                    any_cast<ImGuiStyleVar>(arr[PARAMST + 0]),
                    any_cast<ImVec2>(arr[PARAMST + 1])
                );
                break;
            case PushStyleVar_Float:
                ImGui::PushStyleVar(
                    any_cast<ImGuiStyleVar>(arr[PARAMST + 0]),
                    any_cast<float>(arr[PARAMST + 1])
                );
                break;
            case PopStyleVar:
                ImGui::PopStyleVar();
                break;
            case NewLine:
                ImGui::NewLine();
                break;
            case Columns:
                ImGui::Columns(
                    1,
                    any_cast<std::string>(arr[PARAMST + 1]).c_str(),
                    any_cast<bool>(arr[PARAMST + 2])
                );
                break;

            case NextColumn:
                ImGui::NextColumn();
                break;

            case ColumnsEnd:
                ImGui::Columns(1);
                break;
            case Spacing:
                ImGui::Spacing();
                break;

            case AddImage:
            {
                ImTextureID texId = any_cast<ImTextureID>(arr[PARAMST]);
                ImVec2 p0 = any_cast<ImVec2>(arr[PARAMST + 1]);
                ImVec2 p1 = any_cast<ImVec2>(arr[PARAMST + 2]);
                ImVec2 uv0 = any_cast<ImVec2>(arr[PARAMST + 3]);
                ImVec2 uv1 = any_cast<ImVec2>(arr[PARAMST + 4]);
                ImU32 col = any_cast<ImU32>(arr[PARAMST + 5]);

                if ((uint64_t)texId != 0)
                {
                    draw->AddImage(texId, p0, p1, uv0, uv1, col);
                }
                else
                {
                    // 修正AddRect参数顺序：圆角=2, flags=0, 线宽=2
                    draw->AddRect(p0, p1, IM_COL32(0, 0, 0, 255), 2.f, 0, 2.f);
                    draw->AddRectFilled(p0, p1, IM_COL32(30, 30, 30, 200), 2.f);
                    ImVec2 textSize = ImGui::CalcTextSize("Image");
                    ImVec2 textPos = ImVec2(
                        (p0.x + p1.x - textSize.x) * 0.5f,
                        (p0.y + p1.y - textSize.y) * 0.5f
                    );
                    draw->AddText(nullptr, 14.f, textPos, IM_COL32_WHITE, "Image");
                }
                break;
            }
            case AddImageQuad:
            {
                ImTextureID texId = any_cast<ImTextureID>(arr[PARAMST]);
                ImVec2 p1 = any_cast<ImVec2>(arr[PARAMST + 1]);
                ImVec2 p2 = any_cast<ImVec2>(arr[PARAMST + 2]);
                ImVec2 p3 = any_cast<ImVec2>(arr[PARAMST + 3]);
                ImVec2 p4 = any_cast<ImVec2>(arr[PARAMST + 4]);
                ImVec2 uv1 = any_cast<ImVec2>(arr[PARAMST + 5]);
                ImVec2 uv2 = any_cast<ImVec2>(arr[PARAMST + 6]);
                ImVec2 uv3 = any_cast<ImVec2>(arr[PARAMST + 7]);
                ImU32 tint = any_cast<ImU32>(arr[PARAMST + 8]);

                if ((uint64_t)texId != 0)
                {
                    // 补齐最后一个tint颜色参数，匹配完整原型
                   // draw->AddImageQuad(texId, p1, p2, p3, p4, uv1, uv2, uv3, tint);
                }
                else
                {
                    ImVec2 quadPts[4] = { p1, p2, p3, p4 };
                    draw->AddConvexPolyFilled(quadPts, 4, IM_COL32(30, 30, 30, 200));
                    // 移除多余flags参数，兼容旧版ImGui
                    draw->AddPolyline(quadPts, 4, IM_COL32(0, 0, 0, 255), true, 2.f);

                    ImVec2 center = ImVec2(
                        (p1.x + p2.x + p3.x + p4.x) / 4.f,
                        (p1.y + p2.y + p3.y + p4.y) / 4.f
                    );
                    ImVec2 textSize = ImGui::CalcTextSize("ImageQuad");
                    ImVec2 textPos = ImVec2(center.x - textSize.x * 0.5f, center.y - textSize.y * 0.5f);
                    draw->AddText(nullptr, 14.f, textPos, IM_COL32_WHITE, "ImageQuad");
                }
                break;
            }

            case AddImageRounded:
            {
                ImTextureID texId = any_cast<ImTextureID>(arr[PARAMST]);
                ImVec2 p0 = any_cast<ImVec2>(arr[PARAMST + 1]);
                ImVec2 p1 = any_cast<ImVec2>(arr[PARAMST + 2]);
                ImVec2 uv0 = any_cast<ImVec2>(arr[PARAMST + 3]);
                ImVec2 uv1 = any_cast<ImVec2>(arr[PARAMST + 4]);
                float rounding = any_cast<float>(arr[PARAMST + 5]);
                ImU32 col = any_cast<ImU32>(arr[PARAMST + 6]);

                if ((uint64_t)texId != 0)
                {
                    draw->AddImageRounded(texId, p0, p1, uv0, uv1, rounding, col);
                }
                else
                {
                    draw->AddRectFilled(p0, p1, IM_COL32(30, 30, 30, 200), rounding);
                    // 圆角矩形边框，flags填0，线宽2
                    draw->AddRect(p0, p1, IM_COL32(0, 0, 0, 255), rounding, 0, 2.f);
                    ImVec2 textSize = ImGui::CalcTextSize("ImageRounded");
                    ImVec2 textPos = ImVec2(
                        (p0.x + p1.x - textSize.x) * 0.5f,
                        (p0.y + p1.y - textSize.y) * 0.5f
                    );
                    draw->AddText(nullptr, 14.f, textPos, IM_COL32_WHITE, "ImageRounded");
                }
                break;
            }

            case PathArcToFast:
                draw->PathArcToFast(
                    any_cast<ImVec2>(arr[PARAMST]),
                    any_cast<float>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3])
                );
                break;
            }
        

        }

        if (getLV.type() == typeid(LView)) {
            switch (any_cast<LView>(getLV))
            {
            case Text:
                ImGui::Text("%s", any_cast<std::string>(arr[PARAMST]).c_str());
                break;
            case TextWrapped:
                ImGui::TextWrapped("%s", any_cast<std::string>(arr[PARAMST]).c_str());
                break;
            case LabelText:
                ImGui::LabelText(any_cast<std::string>(arr[PARAMST]).c_str(),
                    any_cast<std::string>(arr[PARAMST + 1]).c_str());
                break;
            case BulletText:
                ImGui::BulletText("%s", any_cast<std::string>(arr[PARAMST]).c_str());
                break;
            case Bullet:
                ImGui::Bullet();
                break;

            case TextDisabled:
                ImGui::TextDisabled("%s", any_cast<std::string>(arr[PARAMST]).c_str());
                break;

            case Button:
                ImGui::Button(any_cast<std::string>(arr[PARAMST]).c_str(),
                    any_cast<ImVec2>(arr[PARAMST + 1]));
                break;
            case SmallButton:
                ImGui::SmallButton(any_cast<std::string>(arr[PARAMST]).c_str());
                break;
            case InvisibleButton:
                ImGui::InvisibleButton(any_cast<std::string>(arr[PARAMST]).c_str(),
                    any_cast<ImVec2>(arr[PARAMST + 1]));
                break;

            case Checkbox:
                ImGui::Checkbox(any_cast<std::string>(arr[PARAMST]).c_str(),
                    any_cast<bool>(&arr[PARAMST + 1]));
                break;

            case CheckboxFlags:
                ImGui::CheckboxFlags(any_cast<std::string>(arr[PARAMST]).c_str(),
                    any_cast<int>(&arr[PARAMST + 1]),
                    any_cast<int>(arr[PARAMST + 2]));
                break;

            case RadioButtonEx:
                ImGui::RadioButton(any_cast<std::string>(arr[PARAMST]).c_str(),
                    any_cast<int>(&arr[PARAMST + 1]),
                    any_cast<int>(arr[PARAMST + 2]));
                break;

            case InputText:
                ImGui::InputText(any_cast<std::string>(arr[PARAMST]).c_str(),
                    const_cast<char*>(any_cast<std::string>(arr[PARAMST + 1]).c_str()),
                    any_cast<int>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<ImGuiInputTextCallback>(arr[PARAMST + 4]),
                    any_cast<void*>(arr[PARAMST + 5]));
                break;
            case InputTextMultiline:
                ImGui::InputTextMultiline(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<char*>(arr[PARAMST + 1]),
                    any_cast<size_t>(arr[PARAMST + 2]),
                    any_cast<ImVec2>(arr[PARAMST + 3]),
                    any_cast<int>(arr[PARAMST + 4]),
                    any_cast<ImGuiInputTextCallback>(arr[PARAMST + 5]),
                    any_cast<void*>(arr[PARAMST + 6])
                );
                break;
            case InputInt:
                ImGui::InputInt(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<int*>(arr[PARAMST + 1]),
                    any_cast<int>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<ImGuiInputTextFlags>(arr[PARAMST + 4])
                );
                break;
            case InputFloat:
                ImGui::InputFloat(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3]),
                    any_cast<std::string>(arr[PARAMST + 4]).c_str(),
                    any_cast<ImGuiInputTextFlags>(arr[PARAMST + 5])
                );
                break;
            case InputFloat3:
                ImGui::InputFloat3(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<std::string>(arr[PARAMST + 2]).c_str(),
                    any_cast<ImGuiInputTextFlags>(arr[PARAMST + 3])
                );
                break;
            case InputFloat4:
                ImGui::InputFloat4(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<std::string>(arr[PARAMST + 2]).c_str(),
                    any_cast<ImGuiInputTextFlags>(arr[PARAMST + 3])
                );
                break;
            case InputInt2:
                ImGui::InputInt2(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<int*>(arr[PARAMST + 1]),
                    any_cast<ImGuiInputTextFlags>(arr[PARAMST + 2])
                );
                break;
            case InputInt3:
                ImGui::InputInt3(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<int*>(arr[PARAMST + 1]),
                    any_cast<ImGuiInputTextFlags>(arr[PARAMST + 2])
                );
                break;
            case InputInt4:
                ImGui::InputInt4(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<int*>(arr[PARAMST + 1]),
                    any_cast<ImGuiInputTextFlags>(arr[PARAMST + 2])
                );
                break;
            case InputDouble:
                ImGui::InputDouble(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<double*>(arr[PARAMST + 1]),
                    any_cast<double>(arr[PARAMST + 2]),
                    any_cast<double>(arr[PARAMST + 3]),
                    any_cast<std::string>(arr[PARAMST + 4]).c_str(),
                    any_cast<ImGuiInputTextFlags>(arr[PARAMST + 5])
                );
                break;

            case SliderInt:
                ImGui::SliderInt(
                    any_cast<std::string>(arr[PARAMST]).c_str(),
                    any_cast<int*>(arr[PARAMST + 1]),
                    any_cast<int>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<std::string>(arr[PARAMST + 4]).c_str()
                );
                break;
            case SliderFloat:
                ImGui::SliderFloat(
                    any_cast<std::string>(arr[PARAMST]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3]),
                    any_cast<std::string>(arr[PARAMST + 4]).c_str()
                );
                break;
            case SliderInt2:
                ImGui::SliderInt2(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<int*>(arr[PARAMST + 1]),
                    any_cast<int>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<std::string>(arr[PARAMST + 4]).c_str(),
                    any_cast<int>(arr[PARAMST + 5])
                );
                break;
            case SliderInt3:
                ImGui::SliderInt3(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<int*>(arr[PARAMST + 1]),
                    any_cast<int>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<std::string>(arr[PARAMST + 4]).c_str(),
                    any_cast<int>(arr[PARAMST + 5])
                );
                break;
            case SliderInt4:
                ImGui::SliderInt4(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<int*>(arr[PARAMST + 1]),
                    any_cast<int>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<std::string>(arr[PARAMST + 4]).c_str(),
                    any_cast<int>(arr[PARAMST + 5])
                );
                break;
            case SliderFloat2:
                ImGui::SliderFloat2(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3]),
                    any_cast<std::string>(arr[PARAMST + 4]).c_str(),
                    any_cast<int>(arr[PARAMST + 5])
                );
                break;
            case SliderFloat3:
                ImGui::SliderFloat3(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3]),
                    any_cast<std::string>(arr[PARAMST + 4]).c_str(),
                    any_cast<int>(arr[PARAMST + 5])
                );
                break;
            case SliderFloat4:
                ImGui::SliderFloat4(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3]),
                    any_cast<std::string>(arr[PARAMST + 4]).c_str(),
                    any_cast<int>(arr[PARAMST + 5])
                );
                break;

            case VSliderInt:
                ImGui::VSliderInt(
                    any_cast<std::string>(arr[PARAMST]).c_str(),
                    any_cast<ImVec2>(arr[PARAMST + 1]),
                    any_cast<int*>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<int>(arr[PARAMST + 4]),
                    any_cast<std::string>(arr[PARAMST + 5]).c_str()
                );
                break;

            case VSliderFloat:
                ImGui::VSliderFloat(
                    any_cast<std::string>(arr[PARAMST]).c_str(),
                    any_cast<ImVec2>(arr[PARAMST + 1]),
                    any_cast<float*>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3]),
                    any_cast<float>(arr[PARAMST + 4]),
                    any_cast<std::string>(arr[PARAMST + 5]).c_str()
                );
                break;
            case Combo:
                ImGui::Combo(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<int*>(arr[PARAMST + 1]),
                    any_cast<char**>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<int>(arr[PARAMST + 4])
                );
                break;
            case TreeNode:
                ImGui::TreeNode(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str()
                );
                break;
            case TreeNodeEx:
                ImGui::TreeNodeEx(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<ImGuiTreeNodeFlags>(arr[PARAMST + 1])
                );
                break;
            case CollapsingHeader:
                ImGui::CollapsingHeader(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<int>(arr[PARAMST + 1])
                );
                break;
            case ProgressBar:
                ImGui::ProgressBar(
                    any_cast<float>(arr[PARAMST + 0]),
                    any_cast<ImVec2>(arr[PARAMST + 1]),
                    any_cast<std::string>(arr[PARAMST + 2]).c_str()
                );
                break;
            case DragInt:
                ImGui::DragInt(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<int*>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<int>(arr[PARAMST + 4]),
                    any_cast<std::string>(arr[PARAMST + 5]).c_str(),
                    any_cast<int>(arr[PARAMST + 6])
                );
                break;
            case DragInt2:
                ImGui::DragInt2(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<int*>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<int>(arr[PARAMST + 4]),
                    any_cast<std::string>(arr[PARAMST + 5]).c_str(),
                    any_cast<int>(arr[PARAMST + 6])
                );
                break;
            case DragInt3:
                ImGui::DragInt3(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<int*>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<int>(arr[PARAMST + 4]),
                    any_cast<std::string>(arr[PARAMST + 5]).c_str(),
                    any_cast<int>(arr[PARAMST + 6])
                );
                break;
            case DragInt4:
                ImGui::DragInt4(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<int*>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<int>(arr[PARAMST + 4]),
                    any_cast<std::string>(arr[PARAMST + 5]).c_str(),
                    any_cast<int>(arr[PARAMST + 6])
                );
                break;
            case DragFloat:
                ImGui::DragFloat(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3]),
                    any_cast<float>(arr[PARAMST + 4]),
                    any_cast<std::string>(arr[PARAMST + 5]).c_str(),
                    any_cast<int>(arr[PARAMST + 6])
                );
                break;
            case DragFloat2:
                ImGui::DragFloat2(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3]),
                    any_cast<float>(arr[PARAMST + 4]),
                    any_cast<std::string>(arr[PARAMST + 5]).c_str(),
                    any_cast<int>(arr[PARAMST + 6])
                );
                break;
            case DragFloat3:
                ImGui::DragFloat3(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3]),
                    any_cast<float>(arr[PARAMST + 4]),
                    any_cast<std::string>(arr[PARAMST + 5]).c_str(),
                    any_cast<int>(arr[PARAMST + 6])
                );
                break;
            case DragFloat4:
                ImGui::DragFloat4(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<float>(arr[PARAMST + 2]),
                    any_cast<float>(arr[PARAMST + 3]),
                    any_cast<float>(arr[PARAMST + 4]),
                    any_cast<std::string>(arr[PARAMST + 5]).c_str(),
                    any_cast<int>(arr[PARAMST + 6])
                );
                break;
            case ColorEdit4:
                ImGui::ColorEdit4(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<int>(arr[PARAMST + 2])
                );
                break;
            case ColorEdit3:
                ImGui::ColorEdit3(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<int>(arr[PARAMST + 2])
                );
                break;
            case ColorPicker4:
                ImGui::ColorPicker4(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<int>(arr[PARAMST + 2]),
                    any_cast<float*>(arr[PARAMST + 3])
                );
                break;
            case ColorPicker3:
                ImGui::ColorPicker3(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<int>(arr[PARAMST + 2])
                );
                break;
            case PlotLines:
                ImGui::PlotLines(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<int>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<std::string>(arr[PARAMST + 4]).c_str(),
                    any_cast<float>(arr[PARAMST + 5]),
                    any_cast<float>(arr[PARAMST + 6]),
                    any_cast<ImVec2>(arr[PARAMST + 7]),
                    any_cast<int>(arr[PARAMST + 8])
                );
                break;
            case PlotHistogram:
                ImGui::PlotHistogram(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<float*>(arr[PARAMST + 1]),
                    any_cast<int>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<std::string>(arr[PARAMST + 4]).c_str(),
                    any_cast<float>(arr[PARAMST + 5]),
                    any_cast<float>(arr[PARAMST + 6]),
                    any_cast<ImVec2>(arr[PARAMST + 7]),
                    any_cast<int>(arr[PARAMST + 8])
                );
                break;
            case ListBox:
                ImGui::ListBox(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<int*>(arr[PARAMST + 1]),
                    any_cast<char**>(arr[PARAMST + 2]),
                    any_cast<int>(arr[PARAMST + 3]),
                    any_cast<int>(arr[PARAMST + 4])
                );
                break;

            case MenuItem:
                ImGui::MenuItem(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str(),
                    any_cast<char*>(arr[PARAMST + 1]),
                    any_cast<bool*>(arr[PARAMST + 2]),
                    any_cast<bool>(arr[PARAMST + 3])
                );
                break;
            case SetTooltip:
                ImGui::SetTooltip(
                    any_cast<std::string>(arr[PARAMST + 0]).c_str()
                );
                break;

            }

        }


    }
    return 1;
}


