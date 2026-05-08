#include "ViewDesign/drawing/texture.h"
#include "ViewDesign/platform/directx/directx_resource.h"
#include "ViewDesign/platform/directx/directx_helper.h"


namespace ViewDesign {

using namespace DirectX;


Texture::Texture() : resource(nullptr) { RegisterTexture(*this); }

Texture::~Texture() { Destroy(); UnregisterTexture(*this); }

void Texture::Destroy() { SafeRelease(&resource); }


} // namespace ViewDesign
