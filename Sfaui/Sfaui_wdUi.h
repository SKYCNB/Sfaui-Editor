
#ifndef SFAUI_WDUI_H
#define SFAUI_WDUI_H

//#include "Sfaui_value.h"
#include "Sfaui_fit.h"
#include "Sfaui_fitType.h"
#include "Sfaui_fitEnum.h"

void Windows_Ui();

//about the set
void DrawSettingPanel(bool& openSetPopup);
bool SaveSet();

//make code
std::string AnyToCodeStr(const any& val, const nonstd::any& layoutTypeAny, int paramIndex, int totalParam);
std::string MakeCode();

//draw map view
void DrawMapCanvasOnlyDrag();

#endif
