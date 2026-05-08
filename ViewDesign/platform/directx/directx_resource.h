#pragma once


namespace ViewDesign {

class Texture;

namespace DirectX {


void RegisterTexture(Texture& texture);
void UnregisterTexture(Texture& texture);

void DirectXRecreateResource();


} // namespace DirectX

} // namespace ViewDesign
