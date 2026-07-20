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
void CreatUi(int type);
void LVlist();
void SettingUi();
void Sfaui_DrawLaout();
void log(std::string s);
int translate(std::vector<any> to);

#endif