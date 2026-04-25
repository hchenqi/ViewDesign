#pragma once


namespace ViewDesign {

class Texture;

namespace Win32 {


void RegisterTexture(Texture& texture);
void UnregisterTexture(Texture& texture);

void DirectXRecreateResource();


} // namespace Win32

} // namespace ViewDesign
