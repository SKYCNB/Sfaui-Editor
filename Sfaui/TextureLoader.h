#pragma once
#include <map>
#include <string>
#include "imgui/imgui.h"

#if defined(_WIN32) || defined(_WIN64)
#include <d3d11.h>
extern ID3D11Device* s_DxDevice;
extern ID3D11DeviceContext* s_DxCtx;
void TexLoader_SetDXDevice(ID3D11Device* dev, ID3D11DeviceContext* ctx);
#endif

#if !defined(_WIN32) && !defined(_WIN64)
#if defined(__ANDROID__)
#include <GLES2/gl2.h>
#elif defined(__APPLE__) || defined(__linux__)
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif
#endif

extern std::map<std::string, ImTextureID> g_tex_pool;

ImTextureID LoadEmbeddedRGBA(const unsigned char* data, int w, int h);

inline void RegTex(const std::string& name, const unsigned char* data, int w, int h)
{
    g_tex_pool[name] = LoadEmbeddedRGBA(data, w, h);
}

void ClearAllTex();