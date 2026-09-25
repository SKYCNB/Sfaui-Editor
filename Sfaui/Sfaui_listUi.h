#ifndef SFAUI_LISTUI_H
#define SFAUI_LISTUI_H

//#include "Sfaui_value.h"
#include "Sfaui_fit.h"
#include "Sfaui_fitType.h"
#include "Sfaui_fitEnum.h"

using namespace nonstd;


 

//void UiCode();
extern int ListUi_NowStart;
extern int ListUi_NowEnd;

bool OffWhiteLV_OnListUi(int st, int en);
bool AllWhiteLV_OnListUi(int st, int en);
bool CreateLVUi_OnListUi(int type);
void LVlistUi();
void SettingLVUi_OnListUi();
void Sfaui_DrawLaout();
void log(std::string s);
bool translateLV_OnListUi(std::vector<any> to);

#endif