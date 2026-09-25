#include "Sfaui_value.h"
#include "Sfaui_fit.h"
#include "Sfaui_fitType.h"
#include "Sfaui_fitEnum.h"

using namespace nonstd;


SfauiSettingConfig g_SettingConfig;

std::vector<std::array<any,20>> All_mRNA;
std::vector<int> LVBuild;
std::vector<char> WasChose;
std::vector<char> Visibility;

bool ifundo=false;
bool ifredo = false;
std::stack<Help_mRNA>dedo;
std::stack<Help_mRNA>undo;

ImVec2 IMAGESIZE = ImVec2(20, 20);
ImVec2 ButtonSIZE = ImVec2(-1, 70);

void Make_undo()
{
    if(!dedo.empty())
    {
        while (!dedo.empty())
            dedo.pop();
    }
    Help_mRNA *snap = new Help_mRNA;
    snap->mRNA = All_mRNA;
    snap->LVBuild = LVBuild;
    snap->WasChose = WasChose;
    snap->Visibility = Visibility;
    undo.push(*snap);
}

void Make_dedo()
{
    if (undo.size()<=1) return;
    Help_mRNA *cur = &undo.top();
    dedo.push(*cur);
    undo.pop();
}

void New_DUdo()
{

    All_mRNA=undo.top().mRNA;
    LVBuild=undo.top().LVBuild;
    WasChose=undo.top().WasChose;
    Visibility=undo.top().Visibility;
}

void To_undo()
{
    if(dedo.empty())
        return;

    undo.push(dedo.top());
    dedo.pop();
}

void  Help_mRNA_to_full(Help_mRNA &t) {

}



void CopyGlobalToSnap(Help_mRNA& snap)
{
    snap.mRNA = All_mRNA;
    snap.LVBuild = LVBuild;
    snap.WasChose = WasChose;
    snap.Visibility = Visibility;
}
void ApplySnapToGlobal(const Help_mRNA& snap)
{
    All_mRNA = snap.mRNA;
    LVBuild = snap.LVBuild;
    WasChose = snap.WasChose;
    Visibility = snap.Visibility;
}


mRNAHistoryManager g_HistoryMgr;





void AppendWidgetData(int lv = 0, char chose = 1, char visible = 1)
{
    std::array<nonstd::any, 20> empty{};
    All_mRNA.push_back(empty);
    LVBuild.push_back(lv);
    WasChose.push_back(chose);
    Visibility.push_back(visible);
}
float SFAUI_SW=1920.0f; //屏幕宽
float SFAUI_SH=1080.0f; //屏幕高

void adjust_scren() {
    ImVec2 screen_size = ImGui::GetIO().DisplaySize;
    SFAUI_SW = screen_size.x;
    SFAUI_SH = screen_size.y;
}

// 读取日志，Get时间
static std::string GetTimePrefix()
{
    char time_buf[32];
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    strftime(time_buf, sizeof(time_buf), "[%Y-%m-%d %H:%M:%S]  ", t);
    return std::string(time_buf);
}

std::string Sfaui_log_test = "log";
std::string Sfaui_log_path = "./Sfaui.log";
std::string Sfaui_log(const char* fmt, ...)
{
    SfauiSettingConfig& cfg = g_SettingConfig;

    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    std::string timeStr = GetTimePrefix();
    std::string out = timeStr + buf + "\n";
    if(cfg.enableLogError)
    {
        FILE* fp = fopen(Sfaui_log_path.c_str(), "a");
        if (!fp)
        {
            return timeStr + "Sfaui_initting error";
        }
        fprintf(fp, "%s", out.c_str());
        fclose(fp);
    }
    Sfaui_log_test = out;
    return out;
}

////
#define SFAUI_PROJ_MAGIC 0x53464155
#define SFAUI_PROJ_VER   1

static void WriteAny(FILE* fp, const nonstd::any& item)
{
    AnyType t = GetAnyType(item);
    fwrite(&t, sizeof(AnyType), 1, fp);
    switch (t)
    {
    case AnyType_Empty:
    case AnyType_Unknown:
        //  case AnyType_Help_mRNA:
        break;
    case AnyType_Int:
    {
        int v = any_cast<int>(item);
        fwrite(&v, sizeof(int), 1, fp);
        break;
    }
    case AnyType_Float:
    {
        float v = any_cast<float>(item);
        fwrite(&v, sizeof(float), 1, fp);
        break;
    }
    case AnyType_Double:
    {
        double v = any_cast<double>(item);
        fwrite(&v, sizeof(double), 1, fp);
        break;
    }
    case AnyType_Bool:
    {
        bool v = any_cast<bool>(item);
        fwrite(&v, sizeof(bool), 1, fp);
        break;
    }
    case AnyType_String:
    {
        std::string s = any_cast<std::string>(item);
        int len = (int)s.size();
        fwrite(&len, sizeof(int), 1, fp);
        fwrite(s.data(), 1, len, fp);
        break;
    }
    case AnyType_ImVec2:
    {
        ImVec2 v = any_cast<ImVec2>(item);
        fwrite(&v.x, sizeof(float), 1, fp);
        fwrite(&v.y, sizeof(float), 1, fp);
        break;
    }
    case AnyType_ImVec4:
    {
        ImVec4 v = any_cast<ImVec4>(item);
        fwrite(&v.x, sizeof(float), 1, fp);
        fwrite(&v.y, sizeof(float), 1, fp);
        fwrite(&v.z, sizeof(float), 1, fp);
        fwrite(&v.w, sizeof(float), 1, fp);
        break;
    }
    case AnyType_ImU32:
    {
        ImU32 v = any_cast<ImU32>(item);
        fwrite(&v, sizeof(ImU32), 1, fp);
        break;
    }

    case AnyType_LView:
    {
        LView v = any_cast<LView>(item);
        int val = static_cast<int>(v);
        fwrite(&val, sizeof(int), 1, fp);
        break;
    }
    case AnyType_LLayout:
    {
        LLayout v = any_cast<LLayout>(item);
        int val = static_cast<int>(v);
        fwrite(&val, sizeof(int), 1, fp);
        break;
    }

    }
}

char  Sfaui_SavePathBuf[512] = "./Sfaui.sam";
char  Sfaui_WriteCodePathBuf[512] = "./Sfauied.cpp";
bool SaveProjectBinary(const char* filePath)
{
    FILE* fp = fopen(filePath, "wb");
    if (!fp)
    {
        Sfaui_log("SaveBinary fail: open file %s failed", filePath);
        return false;
    }

    
    uint32_t magic = SFAUI_PROJ_MAGIC;
    uint32_t ver = SFAUI_PROJ_VER;
    fwrite(&magic, sizeof(uint32_t), 1, fp);
    fwrite(&ver, sizeof(uint32_t), 1, fp);

    int totalRow = (int)All_mRNA.size();
    fwrite(&totalRow, sizeof(int), 1, fp);

  
    for (auto& row : All_mRNA)
    {
        for (auto& item : row)
        {
            WriteAny(fp, item);
        }
    }

  
    fwrite(LVBuild.data(), sizeof(int), totalRow, fp);
    fwrite(WasChose.data(), sizeof(char), totalRow, fp);
    fwrite(Visibility.data(), sizeof(char), totalRow, fp);

 
    fwrite(&totalRow, sizeof(int), 1, fp);

    fflush(fp);
    fclose(fp);
    Sfaui_log("Binary save success, total rows: %d", totalRow);
    ImGui::OpenPopup("二进制保存成功");
    return true;
}




