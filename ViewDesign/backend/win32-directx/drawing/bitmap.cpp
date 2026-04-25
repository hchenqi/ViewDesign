#include "ViewDesign/drawing/bitmap.h"
#include "ViewDesign/platform/win32/d2d_api.h"
#include "ViewDesign/platform/win32/directx_helper.h"


namespace ViewDesign {

using namespace Win32;


namespace {

inline owner_ptr<ID2D1Bitmap1> CreateD2DBitmap(const PixelBuffer& pixel_buffer) {
	auto [width, height] = pixel_buffer.Size();
	D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_NONE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
	ComPtr<ID2D1Bitmap1> bitmap;
	hr << GetD2DDeviceContext().CreateBitmap(D2D1::SizeU(width, height), pixel_buffer.Pixels().data(), (uint)(width * sizeof(Color)), &bitmap_properties, &bitmap);
	return bitmap.Detach();
}

} // namespace


const Texture& Bitmap::GetTexture() const {
	if (texture.IsEmpty()) {
		texture.Set(static_cast<owner_ptr<TextureResource>>(CreateD2DBitmap(pixel_buffer)));
	}
	return texture;
}


void BitmapFigure::DrawOn(RenderTarget& target, Point point) const {
	target.DrawBitmap(
		bitmap.GetTexture().GetResource(),
		AsD2DRect(Rect(point, region.size)),
		OpacityAsFloat(opacity),
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		AsD2DRect(region)
	);
}


} // namespace ViewDesign
