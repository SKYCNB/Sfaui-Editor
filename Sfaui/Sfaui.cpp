#include "Sfaui.h"
#include "TextureLoader.h"
//#include "Sfaui_value.h"
using namespace nonstd;


//read project
#if defined(_WIN32) || defined(_WIN64)
#include <d3d11.h>
#endif

//full the mRNA
#define SFAUI_PROJ_MAGIC 0x53464155
#define SFAUI_PROJ_VER   1
std::multimap<std::string, const std::vector<any>*> g_FuncReverseMap;
static void BuildReverse_tRNA()
{
    g_FuncReverseMap.clear();

    if (!g_FuncReverseMap.empty())
        return;

    for (auto& entry : ViewList_tRNA)
    {
        auto& tplArr = entry.second;
        if (tplArr.size() <= FUNCTIONNMAE)
            continue;
        std::string func = any_cast<std::string>(tplArr[FUNCTIONNMAE]);
        g_FuncReverseMap.insert({ func, &tplArr });
    }
    for (auto& entry : LaoutList_tRNA)
    {
        auto& tplArr = entry.second;
        if (tplArr.size() <= FUNCTIONNMAE)
            continue;
        std::string func = any_cast<std::string>(tplArr[FUNCTIONNMAE]);
        g_FuncReverseMap.insert({ func, &tplArr });
    }
}


static void FillPointerFromTemplate(std::vector<std::array<any, 20>>& target_mRNA)
{
    for (auto& row : target_mRNA)
    {
        std::string targetFunc = any_cast<std::string>(row[FUNCTIONNMAE]);
        AnyType rowVLEnumType = GetAnyType(row[VLENUM]);
        int rowParamCnt = any_cast<int>(row[PARAMNUM]);

        auto range = g_FuncReverseMap.equal_range(targetFunc);
        const std::vector<any>* matchTpl = nullptr;

        for (auto it = range.first; it != range.second; ++it)
        {
            const auto* tpl = it->second;
            AnyType tplVLEnumType = GetAnyType((*tpl)[VLENUM]);
            int tplParamCnt = any_cast<int>((*tpl)[PARAMNUM]);

            if (tplVLEnumType == rowVLEnumType && tplParamCnt == rowParamCnt)
            {
                matchTpl = tpl;
                break;
            }
        }

        if (!matchTpl) continue;
        const auto& tpl = *matchTpl;
        int paramCount = rowParamCnt;
        for (int p = 0; p < paramCount; p++)
        {
            int cellIdx = PARAMST + p;
            AnyType cellTp = GetAnyType(row[cellIdx]);
            if (cellTp == AnyType_Empty)
            {
                row[cellIdx] = tpl[cellIdx];
            }
        }
    }
}


static any ReadAny_FromProject(FILE* fp)
{
    AnyType t = AnyType_Empty;
    fread(&t, sizeof(AnyType), 1, fp);
    switch (t)
    {
    case AnyType_Empty:
    case AnyType_Unknown:
        return nonstd::any{};
    case AnyType_Int:
    {
        int v;
        fread(&v, sizeof(int), 1, fp);
        return v;
    }
    case AnyType_Float:
    {
        float v;
        fread(&v, sizeof(float), 1, fp);
        return v;
    }
    case AnyType_Double:
    {
        double v;
        fread(&v, sizeof(double), 1, fp);
        return v;
    }
    case AnyType_Bool:
    {
        bool v;
        fread(&v, sizeof(bool), 1, fp);
        return v;
    }
    case AnyType_String:
    {
        int len;
        fread(&len, sizeof(int), 1, fp);
        std::vector<char> buf(len);
        fread(buf.data(), 1, len, fp);
        return std::string(buf.begin(), buf.end());
    }
    case AnyType_ImVec2:
    {
        ImVec2 v;
        fread(&v.x, sizeof(float), 1, fp);
        fread(&v.y, sizeof(float), 1, fp);
        return v;
    }
    case AnyType_ImVec4:
    {
        ImVec4 v;
        fread(&v.x, sizeof(float), 1, fp);
        fread(&v.y, sizeof(float), 1, fp);
        fread(&v.z, sizeof(float), 1, fp);
        fread(&v.w, sizeof(float), 1, fp);
        return v;
    }
    case AnyType_ImU32:
    {
        ImU32 v;
        fread(&v, sizeof(ImU32), 1, fp);
        return v;
    }

    case AnyType_LView:
    {
        int val;
        fread(&val, sizeof(int), 1, fp);
        if (val < 0 || val >(int)FINALLYVIEW)
        {
            Sfaui_log("Load err: invalid LView val %d", val);
            return any{};
        }
        return static_cast<LView>(val);
    }
    case AnyType_LLayout:
    {
        int val;
        fread(&val, sizeof(int), 1, fp);
        if (val < 0 || val >(int)FINALLYLAYOUT)
        {
            Sfaui_log("Load err: invalid LLayout val %d", val);
            return any{};
        }
        return static_cast<LLayout>(val);
    }
    default:
        return nonstd::any{};
    }
}



bool Sfaui_loadProject(const char* filePath)
{
    Sfaui_log("====== Sfaui_loadProject Start ======");
    FILE* fp = fopen(filePath, "rb");
    if (!fp)
    {
        Sfaui_log("LoadBinary fail: cannot open %s", filePath);
        return false;
    }
    Sfaui_log("Open file success: %s", filePath);

    uint32_t magic, ver;
    fread(&magic, sizeof(uint32_t), 1, fp);
    fread(&ver, sizeof(uint32_t), 1, fp);
    Sfaui_log("Magic:0x%X, Version:%d", magic, ver);

    if (magic != SFAUI_PROJ_MAGIC || ver > SFAUI_PROJ_VER)
    {
        Sfaui_log("LoadBinary fail: magic/version mismatch");
        fclose(fp);
        return false;
    }

    int totalRow = 0;
    fread(&totalRow, sizeof(int), 1, fp);
    Sfaui_log("Total UI Row Count: %d", totalRow);

    int errAnyCount = 0;
    Help_mRNA* temp = new Help_mRNA;

    Sfaui_log("Step1: Start read All_mRNA data...");
    for (int r = 0; r < totalRow; r++)
    {
        std::array<any, 20> tempRow{};
        for (int c = 0; c < 20; c++)
        {
            tempRow[c] = ReadAny_FromProject(fp);
            AnyType tp = GetAnyType(tempRow[c]);
            if (tp == AnyType_Unknown) errAnyCount++;
        }
        (*temp).mRNA.push_back(tempRow);
    }
    Sfaui_log("finish: All any parse finish, unknown/pointer cell count: %d", errAnyCount);

    Sfaui_log("Start read LVBuild / WasChose / Visibility array...");
    (*temp).LVBuild.resize(totalRow);
    (*temp).WasChose.resize(totalRow);
    (*temp).Visibility.resize(totalRow);
    fread((*temp).LVBuild.data(), sizeof(int), totalRow, fp);
    fread((*temp).WasChose.data(), sizeof(char), totalRow, fp);
    fread((*temp).Visibility.data(), sizeof(char), totalRow, fp);
    Sfaui_log("finish: Read three vector finish");

    int checkRow = 0;
    fread(&checkRow, sizeof(int), 1, fp);
    Sfaui_log("Footer checkRow:%d, header totalRow:%d", checkRow, totalRow);
    fclose(fp);
    Sfaui_log("File closed");

    if (checkRow != totalRow)
    {
        Sfaui_log("LoadBinary warn: file truncated, data incomplete!");
        return false;
    }

    Sfaui_log("Start fill pointer template...");
    FillPointerFromTemplate((*temp).mRNA);
    Sfaui_log("finish: Fill pointer template finish");

    Sfaui_log("Clear global four containers");
    All_mRNA.clear();
    LVBuild.clear();
    WasChose.clear();
    Visibility.clear();

    Sfaui_log("Assign temp data to global All_mRNA");
    All_mRNA.swap((*temp).mRNA);

    ListUi_NowEnd = All_mRNA.size();

    Sfaui_log("Assign temp_LVBuild(int) to global LVBuild(int)");
    LVBuild.assign((*temp).LVBuild.begin(), (*temp).LVBuild.end());

    Sfaui_log("Assign temp_WasChose(char) to global WasChose(char)");
    WasChose.assign((*temp).WasChose.begin(), (*temp).WasChose.end());

    Sfaui_log("Assign temp_Visibility(char) to global Visibility(char)");
    Visibility.assign((*temp).Visibility.begin(), (*temp).Visibility.end());

    Sfaui_log("Binary load success, total rows: %d", totalRow);
    Sfaui_log("====== Sfaui_loadProject End ======\n");
    return true;
}

//read setting
static bool LoadSet() {
    FILE* fp = fopen("./Sfaui.config", "rb");
    if (!fp)
    {
        return false;
    }
    fread(&g_SettingConfig, sizeof(SfauiSettingConfig), 1, fp);
    return true;
}


SfauiSettingConfig& cfgi = g_SettingConfig;
mRNAHistoryManager& histman = g_HistoryMgr;
//放在imgui init 后，放在while前
bool Sfaui_init(std::string file) {

    if (!LoadSet())
        Sfaui_log("load set fail");

#if defined(_WIN32) || defined(_WIN64)
    
#endif
    Sfaui_log("initting\n");
    //加载图片
    RegTex("deletey", deletey, deletey_w, deletey_h);
    RegTex("copyy", copyy, copyy_w, copyy_h);
    RegTex("allwhitey", allwhitey, allwhitey_w, allwhitey_h);
    RegTex("inveri", inveri, inveri_w, inveri_h);
    RegTex("offwhite", offwhite, offwhite_w, offwhite_h);
    RegTex("setim", setim, setim_w, setim_h);
    RegTex("puckup", puckup, puckup_w, puckup_h);
    RegTex("content_duplicate_white", content_duplicate_white, content_duplicate_white_w, content_duplicate_white_h);
    RegTex("ic_dedo", ic_dedo, ic_dedo_w, ic_dedo_h);
    RegTex("ic_undo", ic_undo, ic_undo_w, ic_undo_h);
    RegTex("remove_eye_white", remove_eye_white, remove_eye_white_w, remove_eye_white_h);
    RegTex("menu_white", menu_white, menu_white_w, menu_white_h);

    undo.push(*(new Help_mRNA));

    BuildReverse_tRNA();


    if (Sfaui_loadProject(file.c_str())) {
        strncpy(Sfaui_SavePathBuf, file.c_str(), 511);
        Sfaui_SavePathBuf[511] = '\0'; 
    }
    else
        Sfaui_log("load project fail, had created a new project");
  

    if(!cfgi.buttoniffollowfont) ButtonSIZE = ImVec2(cfgi.ButtonSIZE.x, cfgi.ButtonSIZE.x);
    else  ButtonSIZE = ImVec2(-1, 0);

    IMAGESIZE = ImVec2(cfgi.IMAGESIZE.x, cfgi.IMAGESIZE.x);

    histman.MAX_HISTORY_STEP = cfgi.undoMaxCount;
    Sfaui_log("init finish\n");
    return true;
}

//放在循环里面，用于显示编辑器
bool Sfaui_editing() {
    adjust_scren();

    bool useCustomStyle = !g_SettingConfig.followProjectStyle;
   
    int pushVarCount = 0;
    int pushColorCount = 0;
    if (useCustomStyle)
    {
        // 圆角
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, cfgi.WindowRounding);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, cfgi.ChildRounding);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, cfgi.FrameRounding);
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, cfgi.PopupRounding);

        // 边框
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, cfgi.WindowBorderSize);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, cfgi.FrameBorderSize);
        ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, cfgi.PopupBorderSize);

        // 边距间距
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, cfgi.WindowPadding);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, cfgi.FramePadding);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, cfgi.ItemSpacing);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, cfgi.ItemInnerSpacing);

        // 配色
        ImGui::PushStyleColor(ImGuiCol_WindowBg, cfgi.col_WindowBg);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, cfgi.col_FrameBg);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, cfgi.col_FrameBgHovered);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, cfgi.col_FrameBgActive);
        ImGui::PushStyleColor(ImGuiCol_Button, cfgi.col_Button);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, cfgi.col_ButtonHovered);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, cfgi.col_ButtonActive);
        ImGui::PushStyleColor(ImGuiCol_Header, cfgi.col_Header);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, cfgi.col_HeaderHovered);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, cfgi.col_HeaderActive);
        ImGui::PushStyleColor(ImGuiCol_Text, cfgi.col_Text);

        pushVarCount = 11;
        pushColorCount = 11;
    }
    else
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, ImGui::GetStyle().FrameRounding);
        pushVarCount = 1;
    }

   
    Sfaui_DrawLaout();
    if (!cfgi.drawuifollowProjectStyle)
    {
        DrawNewUi();
    }
    Windows_Ui();
    if (cfgi.showLogWindow)
        log("s");

    
    if (useCustomStyle)
    {
        ImGui::PopStyleColor(pushColorCount);
    }
    ImGui::PopStyleVar(pushVarCount);

    // 只在样式处理完成后执行一次
    if (cfgi.drawuifollowProjectStyle)
    {
        DrawNewUi();
    }

    return true;
}

//放在imgui shutdown后
bool Sfaui_shutdown() {
    ClearAllTex();
    return true;
}