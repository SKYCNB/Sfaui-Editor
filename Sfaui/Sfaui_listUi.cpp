#include "Sfaui_listUi.h"
using namespace nonstd;

static enum SfauiLaout
{
    ToSettingUi,
    ToListUi,
    ToCreatUi,
    ToNoneUi,
    ToSetSCUi_Imvec2
};

static enum ListUiOnClick
{
    On_View,
    On_Laout,
    On_None,
};
static SfauiLaout NOW_SFAUILAOUT = ToNoneUi;
static ListUiOnClick NOW_LISTUIONCLICK = On_None;
static int idnum=0; //for id, avoid id error
static int flood = 0; //check is on where is the onclik laout flood

//check the like "Begin" laout that has the pair funtion num
static int floodDoor=0;

//for part the ui list that are very important for project
int ListUi_NowStart = 0; //drawing of list ui Start
int ListUi_NowEnd = LVBuild.size(); //drawing of list ui End
int ListUi_NowPos=0; //look oncliking pos

static Help_mRNA Copy;
static int OPCLUI=0; //for open or close Ui laout

static int ONLOCK=0;
static ImGuiWindowFlags LOCKLISTUI = ImGuiWindowFlags_NoTitleBar;
static bool NewFrame_OnClik_back_Button = false;
//动画
SfauiSettingConfig& cfg = g_SettingConfig;

static float floatBall_OffsetX = 0.0f;
static float floatBall_Alpha = 0.0f;
static const float floatBall_AnimSpeed = 4.5f;
static const float floatBall_MaxOffRatio = 0.35f;

static void UpdateFloatBallAnim()
{

    float targetOffX = (OPCLUI % 2 == 1) ? 0.0f : SFAUI_SW * floatBall_MaxOffRatio;
    float targetAlpha = (OPCLUI % 2 == 1) ? 1.0f : 0.0f;


    // 缓动插值
    floatBall_OffsetX += (targetOffX - floatBall_OffsetX) * ImGui::GetIO().DeltaTime * floatBall_AnimSpeed;
    floatBall_Alpha += (targetAlpha - floatBall_Alpha) * ImGui::GetIO().DeltaTime * floatBall_AnimSpeed;
}


//模拟生物的翻译(finish)
int translate(std::vector<any> to)
{
    if(to.size() > 20) {
        Sfaui_log("translate error:any size=%d over range",to.size());
        return 0;
    }

    int t=0;
    if(to[PARAMNUM].has_value() && to[PARAMNUM].type() == typeid(int))
        t=any_cast<int>(to[PARAMNUM]);
    else
    {
        Sfaui_log("translate error:any[%d] no has value",PARAMNUM);
        return 0;
    }
    std::array<any, 20> full{};
    for(int i = 0; i < to.size(); i++)
    {
        if(i<=6 + t)
        {
            if(!to[i].has_value())
            {
                Sfaui_log("translate error:any[%d] no has value",i);
                return 0;
            }
        }
        full[i] = to[i];
    }

    if(full[PAIR].type() != typeid(std::string)||
            full[POS].type() != typeid(ImVec2)||
            full[RANGE].type() != typeid(ImVec2)||
            full[REALID].type() != typeid(std::string)||
            full[PARAMNUM].type() != typeid(int)||
            full[FUNCTIONNMAE].type() != typeid(std::string))
    {
        Sfaui_log("translate error:fixed sequence of tRNA is error");
        return 0;
    }

    if (full[REALID].has_value() && full[REALID].type() == typeid(std::string)) //防止id重复
    {
        std::string s = any_cast<std::string>(full[REALID]);
        full[REALID] = s + std::to_string(idnum);
        if(full[PARAMST].has_value() && full[PARAMST].type()==typeid(std::string))
        {
            //if the funtion has the imgui id, usrealtyly id as imgui id
            full[PARAMST] = full[REALID];
        }
        idnum++;
    }
    All_mRNA.insert(All_mRNA.begin()+ListUi_NowEnd,full);
    return 1;
}


void CreatUi(int type)

{
   
    //type is laout or view
    std::vector<CtrlCategory>控件分类;
    std::map<std::string, std::vector<any>> LVList_tRNA;

    控件分类 = PrintLaoutList;
    LVList_tRNA = LaoutList_tRNA;


    ImGui::Spacing();
    if(ImGui::Button("Back", ImVec2(-1.0f, ButtonSIZE.y)))
        NOW_SFAUILAOUT=ToListUi;
    const float btnHeight = 55.0f;
    for(int i = 0; i < 2; i++) {
        for (auto& cat : 控件分类)
        {
            if (ImGui::CollapsingHeader(cat.name.c_str()))
            {
                for (int i = 0; i < cat.list.size(); i++)
                {
                    auto& name = cat.list[i];
                    ImVec2 textSize = ImGui::CalcTextSize(name.c_str());
                    ImVec2 btnSize(textSize.x + 30.0f, ButtonSIZE.y);

                    if (ImGui::Button(name.c_str(), btnSize))
                    {

                        g_HistoryMgr.SaveSnapshot();

                        if (LVBuild.size() != WasChose.size() ||
                            LVBuild.size() != Visibility.size() ||
                            Visibility.size() != WasChose.size())
                        {
                            Sfaui_log("creat ui error:\"WasChose\",\"LVBuild\",\"Visibility\" is not correspond");
                            return;
                        }

                        //点击添加
                        translate(LVList_tRNA[name]);
                        std::string pair = any_cast<std::string>(LVList_tRNA[name][PAIR]);
                        ListUi_NowEnd += 1;

                        if (pair != name)
                        {

                            translate(LVList_tRNA[pair]);
                            ListUi_NowEnd -= 1;
                            //进行分层处理
                            //make pair laout

                            LVBuild.insert(LVBuild.begin() + ListUi_NowEnd, flood + 1);
                            LVBuild.insert(LVBuild.begin() + ListUi_NowEnd, flood + 1);

                            WasChose.insert(WasChose.begin() + ListUi_NowEnd, 0);
                            WasChose.insert(WasChose.begin() + ListUi_NowEnd, 0);

                            Visibility.insert(Visibility.begin() + ListUi_NowEnd, 1);
                            Visibility.insert(Visibility.begin() + ListUi_NowEnd, 1);
                            ListUi_NowEnd += 2; //因为ListUi_NowEnd只有在初始化和第一层会被LVBuild.size()赋值,必须同步LVBuild.size()
                        }
                        if (pair == name)
                        {
                            ListUi_NowEnd -= 1;
                            LVBuild.insert(LVBuild.begin() + ListUi_NowEnd, 0);
                            WasChose.insert(WasChose.begin() + ListUi_NowEnd, 0);
                            Visibility.insert(Visibility.begin() + ListUi_NowEnd, 1);
                            ListUi_NowEnd++;
                        }


                    }
            
                    if ((i + 1) % 2 != 0)
                        ImGui::SameLine();
                }
                ImGui::Spacing();
            }
        }
        控件分类 = PrintViewList;
        LVList_tRNA = ViewList_tRNA;

    }


}




bool DeleteUi(int st,int en)
{
    int totalSize = (int)LVBuild.size();

    if (st < 0) st = 0;
    if (en > totalSize) en = totalSize;
    if (st >= en)
    {
        Sfaui_log("Delete error");
        return false;
    }


    int love_you = 0;

    for(int epos = st; epos < en; epos++)
    {
        if(WasChose[epos]==1)
            love_you++;
    }

    for(int i = 0; i < love_you; i++)
    {
        for(int j = st; j < en; j++)
        {
            if(WasChose[j]==1)
            {
                WasChose.erase(WasChose.begin() + j);
                Visibility.erase(Visibility.begin() + j);
                LVBuild.erase(LVBuild.begin() + j);
                All_mRNA.erase(All_mRNA.begin() + j);
                ListUi_NowEnd--;

                if (ListUi_NowPos > j)
                {
                    ListUi_NowPos--;
                }
                else if (ListUi_NowPos == j)
                {
                    ListUi_NowPos = st;
                }
                break;
            }
        }
    }

    return true;
}

bool OffWhiteUi(int st,int en) {
    for(int i = st; i < en; i++)
    {
        if(WasChose[i]==1)
        {
            WasChose[i]=0;
        }
    }
    return true;
}
bool AllWhiteyUi(int st,int en) {
    for(int i = st; i < en; i++)
    {
        if(WasChose[i]==0)
        {
            WasChose[i]=1;
        }
    }
    return true;
}

bool InveriUi(int st,int en,std::vector<char> &t)
{
    for(int i = st; i < en; ++i)
    {
        t[i] = t[i] ^ 1;
    }
    return true;
}


bool CopyUi(int st,int en) {
    Copy.mRNA.clear();
    Copy.LVBuild.clear();
    Copy.WasChose.clear();
    Copy.Visibility.clear();
    for(int i = st; i < en; i++) {
        if(WasChose[i]==1)
        {
            Copy.mRNA.push_back(All_mRNA[i]);
            Copy.LVBuild.push_back(LVBuild[i]);
            Copy.WasChose.push_back(0);
            Copy.Visibility.push_back(1);
        }
    }
    return true;
}


bool PickUpUi(int en)
{
    if(Copy.mRNA.empty())
        return false;

    int totalSize = (int)LVBuild.size();

    if(en < 0) en = 0;
    if(en > totalSize) en = totalSize;
    int iniflood_for_copy_LVBuild = 0;

    auto find_it = std::find_if(Copy.LVBuild.begin(), Copy.LVBuild.end(), [](int val) {
        return val != 0;
    });
    if (find_it != Copy.LVBuild.end())
    {
        iniflood_for_copy_LVBuild = Copy.LVBuild[find_it - Copy.LVBuild.begin()] - (flood + 1);
    }
    for(int df = 0; df < Copy.LVBuild.size(); df++)
    {
        if(Copy.LVBuild[df] != 0)
        {
            Copy.LVBuild[df] -= iniflood_for_copy_LVBuild;
        }
    }

    All_mRNA.insert(All_mRNA.begin() + en, Copy.mRNA.begin(), Copy.mRNA.end());
    WasChose.insert(WasChose.begin() + en, Copy.WasChose.begin(), Copy.WasChose.end());
    Visibility.insert(Visibility.begin() + en, Copy.Visibility.begin(), Copy.Visibility.end());
    LVBuild.insert(LVBuild.begin() + en, Copy.LVBuild.begin(), Copy.LVBuild.end());

    int addCnt = (int)Copy.mRNA.size();
    ListUi_NowEnd += addCnt;

    if(ListUi_NowPos >= en)
    {
        ListUi_NowPos += addCnt;
    }

    for(int renamenum = 0; renamenum < addCnt; renamenum++)
    {
        try
        {
            auto& targetArr = All_mRNA[en + renamenum];
            std::string baseId = nonstd::any_cast<std::string>(targetArr[REALID]);
            std::string newId = baseId + "_" + std::to_string(idnum);

            targetArr[REALID] = newId;
            if(targetArr[PARAMST].has_value() && targetArr[PARAMST].type() == typeid(std::string))
            {
                targetArr[PARAMST] = newId;
            }
            idnum++;
        }
        catch(...)
        {
            continue;
        }
    }

    return true;
}
/////
static bool ifcontent_duplicate_white = false;
//锁定开启插入时的层级区间，防止跨层级乱显示
static int dragStartCheck = 0;
static int dragEndCheck = 0;

// 统一关闭插入模式函数
static void CloseDragInsertMode()
{
    ifcontent_duplicate_white = false;
    dragStartCheck = 0;
    dragEndCheck = 0;
}

bool content_duplicate_white(int st) {
    if(std::find(WasChose.begin()+ListUi_NowStart, WasChose.begin()+ListUi_NowEnd, 1) != WasChose.begin()+ListUi_NowEnd) {
        ifcontent_duplicate_white = true;
        // 开启时保存当前层级边界
        dragStartCheck = ListUi_NowStart;
        dragEndCheck = ListUi_NowEnd;
        return true;
    }
    return false;
}



void LVlist()
{

    SfauiSettingConfig& cfg = g_SettingConfig;

    if(cfg.enableAnim)
        UpdateFloatBallAnim();

    //safety check
    int totalSize = (int)LVBuild.size();
    if (ListUi_NowStart < 0) ListUi_NowStart = 0;
    if (ListUi_NowEnd > totalSize) ListUi_NowEnd = totalSize;
    if (ListUi_NowEnd < ListUi_NowStart) ListUi_NowEnd = ListUi_NowStart;
    if (ListUi_NowPos < 0) ListUi_NowPos = 0;
    if (totalSize > 0 && ListUi_NowPos >= totalSize)
        ListUi_NowPos = totalSize - 1;
    if(NOW_LISTUIONCLICK==On_Laout)
    {
        if(flood>0&&LVBuild[ListUi_NowEnd]!=flood) NewFrame_OnClik_back_Button=true;
    }


    if (ifcontent_duplicate_white)
    {
        if (ListUi_NowStart != dragStartCheck || ListUi_NowEnd != dragEndCheck)
        {
            CloseDragInsertMode();
        }
    }


    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(cfg.col_WindowBg.x, cfg.col_WindowBg.y, cfg.col_WindowBg.z, floatBall_Alpha));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(cfg.col_Text.x, cfg.col_Text.y, cfg.col_Text.z, floatBall_Alpha));
    ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.4f,0.4f,0.4f,floatBall_Alpha));

    ImVec2 LVlistUi=ImGui::GetWindowSize();

    float titleWidth = ImGui::CalcTextSize("Sfaui edit").x;
    float availableWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SameLine((availableWidth - titleWidth) * 0.5f);
    ImGui::TextUnformatted("Sfaui edit");
    {
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, cfg.FrameRounding);
        std::string strnow="dj dj my small dj";

        if(ImGui::Button("Back", ImVec2(-1.0f, ButtonSIZE.y))||NewFrame_OnClik_back_Button)
        {
            CloseDragInsertMode(); // 返回直接关闭

            if(NOW_LISTUIONCLICK==On_None)
            {
                OPCLUI++;
                NOW_SFAUILAOUT=ToNoneUi;
            }
            if(NOW_LISTUIONCLICK==On_Laout)
                flood--;
            if(NOW_LISTUIONCLICK==On_View)
                flood=flood;

            if(flood>0)
            {
                for(int findst = ListUi_NowPos; findst >= 0; findst--) {
                    if(LVBuild[findst]==flood)
                    {
                        ListUi_NowStart=findst;
                        break;
                    }
                    if(LVBuild[findst]!=flood&&findst<=0)
                        ListUi_NowStart=-1;
                }
                ListUi_NowEnd=(int)FindFlood(ListUi_NowStart,LVBuild.size(),flood).y;
                ListUi_NowPos=ListUi_NowStart;
                ListUi_NowStart=ListUi_NowStart+1;
                NOW_LISTUIONCLICK=On_Laout;
            }
            else
            {
                flood=0;
                ListUi_NowStart=(int)FindFlood(-1,-1,-1).x;  //reinit
                ListUi_NowEnd=(int)FindFlood(-1,-1,-1).y;
                ListUi_NowPos = 0;
                NOW_LISTUIONCLICK=On_None;
            };
            NewFrame_OnClik_back_Button=false;
        }

        if(NOW_LISTUIONCLICK==On_Laout||NOW_LISTUIONCLICK==On_None) {
            if(ImGui::Button("+ Add", ImVec2(LVlistUi.x * 0.2f, ButtonSIZE.y)))
                NOW_SFAUILAOUT=ToCreatUi;
        }
        ImGui::SameLine();

        if (ImGui::ImageButton("btn_delete_icon", g_tex_pool["deletey"], IMAGESIZE, ImVec2(0,0), ImVec2(1,1)))
        {
            //删除
            g_HistoryMgr.SaveSnapshot();
            DeleteUi(ListUi_NowStart,ListUi_NowEnd);
            //Make_undo();
        }
        ImGui::SameLine();

        if (ImGui::ImageButton("btn_copy_icon", g_tex_pool["copyy"], IMAGESIZE, ImVec2(0,0), ImVec2(1,1)))
            CopyUi(ListUi_NowStart,ListUi_NowEnd);

        ImGui::SameLine();

        if (ImGui::ImageButton("btn_inveri_icon", g_tex_pool["inveri"], IMAGESIZE, ImVec2(0,0), ImVec2(1,1)))
            InveriUi(ListUi_NowStart,ListUi_NowEnd,WasChose);

        ImGui::SameLine();

        if (ImGui::ImageButton("btn_offwhite_icon", g_tex_pool["offwhite"], IMAGESIZE, ImVec2(0,0), ImVec2(1,1)))
            OffWhiteUi(ListUi_NowStart,ListUi_NowEnd);

        ImGui::SameLine();

        if (ImGui::ImageButton("btn_allwhitey_icon", g_tex_pool["allwhitey"], IMAGESIZE, ImVec2(0,0), ImVec2(1,1)))
            AllWhiteyUi(ListUi_NowStart,ListUi_NowEnd);

        ImGui::SameLine();

        if (ImGui::ImageButton("btn_puckup_icon", g_tex_pool["puckup"], IMAGESIZE, ImVec2(0,0), ImVec2(1,1)))
        {
            // 粘贴
            g_HistoryMgr.SaveSnapshot();
            PickUpUi(ListUi_NowEnd);
           // Make_undo();
        }
        ImGui::SameLine();

        if (ImGui::ImageButton("btn_content_duplicate_white_icon", g_tex_pool["content_duplicate_white"], IMAGESIZE, ImVec2(0,0), ImVec2(1,1)))
            content_duplicate_white(ListUi_NowStart);

        ImGui::SameLine();

        if (ImGui::ImageButton("remove_eye_white_icon", g_tex_pool["remove_eye_white"], IMAGESIZE, ImVec2(0,0), ImVec2(1,1)))
        {
            //可见
            for(int i = 0; i < WasChose.size(); i++) {
                if(WasChose[i]==1) {
                    InveriUi(i,i+1,Visibility);
                }
            }
        }

        ImGui::Text("Flood : %d      ",flood);
        ImGui::SameLine();
        if(NOW_LISTUIONCLICK==On_Laout) ImGui::Text("Laout");
        if(NOW_LISTUIONCLICK==On_View) ImGui::Text("View");
        if(NOW_LISTUIONCLICK==On_None) ImGui::Text(" ");
        if(totalSize == 0)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,1,1,floatBall_Alpha));
            ImGui::Text("OnClik \"( + Add )\" come to creat your ui");
            ImGui::Text("Sfaui editor version : beta %0.2f",SFAUIED_VESION);
            ImGui::Text("by Skyc 天C");
            ImGui::PopStyleColor();
        }

        if(NOW_LISTUIONCLICK!=On_None)
            SettingUi();

        floodDoor = 0;
        ImGui::BeginChild("list", ImVec2(-1, -1));
        for(int i = ListUi_NowStart; i < ListUi_NowEnd; i++)
        {
            if(LVBuild[i]==flood+1)
            {
                floodDoor++; //only show the "Begin"
            }
            if(LVBuild[i]==0||LVBuild[i]==flood+1&&floodDoor%2!=0)
            {
                auto arr = &All_mRNA[i];
                if((*arr)[REALID].has_value()&&(*arr)[REALID].type()==typeid(std::string))
                {
                    std::string id= any_cast<std::string>((*arr)[REALID]);  //reality id

                    if(ifcontent_duplicate_white
                            && ListUi_NowStart == dragStartCheck
                            && ListUi_NowEnd == dragEndCheck
                            && WasChose[i] != 1
                            && std::find(WasChose.begin()+ListUi_NowStart, WasChose.begin()+ListUi_NowEnd, 1) != WasChose.begin()+ListUi_NowEnd)
                    {

                        ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(0.22f, 0.78f, 0.48f, 1.0f));
                        if(ImGui::Button(("点击此处插入 Click here to insert"+std::to_string(i)).c_str(), ImVec2(-1.0f,ButtonSIZE.y)))
                        {
                            g_HistoryMgr.SaveSnapshot();

                            Help_mRNA base;

                            for(int ifwaschosen = ListUi_NowStart; ifwaschosen < ListUi_NowEnd; ifwaschosen++) {
                                if(WasChose[ifwaschosen]==1)
                                {
                                    base.mRNA.push_back(All_mRNA[ifwaschosen]);
                                    base.LVBuild.push_back(LVBuild[ifwaschosen]);
                                    base.Visibility.push_back(Visibility[ifwaschosen]);
                                    base.WasChose.push_back(0);
                                }
                            }
                            All_mRNA.insert(All_mRNA.begin()+i,base.mRNA.begin(),base.mRNA.end());
                            WasChose.insert(WasChose.begin()+i,base.WasChose.begin(),base.WasChose.end());
                            Visibility.insert(Visibility.begin()+i,base.Visibility.begin(),base.Visibility.end());
                            LVBuild.insert(LVBuild.begin()+i,base.LVBuild.begin(),base.LVBuild.end());
                            ListUi_NowEnd+=base.mRNA.size();
                            DeleteUi(ListUi_NowStart,ListUi_NowEnd);

                            CloseDragInsertMode(); // 插入完成统一关闭
                            //Make_undo();
                        }
                        ImGui::PopStyleColor();
                    }

                    if(WasChose[i]==1) {
                        ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(0.45f,0.18f,0.18f,0.35f));
                    }
                    else {
                        if(Visibility[i]==1) {

                            if(!cfg.followProjectStyle)
                                ImGui::PushStyleColor(ImGuiCol_Button,cfg.col_Button);
                            else
                                ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(0.20f, 0.24f, 0.32f, 1.00f));
                        }
                        else {
                            ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(0.20f, 0.24f, 0.32f, 0.10f));
                        }
                    }

                    if(ImGui::Button(id.c_str(), ImVec2(LVlistUi.x*0.8f,ButtonSIZE.y)))
                    {
                        CloseDragInsertMode(); // 进入子布局强制关闭
                        ListUi_NowPos=i;
                        if(LVBuild[ListUi_NowPos]==flood+1)
                        {
                            ImVec2 ffd=  FindFlood(ListUi_NowPos,LVBuild.size(),flood+1);
                            ListUi_NowStart =(int)ffd.x +1;
                            ListUi_NowEnd = (int)ffd.y;
                            flood++;
                            NOW_LISTUIONCLICK=On_Laout;
                        }
                        if(LVBuild[ListUi_NowPos]==0)
                        {
                            ListUi_NowStart=ListUi_NowPos;
                            ListUi_NowEnd=ListUi_NowPos;
                            NOW_LISTUIONCLICK=On_View;
                        }

                    }
                    ImGui::PopStyleColor();
                    ImGui::SameLine();

                    //列表勾选框
                    bool tmp = WasChose[i] != 0;
                    std::string indexnum = " ";
                    if(cfg.showItemIndex)indexnum = std::to_string(i);
                    if (ImGui::Checkbox(indexnum.c_str(), &tmp))
                    {
                        WasChose[i] = tmp ? 1 : 0;
                        //检测配套控件被选的情况
                        if(LVBuild[i]>0)
                        {
                            ImVec2 ffd=  FindFlood(i,LVBuild.size(),LVBuild[i]);
                            for(int z = i+1; z <= (int)ffd.y; z++) {
                                WasChose[z] = tmp ? 1 : 0;
                            }
                        }
                    }

                }
            }
            //为了过滤其他楼层的控件
            if(LVBuild[i]==flood+1&&floodDoor%2!=0) {
                for(int j = i+1; j < LVBuild.size(); j++) {
                    if(LVBuild[j]==flood+1)
                    {
                        i=j-1;
                        break;
                    }
                }
            }
        }
        ImGui::EndChild();
    }
    ImGui::PopStyleVar();

    ImGui::PopStyleColor(3);
}



/////


void RenderFlagChecklist(int& mask, const FlagOption* list, size_t count, const char* title, ImVec4 titleColor)
{
    ImGui::TextColored(titleColor, "%s", title);
    for (size_t i = 0; i < count; ++i)
    {
        const auto& opt = list[i];
        bool on = (mask & opt.bit) != 0;
        if (ImGui::Checkbox(opt.label, &on))
        {
            mask = on ? (mask | opt.bit) : (mask & ~opt.bit);
        }
    }
    ImGui::Spacing();
}

void RenderSingleRadio(int& val, const FlagOption* list, size_t count, const char* title, ImVec4 titleColor, int defaultBit)
{
    ImGui::TextColored(titleColor, "%s", title);
    bool valid = false;
    for (size_t i = 0; i < count; i++)
    {
        if (list[i].bit == val) {
            valid = true;
            break;
        }
    }
    if (!valid) val = defaultBit;

    for (size_t i = 0; i < count; ++i)
    {
        const auto& opt = list[i];
        // 拼接唯一ID后缀，区分每一行Radio
        char fullId[256];
        snprintf(fullId, IM_ARRAYSIZE(fullId), "%s##radio_%zu", opt.label, i);
        if (ImGui::RadioButton(fullId, val == opt.bit))
        {
            val = opt.bit;
        }
    }
    ImGui::Spacing();
}



static bool g_OpenVec2Editor = false;
static ImVec2* g_CurrentEditVecTarget = nullptr;
ImVec2 offag = ImVec2(0, 0);
static bool OpenScreenVec2EditPopup(ImVec2& outResult)
{
    static bool isDragging = false;
    static ImVec2 dragStartMouse;
    static ImVec2 dragStartBallPos;
    static ImVec2 editTempVec;
    static ImVec2 ballPos;
    static bool initFlag = false;

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 dispSize = io.DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(dispSize);
    ImGuiWindowFlags winFlags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBackground;

    bool popupOpen = true;
    ImGui::Begin("Vec2ScreenEditorOverlay##unique_vec2_editor", &popupOpen, winFlags);
    ImDrawList* draw = ImGui::GetWindowDrawList();

    ImVec2 winMin = ImGui::GetWindowPos();
    ImVec2 winMax = ImVec2(winMin.x + dispSize.x, winMin.y + dispSize.y);

    draw->AddRectFilled(winMin, winMax, IM_COL32(0,0,0,30));

    if (!initFlag)
    {
        ballPos = outResult;
        editTempVec = ballPos;
        isDragging = false;
        dragStartMouse = ImVec2(0,0);
        dragStartBallPos = ImVec2(0,0);
        initFlag = true;
    }

    ImGui::SetCursorPosX(winMax.x - 190);
    ImGui::Text("X:%.1f Y:%.1f", ballPos.x, ballPos.y);

    float bottomPanelHeight = 200.0f;
    ImVec2 canvasSize = ImVec2(dispSize.x, dispSize.y - bottomPanelHeight);

    ImGui::BeginChild("DragCanvas##vec2", canvasSize, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    bool hovered = ImGui::IsWindowHovered();
    ImDrawList* canvasDraw = ImGui::GetWindowDrawList();

    float r = 18;
    canvasDraw->AddCircleFilled(ballPos, r, IM_COL32(255,255,255,240));
    canvasDraw->AddCircle(ballPos, r, IM_COL32(0,0,0,255), 0, 2);

    if (hovered && io.MouseDown[0])
    {
        if (!isDragging)
        {
            isDragging = true;
            dragStartMouse = io.MousePos;
            dragStartBallPos = ballPos;
        }
        ImVec2 dragDelta(
            io.MousePos.x - dragStartMouse.x,
            io.MousePos.y - dragStartMouse.y
        );
        ballPos.x = dragStartBallPos.x + dragDelta.x;
        ballPos.y = dragStartBallPos.y + dragDelta.y;
        editTempVec = ballPos;
        outResult = ballPos; // 拖拽实时同步
    }
    else
    {
        isDragging = false;
    }
    ImGui::EndChild();
   
    ImGui::Text("X:");
    ImGui::SameLine();
    ImGui::InputFloat("##edit_x", &editTempVec.x, 1.0f, 0.0f, "%.1f");
    ImGui::Text("Y:");
    ImGui::SameLine();
    ImGui::InputFloat("##edit_y", &editTempVec.y, 1.0f, 0.0f, "%.1f");
    ballPos = editTempVec;
    outResult = ImVec2(ballPos.x + offag .x, ballPos.y + offag.y); // 输入框实时同步

    ImGui::SameLine();
    bool confirm = ImGui::Button("Confirm Exit", ImVec2(100,70));
   
    if (confirm)
    {
        g_OpenVec2Editor = false;
        outResult = ballPos;
        initFlag = false;
        isDragging = false;

        ImGui::End();
        return true;
    }

    ImGui::End();
    if (!popupOpen)
    {
        initFlag = false;
        isDragging = false;
        return true;
    }
    return false;
}


// 控件参数可视化编辑面板
// 适配类型：int、float、ImVec2、std::string、bool*、unsigned int(Flags)
// 直接修改原数据，实时同步预览与代码导出
void SettingUi() {

    if (ListUi_NowPos < 0 || ListUi_NowPos >= All_mRNA.size())
    {
        NewFrame_OnClik_back_Button=true;
        ImGui::TextColored(ImVec4(1,0.5f,0.5f,1), "Don't have choose any laout or view");
        return;
    }

    ImGui::Spacing();
    const float btnHeight = 55.0f;

    //reback find the map and key
    auto arr = &All_mRNA[ListUi_NowPos];
    auto itlv = LaoutList_tRNA.find(any_cast<std::string>((*arr)[PAIR]));
    std::vector<any> compamap;
    if (itlv != LaoutList_tRNA.end())
    {
        compamap=itlv->second;
    } else
    {
        itlv = ViewList_tRNA.find(any_cast<std::string>((*arr)[PAIR]));
        if (itlv != ViewList_tRNA.end())
        {
            compamap=itlv->second;
        } else
        {
            Sfaui_log("[%s] Don't find pair map\n",any_cast<std::string>((*arr)[PAIR]).c_str());
            return;
        };
    };

    //for check if is the "end" and others
    if(LVBuild[ListUi_NowPos]!=0&&
            any_cast<std::string>(compamap[FUNCTIONNMAE])==any_cast<std::string>((*arr)[FUNCTIONNMAE]))
    {
        NewFrame_OnClik_back_Button=true;
        return;
    }
    //check the tity
    if(LVBuild[ListUi_NowPos]==0&&
            (ListUi_NowPos!=ListUi_NowEnd||
             ListUi_NowPos!=ListUi_NowStart))
    {
        ListUi_NowEnd=ListUi_NowPos;
        ListUi_NowStart=ListUi_NowPos;
    }
    //when don't has imgui id, total reality id
    // if((*arr)[PARAMST].has_value()==false||(*arr)[PARAMST].type()!=typeid(std::string)) {
    std::string str = any_cast<std::string>((*arr)[REALID]);
    char idbuf[256] {};
    strcpy(idbuf, str.c_str());
    ImGui::InputText("id", idbuf, IM_ARRAYSIZE(idbuf));
    if(std::string(idbuf).size()>0)(*arr)[REALID]= std::string(idbuf);


    std::string id= any_cast<std::string>((*arr)[REALID]);
    if (ImGui::CollapsingHeader(id.c_str()))
    {
        //(*arr)[5] is function num
        for (int j = PARAMST; j < PARAMST+any_cast<int>((*arr)[PARAMNUM]); j++)
        {
            auto& item = (*arr)[j];
            if (!item.has_value()) continue;

            AnyType tp = GetAnyType(item);
            switch (tp)
            {
            case AnyType_Int:
            {
                int val = nonstd::any_cast<int>(item);
                int paramIdx = j - PARAMST;
                auto& typeAny = (*arr)[VLENUM];
                int paramTotal = nonstd::any_cast<int>((*arr)[PARAMNUM]);

                int flagMask = 0;
                if (typeAny.type() == typeid(LLayout))
                {
                    int curLay = static_cast<int>(nonstd::any_cast<LLayout>(typeAny));
                    flagMask = CalcFlagMask(typeid(LLayout), curLay, j, paramTotal);
                }
                else if (typeAny.type() == typeid(LView))
                {
                    int curView = static_cast<int>(nonstd::any_cast<LView>(typeAny));
                    flagMask = CalcFlagMask(typeid(LView), curView, j, paramTotal);
                }

                if (flagMask & BIT_WINFLAG)
                    RenderFlagChecklist(val, g_WindowFlagList, g_WindowFlagCount, "窗口Flags", ImVec4(1,1,0,1));

                else if (flagMask & BIT_COLFLAG)
                    RenderSingleRadio(val, g_ImGuiColList, g_ImGuiColCount, "样式颜色索引 ImGuiCol", ImVec4(1,0,1,1), g_ImGuiColList[0].bit);

                else if (flagMask & BIT_STYLEVAR_VEC2)
                    RenderSingleRadio(val, g_StyleVarVec2List, g_StyleVarVec2Count, "样式变量 ImGuiStyleVar", ImVec4(0.3f,1,0.8f,1), g_StyleVarVec2List[0].bit);

                else if (flagMask & BIT_STYLEVAR_FLOAT)
                    RenderSingleRadio(val, g_StyleVarFloatList, g_StyleVarFloatCount, "样式变量 ImGuiStyleVar", ImVec4(0.3f,1,0.8f,1), g_StyleVarFloatList[0].bit);

                else if (flagMask & BIT_IMGUICOND)
                    RenderSingleRadio(val, g_ImGuiCondList, g_ImGuiCondCount, "枚举 ImGuiCond", ImVec4(0.2f, 0.9f, 1.0f, 1.0f), g_ImGuiCondList[0].bit);

                else if (flagMask & BIT_TREEFLAG)
                    RenderFlagChecklist(val, g_TreeFlagList, g_TreeFlagCount, "树形TreeNodeFlags", ImVec4(0,1,1,1));

                else if (flagMask & BIT_INPUTTEXTFLAG)
                    RenderFlagChecklist(val, g_InputTextFlagList, g_InputTextFlagCount, "InputTextFlags 输入框标志", ImVec4(0.8f,0.6f,1.0f,1.0f));

                else if (flagMask & BIT_DRAGFLAG)
                    RenderFlagChecklist(val, g_DragFlagList, g_DragFlagCount, "DragFlags 拖拽标志", ImVec4(0.4f,1.0f,0.4f,1.0f));

                else if (flagMask & BIT_DRAWFLAG)
                    RenderSingleRadio(val, g_ImDrawFlagList, g_ImDrawFlagCount, "DrawFlag 拖拽标志", ImVec4(0.4f, 6.0f, 0.4f, 1.0f), g_ImDrawFlagList[0].bit);

                else
                    ImGui::DragInt(("参数" + std::to_string(paramIdx)).c_str(), &val, 1, 0, SFAUI_SH*SFAUI_SW);

                item = val;
                break;
            }

            case AnyType_Float:
            {
                float val = nonstd::any_cast<float>(item);
                int paramIdx = j - PARAMST;
                ImGui::DragFloat(("float_" + std::to_string(paramIdx)).c_str(), &val, 1.0f, 0.0f, SFAUI_SW*SFAUI_SH);
                item = val;
                break;
            }
            case AnyType_ImVec2:
            {
                int paramIdx = j - PARAMST;
                ImVec2* refPtr = nonstd::any_cast<ImVec2>(&item);

                if(cfg.UseScreen_SetImvec2)
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f,0.5f,0.9f,0.35f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f,0.65f,1.0f,0.5f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f,0.75f,1.0f,0.6f));
                 
                    if(ImGui::Button(("Imvec2_Screen Drag##" + std::to_string(paramIdx)).c_str(), ImVec2(280,50)))
                    {
                    
                        g_CurrentEditVecTarget = refPtr;
                        g_OpenVec2Editor = true;
                    }
                    ImGui::PopStyleColor(3);

                    if(g_OpenVec2Editor && g_CurrentEditVecTarget == refPtr)
                    {
                        ImVec2 temp = *refPtr;
                        OpenScreenVec2EditPopup(temp);
                        *refPtr = temp;
                    }
                }
                else
                {
                    ImVec2 v = *refPtr;
                    ImGui::PushID(paramIdx);
                    ImGui::DragFloat("ImVec2_X", &v.x, 1.0f, -SFAUI_SW, SFAUI_SW);
                    ImGui::DragFloat("ImVec2_Y", &v.y, 1.0f, -SFAUI_SH, SFAUI_SH);
                    ImGui::PopID();
                    *refPtr = v;
                }
                break;
            }
            case AnyType_ImU32:
            {
                ImU32 col_u32 = nonstd::any_cast<ImU32>(item);
                float col[4] = {
                    ((col_u32 >> IM_COL32_R_SHIFT) & 0xFF) / 255.f,
                    ((col_u32 >> IM_COL32_G_SHIFT) & 0xFF) / 255.f,
                    ((col_u32 >> IM_COL32_B_SHIFT) & 0xFF) / 255.f,
                    ((col_u32 >> IM_COL32_A_SHIFT) & 0xFF) / 255.f
                };
                int paramIdx = j - PARAMST;
                ImGui::ColorEdit4(("ImU32_" + std::to_string(paramIdx)).c_str(), col);
                ImU32 new_col = IM_COL32(
                                    (uint8_t)(col[0] * 255.f),
                                    (uint8_t)(col[1] * 255.f),
                                    (uint8_t)(col[2] * 255.f),
                                    (uint8_t)(col[3] * 255.f)
                                );
                item = new_col;
                break;
            }

            case AnyType_ImVec4:
            {
                ImVec4 color = nonstd::any_cast<ImVec4>(item);
                ImGui::TextUnformatted("ImVec4_");
                ImGui::ColorEdit4("", &color.x);
                item = color;
                break;
            }

            case AnyType_String:
            {
                std::string s = nonstd::any_cast<std::string>(item);
                char buf[256] {};
                strcpy(buf, s.c_str());
                int paramIdx = j - PARAMST;
                ImGui::InputText(("String_" + std::to_string(paramIdx)).c_str(), buf, IM_ARRAYSIZE(buf));
                if(std::string(buf).size()>0)
                    item = std::string(buf);
                break;
            }
            case AnyType_Bool:
            {
                bool val = nonstd::any_cast<bool>(item);
                int paramIdx = j - PARAMST;
                ImGui::Checkbox(("bool_"+ std::to_string(paramIdx)).c_str(), &val);
                item = val;
                break;
            }
            case AnyType_Double:
            {
                double val = nonstd::any_cast<double>(item);
                int paramIdx = j - PARAMST;
                float tmp = (float)val;
                ImGui::DragFloat(("Double_" + std::to_string(paramIdx)).c_str(), &tmp, 1.0f, 0.0f, SFAUI_SW*SFAUI_SH);
                item = (double)tmp;
                break;
            }

            case AnyType_Unknown:
            {
                ImGui::Text("_____unknow_____maybe the point\n可能是指针或未适配类型");
            }
            default:
                break;
            }
            ImGui::Spacing();
        }
        ImGui::Spacing();
    }

}



void Sfaui_DrawLaout() {
    {
        ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(SFAUI_SW*0.91f,SFAUI_SH*0.75f),
                ImVec2(SFAUI_SW*1.01f,SFAUI_SH*0.77f),
                IM_COL32(10, 52, 200, 255), 100.0f, 0);

        if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)
                && ImGui::GetMousePos().x >= SFAUI_SW*0.91f
                && ImGui::GetMousePos().x <= SFAUI_SW*1.01f
                && ImGui::GetMousePos().y >= SFAUI_SH*0.75f
                && ImGui::GetMousePos().y <= SFAUI_SH*0.77f)
        {
            OPCLUI++;
            NOW_SFAUILAOUT = (NOW_SFAUILAOUT == ToNoneUi) ? ToListUi : ToNoneUi;
            floatBall_Alpha = 0.0f;
            floatBall_OffsetX = 0.0f;
        }

        if(OPCLUI%2==0)
        {
            ImGui::GetBackgroundDrawList()->AddText(nullptr,0.02*SFAUI_SH,ImVec2(SFAUI_SW*0.93f,SFAUI_SH*0.75f),
                                                    IM_COL32(255, 255, 255, 255), "open", nullptr);
        } else
        {
            ImGui::GetBackgroundDrawList()->AddText(nullptr,0.02*SFAUI_SH,ImVec2(SFAUI_SW*0.93f,SFAUI_SH*0.75f),
                                                    IM_COL32(255, 255, 255, 255), "close", nullptr);
        }
    }


    //lock
    {

        ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(SFAUI_SW*0.91f,SFAUI_SH*0.65f),
                ImVec2(SFAUI_SW*1.01f,SFAUI_SH*0.67f),
                IM_COL32(100, 100, 60, 255), 100.0f, 0);

        if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)
                && ImGui::GetMousePos().x >= SFAUI_SW*0.91f
                && ImGui::GetMousePos().x <= SFAUI_SW*1.01f
                && ImGui::GetMousePos().y >= SFAUI_SH*0.65f
                && ImGui::GetMousePos().y <= SFAUI_SH*0.67f)
        {
            ONLOCK++;
            LOCKLISTUI = ONLOCK % 2 ==0 ? ImGuiWindowFlags_NoTitleBar : ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;
        }
        if(ONLOCK%2==0)
        {
            ImGui::GetBackgroundDrawList()->AddText(nullptr,0.02*SFAUI_SH,ImVec2(SFAUI_SW*0.93f,SFAUI_SH*0.65f),
                                                    IM_COL32(255, 255, 255, 255), "lock", nullptr);
        } else
        {
            ImGui::GetBackgroundDrawList()->AddText(nullptr,0.02*SFAUI_SH,ImVec2(SFAUI_SW*0.93f,SFAUI_SH*0.65f),
                                                    IM_COL32(255, 255, 255, 255), "outlock", nullptr);
        }

    }

    if(ifundo) {
        ListUi_NowEnd = LVBuild.size();
        ListUi_NowPos = 0;
        ListUi_NowStart = 0;
        flood = 0;
        ifundo=false;
    }
     if (ifredo) {
            ListUi_NowEnd = LVBuild.size();
            ListUi_NowPos = 0;
            ListUi_NowStart = 0;
            flood = 0;
            ifredo = false;
      }

    if(NOW_SFAUILAOUT!=ToNoneUi)

        switch(NOW_SFAUILAOUT)
        {
        case ToSettingUi:
            SettingUi();
            break;
        case ToCreatUi:
            ImGui::Begin("Sfaui_listui", nullptr,LOCKLISTUI);
            CreatUi(0);
            ImGui::End();
            break;
        case ToListUi:
            ImGui::Begin("Sfaui_listui", nullptr,LOCKLISTUI);
            LVlist();
            ImGui::End();
            break;
        case ToNoneUi:

            break;
        default:
            return;
            break;
        }

}


void log(std::string s) {
    ImGui::Begin("log");
    ImGui::Text("%s", Sfaui_log_test.c_str());

    ImGui::Text("All_mRNA.size()=%d", (int)All_mRNA.size());
    ImGui::Text("ListUi_NowEnd=%d,ListUi_NowStart=%d", ListUi_NowEnd, ListUi_NowStart);
    ImGui::Text("ListUi_NowPos=%d", ListUi_NowPos);

    ImGui::Text("flood=%d", flood);
    ImGui::Text("flooddoor=%d", floodDoor);

    ImGui::Text("LVBuild");
    for (int i = 0; i < LVBuild.size(); i++) {
        ImGui::Text("%d", LVBuild[i]);
    }

    ImGui::Text("WasChose");
    for (int i = 0; i < WasChose.size(); i++) {
        ImGui::Text("%d", WasChose[i]);
    }

    ImGui::Text("Visibility");
    for (int i = 0; i < Visibility.size(); i++) {
        ImGui::Text("%d", Visibility[i]);
    }

    // 原有打印
    for (int i = 0; i < All_mRNA.size(); i++) {
        auto &arr = All_mRNA[i];
        auto id = arr[REALID];
        ImGui::Text("%d:%s", LVBuild[i], any_cast<std::string>(id).c_str());
    }

    // 新增：完整打印 All_mRNA 每一条的 20 个元素
    ImGui::Separator();
    ImGui::Text("===== All_mRNA Full Data =====");
    for (int i = 0; i < All_mRNA.size(); ++i)
    {
        ImGui::Text("Row %d:", i);
        auto& rowArr = All_mRNA[i];
        for (int j = 0; j < 20; ++j)
        {
            try
            {
                auto& item = rowArr[j];
                if (item.type() == typeid(int))
                {
                    ImGui::Text("  [%d] int: %d", j, nonstd::any_cast<int>(item));
                }
                else if (item.type() == typeid(float))
                {
                    ImGui::Text("  [%d] float: %.2f", j, nonstd::any_cast<float>(item));
                }
                else if (item.type() == typeid(double))
                {
                    ImGui::Text("  [%d] double: %.3lf", j, nonstd::any_cast<double>(item));
                }
                else if (item.type() == typeid(bool))
                {
                    ImGui::Text("  [%d] bool: %s", j, nonstd::any_cast<bool>(item) ? "true" : "false");
                }
                else if (item.type() == typeid(std::string))
                {
                    ImGui::Text("  [%d] str: %s", j, nonstd::any_cast<std::string>(item).c_str());
                }
                else if (item.type() == typeid(ImVec2))
                {
                    ImVec2 v = nonstd::any_cast<ImVec2>(item);
                    ImGui::Text("  [%d] ImVec2: (%.2f,%.2f)", j, v.x, v.y);
                }
                else if (item.type() == typeid(ImVec4))
                {
                    ImVec4 v = nonstd::any_cast<ImVec4>(item);
                    ImGui::Text("  [%d] ImVec4: (%.2f,%.2f,%.2f,%.2f)", j, v.x, v.y, v.z, v.w);
                }
                else if (item.type() == typeid(ImU32))
                {
                    ImU32 c = nonstd::any_cast<ImU32>(item);
                    ImGui::Text("  [%d] ImU32: 0x%08X", j, c);
                }
                else if (item.type() == typeid(LView))
                {
                    LView v = nonstd::any_cast<LView>(item);
                    ImGui::Text("  [%d] LView enum: %d", j, (int)v);
                }
                else if (item.type() == typeid(LLayout))
                {
                    LLayout v = nonstd::any_cast<LLayout>(item);
                    ImGui::Text("  [%d] LLayout enum: %d", j, (int)v);
                }
                else
                {
                    ImGui::Text("  [%d] unknown type", j);
                }
            }
            catch (...)
            {
                ImGui::Text("  [%d] cast failed", j);
            }
        }
        ImGui::Spacing();
    }

    ImGui::End();
}