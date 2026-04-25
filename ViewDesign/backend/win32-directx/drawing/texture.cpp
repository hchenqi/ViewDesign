#include "ViewDesign/drawing/texture.h"
#include "ViewDesign/platform/win32/directx_resource.h"
#include "ViewDesign/platform/win32/directx_helper.h"


namespace ViewDesign {

using namespace Win32;


Texture::Texture() : resource(nullptr) { RegisterTexture(*this); }

Texture::~Texture() { Destroy(); UnregisterTexture(*this); }

void Texture::Destroy() { SafeRelease(&resource); }


} // namespace ViewDesign
