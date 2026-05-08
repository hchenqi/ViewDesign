#include "ViewDesign/drawing/texture.h"
#include "ViewDesign/platform/glad/texture.h"


namespace ViewDesign {


Texture::Texture() : resource(nullptr) {}

Texture::~Texture() { Destroy(); }

void Texture::Destroy() {
	if (resource != nullptr) {
		delete resource;
		resource = nullptr;
	}
}


} // namespace ViewDesign
