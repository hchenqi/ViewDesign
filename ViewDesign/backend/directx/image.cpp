#include "ViewDesign/view/figure/image.h"
#include "ViewDesign/platform/directx/string.h"
#include "ViewDesign/platform/directx/wic_api.h"
#include "ViewDesign/platform/directx/render_target.h"
#include "ViewDesign/platform/directx/helper.h"


namespace ViewDesign {

using namespace DirectX;


namespace {

using ImageSource = WICFormatConverter;

ComPtr<ImageSource> LoadFromDecoder(ComPtr<IWICBitmapDecoder> decoder) {
	ComPtr<IWICFormatConverter> converter;
	ComPtr<IWICBitmapFrameDecode> source;
	hr << GetWICFactory().CreateFormatConverter(&converter);
	hr << decoder->GetFrame(0, &source);
	hr << converter->Initialize(
		source.Get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		NULL,
		0.0f,
		WICBitmapPaletteTypeMedianCut
	);
	return converter;
}

ComPtr<ImageSource> LoadImageFromFile(u16string file_name) {
	try {
		ComPtr<IWICBitmapDecoder> decoder;
		hr << GetWICFactory().CreateDecoderFromFilename(
			as_wchar_str(file_name.c_str()),
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			&decoder
		);
		return LoadFromDecoder(decoder);
	} catch (std::runtime_error&) {
		throw std::invalid_argument("invalid image file");
	}
}

ComPtr<ImageSource> LoadImageFromMemory(void* address, size_t size) {
	try {
		ComPtr<IWICStream> stream;
		ComPtr<IWICBitmapDecoder> decoder;
		hr << GetWICFactory().CreateStream(&stream);
		hr << stream->InitializeFromMemory(reinterpret_cast<BYTE*>(address), (DWORD)size);
		hr << GetWICFactory().CreateDecoderFromStream(
			stream.Get(),
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&decoder
		);
		return LoadFromDecoder(decoder);
	} catch (std::runtime_error&) {
		throw std::invalid_argument("invalid image address");
	}
}

inline SizeU GetImageSize(ImageSource& source) {
	uint32 width, height;
	hr << source.GetSize(&width, &height);
	return SizeU(width, height);
}

inline ComPtr<D2DBitmap> CreateD2DBitmapFromWicBitmap(ImageSource& source) {
	D2D1_BITMAP_PROPERTIES1 bitmap_properties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_NONE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
	ComPtr<D2DBitmap> bitmap;
	hr << GetD2DDeviceContext().CreateBitmapFromWicBitmap(&source, &bitmap_properties, &bitmap);
	return bitmap;
}

} // namespace


Image::Image(u16string file_name) : source(LoadImageFromFile(file_name).Detach()), size(GetImageSize(*static_cast<ref_ptr<ImageSource>>(source))) {}

Image::Image(void* address, size_t size) : source(LoadImageFromMemory(address, size).Detach()), size(GetImageSize(*static_cast<ref_ptr<ImageSource>>(source))) {}

Image::~Image() { ComPtr<ImageSource>().Swap(reinterpret_cast<owner_ptr<ImageSource>&>(source)); }

void Image::CreateTexture() const {
	if (texture == nullptr) {
		texture = CreateD2DBitmapFromWicBitmap(*static_cast<ref_ptr<ImageSource>>(source)).Detach();
		RegisterBitmap(reinterpret_cast<owner_ptr<D2DBitmap>&>(texture));
	}
}

void Image::DestroyTexture() const {
	if (texture != nullptr) {
		UnregisterBitmap(reinterpret_cast<owner_ptr<D2DBitmap>&>(texture));
		ComPtr<D2DBitmap>().Swap(reinterpret_cast<owner_ptr<D2DBitmap>&>(texture));
	}
}


void ImageFigure::DrawOn(RenderTarget& target, Point point) const {
	target.DrawBitmap(
		static_cast<ref_ptr<D2DBitmap>>(image.GetTexture()),
		AsD2DRect(Rect(point, region.size)),
		opacity,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		AsD2DRect(region)
	);
}


} // namespace ViewDesign
