#ifndef SFAUI_FITTYPE_H
#define SFAUI_FITTYPE_H

//#include "Sfaui_value.h"
//#include "any.hpp"
#include <typeindex>
#include "Sfaui_fit.h"
using namespace nonstd;

//any的类型检测，在生成代码，Seting要用到
//适配如下
enum AnyType
{
    AnyType_Empty,
    AnyType_Int,
    AnyType_Float,
    AnyType_Bool,
    AnyType_Double,
    AnyType_String,
    AnyType_ImVec2,
    AnyType_ImVec4,
    AnyType_ImU32,
    AnyType_LLayout,
    AnyType_LView,
  //  AnyType_Help_mRNA,
    AnyType_Unknown
};

struct TypeMap
{
    const std::type_info* tinfo;
    AnyType type;
};

static const TypeMap typeTable[] = {
    {&typeid(int), AnyType_Int},
    {&typeid(float), AnyType_Float},
    {&typeid(bool), AnyType_Bool},
    {&typeid(double), AnyType_Double},
    {&typeid(std::string), AnyType_String},
    {&typeid(ImVec2), AnyType_ImVec2},
    {&typeid(ImVec4), AnyType_ImVec4},
    {&typeid(ImU32), AnyType_ImU32},
    {&typeid(LLayout), AnyType_LLayout},
    {&typeid(LView), AnyType_LView}
  //  {&typeid(Help_mRNA), AnyType_Help_mRNA},
};

static const size_t typeTableCount = IM_ARRAYSIZE(typeTable);

AnyType GetAnyType(const any& item);


#endif