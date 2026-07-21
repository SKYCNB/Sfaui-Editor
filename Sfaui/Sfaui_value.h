#ifndef SFAUI_VALUE_H
#define SFAUI_VALUE_H

#include <vector>
#include <stack>
#include <array>
#include "any.hpp"
#include "imgui/imgui.h"
#include "TextureLoader.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <map>


//存放全局变量的文件 File for storing global variables
#define SFAUIED_VESION 1.0

#define VLENUM  0
#define PAIR  1
#define  POS 2
#define RANGE  3
#define REALID  4
#define PARAMNUM  5
#define FUNCTIONNMAE  6
#define PARAMST  7

typedef struct Help_mRNA {
    std::vector<std::array<nonstd::any,20>> mRNA;
    std::vector<int> LVBuild;
    std::vector<char> WasChose;
    std::vector<char> Visibility;
} Help_mRNA;


//整个项目最重要的四大数组 The four most important arrays of the entire project
extern std::vector<std::array<nonstd::any,20>> All_mRNA; //Ui data
extern std::vector<int> LVBuild; //Ui flood
extern std::vector<char> WasChose; //if is chhose
extern std::vector<char> Visibility; //ui Visibility,for draw new ui

extern bool ifundo;
extern bool ifredo;
extern std::stack<Help_mRNA> dedo;
extern std::stack<Help_mRNA> undo;
void New_DUdo();
void To_undo();
void Help_mRNA_to_full(Help_mRNA& t);
void Make_undo();
void Make_dedo();




void CopyGlobalToSnap(Help_mRNA& snap);
void ApplySnapToGlobal(const Help_mRNA& snap);


class mRNAHistoryManager
{
public:
     int MAX_HISTORY_STEP = 200;
    void SaveSnapshot()
    {
        m_redoStack.clear();
        if (m_undoStack.size() >= MAX_HISTORY_STEP)
            m_undoStack.erase(m_undoStack.begin());
        Help_mRNA snap;
        CopyGlobalToSnap(snap);
        m_undoStack.push_back(std::move(snap));
    }
    bool Undo()
    {
        if (!CanUndo()) return false;
        Help_mRNA curSnap;
        CopyGlobalToSnap(curSnap);
        m_redoStack.push_back(std::move(curSnap));
        Help_mRNA target = m_undoStack.back();
        m_undoStack.pop_back();
        ApplySnapToGlobal(target);
        return true;
    }
    bool Redo()
    {
        if (!CanRedo()) return false;
        Help_mRNA curSnap;
        CopyGlobalToSnap(curSnap);
        m_undoStack.push_back(std::move(curSnap));
        Help_mRNA target = m_redoStack.back();
        m_redoStack.pop_back();
        ApplySnapToGlobal(target);
        return true;
    }
    void ClearAllHistory() { m_undoStack.clear(); m_redoStack.clear(); }
    bool CanUndo() const { return !m_undoStack.empty(); }
    bool CanRedo() const { return !m_redoStack.empty(); }
private:
    std::vector<Help_mRNA> m_undoStack;
    std::vector<Help_mRNA> m_redoStack;
};
extern mRNAHistoryManager g_HistoryMgr;


void AppendWidgetData(int lv, char chose, char visible);


ImVec2 FindFlood(int start, int end, int floodf);

//image size
extern ImVec2 IMAGESIZE;
extern ImVec2 ButtonSIZE;

//frame dcreen size
extern float SFAUI_SW;
extern float SFAUI_SH;

void adjust_scren();

//log
extern std::string Sfaui_log_path;
extern std::string Sfaui_log_test;
std::string Sfaui_log(const char* fmt, ...);


//set
extern char  Sfaui_SavePathBuf[512];
extern char  Sfaui_WriteCodePathBuf[512];
class SfauiSettingConfig
{
public:
    // 基础设置
    bool UseScreen_SetImvec2 = true;
    bool showLogWindow = false;
    bool showGrid = false;
    bool snapGrid = false;
    float gridSize = 10.0f;
    int undoMaxCount = 50;

    // 总开关：跟随项目全局样式
    bool followProjectStyle = false;
    bool drawuifollowProjectStyle = true;
    float alphaGlobal = 1.0f;

    // 圆角配置
    float WindowRounding = 12.0f;
    float ChildRounding = 10.0f;
    float FrameRounding = 8.0f;
    float PopupRounding = 10.0f;

    // 边框粗细
    float WindowBorderSize = 1.0f;
    float FrameBorderSize = 1.0f;
    float PopupBorderSize = 1.0f;

    // 间距边距
    ImVec2 WindowPadding   = ImVec2(15.0f, 15.0f);
    ImVec2 FramePadding    = ImVec2(10.0f, 8.0f);
    ImVec2 ItemSpacing     = ImVec2(12.0f, 12.0f);
    ImVec2 ItemInnerSpacing= ImVec2(6.0f, 6.0f);

    // 界面配色
    ImVec4 col_WindowBg        = ImVec4(0.12f, 0.14f, 0.18f, 1.00f);
    ImVec4 col_FrameBg         = ImVec4(0.18f, 0.20f, 0.26f, 1.00f);
    ImVec4 col_FrameBgHovered  = ImVec4(0.25f, 0.28f, 0.36f, 1.00f);
    ImVec4 col_FrameBgActive   = ImVec4(0.30f, 0.34f, 0.44f, 1.00f);
    ImVec4 col_Button          = ImVec4(0.20f, 0.24f, 0.32f, 1.00f);
    ImVec4 col_ButtonHovered   = ImVec4(0.28f, 0.34f, 0.46f, 1.00f);
    ImVec4 col_ButtonActive    = ImVec4(0.35f, 0.42f, 0.56f, 1.00f);
    ImVec4 col_Header          = ImVec4(0.16f, 0.18f, 0.22f, 1.00f);
    ImVec4 col_HeaderHovered   = ImVec4(0.22f, 0.26f, 0.34f, 1.00f);
    ImVec4 col_HeaderActive    = ImVec4(0.28f, 0.32f, 0.42f, 1.00f);
    ImVec4 col_Text            = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);

    //大小
    //size
    ImVec2 IMAGESIZE = ImVec2(20, 20);
    bool buttoniffollowfont = true;
    ImVec2 ButtonSIZE = ImVec2(-1, 70);

    // 层级列表设置
    float listItemHeight = 60.0f;
    bool showItemIndex = true;
    bool showItemId = true;
    bool enableAnim = true;

    // 导出保存设置
    bool exportWithComment = true;
    bool exportIndent = true;
    int exportIndentSpace = 4;

    // 调试安全设置
    bool enableSafeCheck = true;
    bool enableLogWarning = true;
    bool enableLogError = true;

    // 一键重置全部默认值
    void ResetAll()
    {
        UseScreen_SetImvec2 = true;
        showLogWindow = true;
        showGrid = false;
        snapGrid = false;
        gridSize = 10.0f;
        undoMaxCount = 50;

        followProjectStyle = false;
        drawuifollowProjectStyle = true;
        alphaGlobal = 1.0f;

        WindowRounding = 12.0f;
        ChildRounding = 10.0f;
        FrameRounding = 8.0f;
        PopupRounding = 10.0f;

        WindowBorderSize = 1.0f;
        FrameBorderSize = 1.0f;
        PopupBorderSize = 1.0f;

        WindowPadding   = ImVec2(15.0f, 15.0f);
        FramePadding    = ImVec2(10.0f, 8.0f);
        ItemSpacing     = ImVec2(12.0f, 12.0f);
        ItemInnerSpacing= ImVec2(6.0f, 6.0f);

        col_WindowBg        = ImVec4(0.12f, 0.14f, 0.18f, 1.00f);
        col_FrameBg         = ImVec4(0.18f, 0.20f, 0.26f, 1.00f);
        col_FrameBgHovered  = ImVec4(0.25f, 0.28f, 0.36f, 1.00f);
        col_FrameBgActive   = ImVec4(0.30f, 0.34f, 0.44f, 1.00f);
        col_Button          = ImVec4(0.20f, 0.24f, 0.32f, 1.00f);
        col_ButtonHovered   = ImVec4(0.28f, 0.34f, 0.46f, 1.00f);
        col_ButtonActive    = ImVec4(0.35f, 0.42f, 0.56f, 1.00f);
        col_Header          = ImVec4(0.16f, 0.18f, 0.22f, 1.00f);
        col_HeaderHovered   = ImVec4(0.22f, 0.26f, 0.34f, 1.00f);
        col_HeaderActive    = ImVec4(0.28f, 0.32f, 0.42f, 1.00f);
        col_Text            = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);

        IMAGESIZE = ImVec2(20, 20);
        buttoniffollowfont = true;
        ButtonSIZE = ImVec2(-1, 70);

        listItemHeight = 60.0f;
        showItemIndex = true;
        showItemId = true;
        enableAnim = true;

        exportWithComment = true;
        exportIndent = true;
        exportIndentSpace = 4;

        enableSafeCheck = true;
        enableLogWarning = true;
        enableLogError = true;
    }
};



extern SfauiSettingConfig g_SettingConfig;

//writeany project
bool SaveProjectBinary(const char* filePath);

#endif
