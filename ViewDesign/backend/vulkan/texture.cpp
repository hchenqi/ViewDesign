#include "ViewDesign/drawing/texture.h"


namespace ViewDesign {


Texture::Texture() : resource(nullptr) {}

Texture::~Texture() { Destroy(); }

void Texture::Destroy() {}


} // namespace ViewDesign
