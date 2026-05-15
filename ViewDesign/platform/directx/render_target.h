#pragma once

#include "ViewDesign/platform/directx/d2d_api.h"


namespace ViewDesign {


struct RenderTarget : DirectX::D2DDeviceContext {
	void PushClip(Rect clip_region) { PushAxisAlignedClip(DirectX::AsD2DRect(clip_region), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE); }
	void PopClip() { PopAxisAlignedClip(); }
	void Clear() { DirectX::D2DDeviceContext::Clear(DirectX::AsD2DColor(color_transparent)); }
	void SetTransform(Transform transform) { DirectX::D2DDeviceContext::SetTransform(DirectX::AsD2DTransform(transform)); }
};


} // namespace ViewDesign
