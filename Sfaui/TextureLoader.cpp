#include "TextureLoader.h"

std::map<std::string, ImTextureID> g_tex_pool;

#if defined(_WIN32) || defined(_WIN64)
#include <d3d11.h>
#include <wrl/client.h>
using namespace Microsoft::WRL;

ID3D11Device* s_DxDevice = nullptr;
ID3D11DeviceContext* s_DxCtx = nullptr;

void TexLoader_SetDXDevice(ID3D11Device* dev, ID3D11DeviceContext* ctx)
{
    s_DxDevice = dev;
    s_DxCtx = ctx;
}

ID3D11Device* TexLoader_GetDXDevice()
{
    return s_DxDevice;
}
ID3D11DeviceContext* TexLoader_GetDXContext()
{
    return s_DxCtx;
}

ImTextureID LoadEmbeddedRGBA(const unsigned char* data, int w, int h)
{
    if (!s_DxDevice || !data || w <= 0 || h <= 0)
        return (ImTextureID)nullptr;

    D3D11_TEXTURE2D_DESC texDesc{};
    texDesc.Width = w;
    texDesc.Height = h;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = data;
    initData.SysMemPitch = w * 4;

    ComPtr<ID3D11Texture2D> pTex;
    s_DxDevice->CreateTexture2D(&texDesc, &initData, &pTex);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    ComPtr<ID3D11ShaderResourceView> pSrv;
    s_DxDevice->CreateShaderResourceView(pTex.Get(), &srvDesc, &pSrv);

   
    ID3D11ShaderResourceView* raw = pSrv.Get();
    raw->AddRef();
    return (ImTextureID)raw;
}

void ClearAllTex()
{
    for (auto& pair : g_tex_pool)
    {
        ID3D11ShaderResourceView* srv = (ID3D11ShaderResourceView*)pair.second;
        if (srv)
            srv->Release();
    }
    g_tex_pool.clear();
}

#elif defined(__ANDROID__)
#include <GLES2/gl2.h>

ImTextureID LoadEmbeddedRGBA(const unsigned char* data, int w, int h)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return (ImTextureID)(uintptr_t)tex;
}

void ClearAllTex()
{
    for (auto& pair : g_tex_pool)
    {
        GLuint tid = (GLuint)(uintptr_t)pair.second;
        glDeleteTextures(1, &tid);
    }
    g_tex_pool.clear();
}

#elif defined(__APPLE__) || defined(__linux__)
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

ImTextureID LoadEmbeddedRGBA(const unsigned char* data, int w, int h)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return (ImTextureID)(uintptr_t)tex;
}

void ClearAllTex()
{
    for (auto& pair : g_tex_pool)
    {
        GLuint tid = (GLuint)(uintptr_t)pair.second;
        glDeleteTextures(1, &tid);
    }
    g_tex_pool.clear();
}
#endif