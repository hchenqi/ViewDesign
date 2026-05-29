#include "ViewDesign/common/unicode.h"
#include "ViewDesign/geometry/sizeu.h"

#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


namespace ViewDesign {

namespace Stb {


class Image {
public:
	int width, height, channels;
	stbi_uc* data;
public:
	Image(bool flip_vertically, const u16string& filename, int desired_channels = STBI_default) {
		stbi_set_flip_vertically_on_load(flip_vertically);
		data = stbi_load(as_char_str(to_u8string(filename).c_str()), &width, &height, &channels, desired_channels);
		if (data == nullptr) {
			throw std::runtime_error("Stb::Image: failed to load image");
		}
	}
	Image(bool flip_vertically, const void* buffer, size_t size, int desired_channels = STBI_default) {
		stbi_set_flip_vertically_on_load(flip_vertically);
		data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(buffer), size, &width, &height, &channels, desired_channels);
		if (data == nullptr) {
			throw std::runtime_error("Stb::Image: failed to load image");
		}
	}
	~Image() { stbi_image_free(data); }
public:
	SizeU GetSize() const { return SizeU(width, height); }
};


} // namespace Stb

} // namespace ViewDesign
