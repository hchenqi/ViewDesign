#include "ViewDesign/view/figure/image.h"

#include <ViewDesign/platform/stb/image.h>

#include <ViewDesign/platform/vulkan/texture.h>
#include <ViewDesign/platform/vulkan/pipeline/composite.h>


namespace ViewDesign {

using namespace Vulkan;


namespace {

owner_ptr<Texture> CreateTexture(const Stb::Image& image) {
	auto texture = std::make_unique<Texture>();
	auto size = image.GetSize();
	texture->CreateImage(vk::Format::eR8G8B8A8Unorm, size, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
	texture->CopyImageFromBuffer(size.width * size.height * 4, image.data);
	return texture.release();
}

} // namespace


Image::Image(const u16string& filename) : source(new Stb::Image(false, filename, STBI_rgb_alpha)), size(static_cast<ref_ptr<Stb::Image>>(source)->GetSize()) {}

Image::Image(const void* buffer, size_t size) : source(new Stb::Image(false, buffer, size, STBI_rgb_alpha)), size(static_cast<ref_ptr<Stb::Image>>(source)->GetSize()) {}

Image::~Image() { DestroyTexture(); delete static_cast<owner_ptr<Stb::Image>>(source); }

void Image::CreateTexture() const {
	if (texture == nullptr) {
		texture = ViewDesign::CreateTexture(*static_cast<ref_ptr<Stb::Image>>(source));
	}
}

void Image::DestroyTexture() const {
	if (texture != nullptr) {
		DeviceContext::Get().device.waitIdle();
		delete static_cast<owner_ptr<Texture>>(texture);
		texture = nullptr;
	}
}


void ImageFigure::DrawOn(RenderTarget& target, Point point) const {
	target.BindPipeline<CompositePipeline>();
	CompositePipeline::SetOpacity(target, opacity);
	CompositePipeline::BindTexture(target, *static_cast<ref_ptr<Texture>>(image.GetTexture()));
	target.DrawVertices(Zip(GetVertices(AsQuad(Rect(point, region.size))), GetVertices(AsQuad(Normalize(image.GetSize(), region)))));
}


} // namespace ViewDesign
