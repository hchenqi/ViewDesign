#pragma once

#include "ViewDesign/view/frame/ViewFrame.h"
#include "ViewDesign/drawing/layer.h"
#include "ViewDesign/geometry/region.h"
#include "ViewDesign/geometry/tiling.h"

#if !defined(NDEBUG)
#include "ViewDesign/view/figure/shape.h"
#endif

#include <unordered_map>


namespace ViewDesign {


template<class WidthTrait, class HeightTrait>
class LayerFrame : public ViewFrame, public SizeTrait<WidthTrait, HeightTrait> {
public:
	using child_type = view_ptr<WidthTrait, HeightTrait>;

public:
	LayerFrame(float opacity, child_type child) : ViewFrame(std::move(child)), opacity(opacity) {}
	LayerFrame(child_type child) : LayerFrame(1.0f, std::move(child)) {}

	// style
protected:
	float opacity;
public:
	float GetOpacity() const { return opacity; }
	void SetOpacity(float opacity) { this->opacity = opacity; Redraw(rect_infinite); }

	// layout
protected:
	Size size;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (Size child_size = UpdateChildSizeRef(child, size_ref); size != child_size) {
			size = child_size;
			layer.Destroy();
		}
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		if (size != child_size) {
			size = child_size;
			layer.Destroy();
			SizeUpdated(size);
		}
	}

	// drawing
protected:
	Scale scale;
	Layer layer;
	Region invalid_region;
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
		scale = canvas.GetCurrentTransform().GetScale();
		SizeU layer_size = Round(size * scale);
		if (!layer.Empty() && layer.GetSize() != layer_size) {
			layer.Destroy();
		}
		Rect composite_region = draw_region * scale, redraw_region = composite_region;
		if (layer.Empty()) {
			layer.Create(layer_size);
			invalid_region.Set(RectI(point_i_zero, layer_size));
		} else {
			Region render_region(RoundUp(redraw_region)); render_region.Intersect(invalid_region);
			redraw_region = render_region.GetBoundingRect();
		}
		if (!redraw_region.IsEmpty()) {
			Canvas canvas;
			canvas.Group(scale, rect_infinite, [&]() { DrawChild(child, point_zero, canvas, redraw_region / scale); });
			layer.RenderCanvas(canvas, vector_zero, redraw_region);
			invalid_region.Sub(RoundDown(redraw_region));
		}
		canvas.draw(draw_region.point, new LayerFigure(layer, composite_region, draw_region.size, opacity));
	}
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override {
		child_redraw_region = child_redraw_region.Intersect(Rect(point_zero, size));
		if (!layer.Empty()) { invalid_region.Union(RoundUp(child_redraw_region * scale)); }
		Redraw(child_redraw_region);
	}
};

template<class T>
LayerFrame(float, T) -> LayerFrame<extract_width_trait<T>, extract_height_trait<T>>;

template<class T>
LayerFrame(T) -> LayerFrame<extract_width_trait<T>, extract_height_trait<T>>;


template<class TilingFunc, class WidthTrait, class HeightTrait>
class LayerFrameTiled : public ViewFrame, public SizeTrait<WidthTrait, HeightTrait> {
public:
	using child_type = view_ptr<WidthTrait, HeightTrait>;

public:
	LayerFrameTiled(float opacity, child_type child) : ViewFrame(std::move(child)), opacity(opacity) {}
	LayerFrameTiled(child_type child) : LayerFrameTiled(1.0f, std::move(child)) {}

	// style
protected:
	float opacity;
public:
	float GetOpacity() const { return opacity; }
	void SetOpacity(float opacity) { this->opacity = opacity; Redraw(rect_infinite); }

	// layout
protected:
	Size size;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		if (Size child_size = UpdateChildSizeRef(child, size_ref); size != child_size) {
			size = child_size;
			tile_cache.clear();
			tile_size = TilingFunc()(tile_size, size * scale);
		}
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		if (size != child_size) {
			size = child_size;
			tile_cache.clear();
			tile_size = TilingFunc()(tile_size, size * scale);
			SizeUpdated(size);
		}
	}

	// drawing
protected:
	Scale scale;
	Region invalid_region;
	SizeU tile_size = size_u_empty;
protected:
	struct TileIndexHash {
		size_t operator() (const TileIndex& tile_index) const {
			return std::hash<int>()(tile_index.x) ^ std::hash<int>()(tile_index.y);
		}
	};
	std::unordered_map<TileIndex, Layer, TileIndexHash> tile_cache;
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		draw_region = draw_region.Intersect(Rect(point_zero, size)); if (draw_region.IsEmpty()) { return; }
		if (Scale scale_new = canvas.GetCurrentTransform().GetScale(); scale != scale_new) {
			scale = scale_new;
			tile_cache.clear();
			tile_size = TilingFunc()(tile_size, size * scale);
		}
		Rect composite_region = draw_region * scale;
		for (TileIndex tile_index : GetOverlappingTileRange(tile_size, composite_region)) {
			if (!tile_cache.contains(tile_index)) {
				invalid_region.Union(RoundUp(GetTileRegion(tile_size, tile_index)));
			}
		}
		Region render_region(RoundUp(composite_region)); render_region.Intersect(invalid_region);
		Rect redraw_region = render_region.GetBoundingRect();
		if (!redraw_region.IsEmpty()) {
			Canvas canvas;
			canvas.Group(scale, rect_infinite, [&]() { DrawChild(child, point_zero, canvas, redraw_region / scale); });
			for (TileIndex tile_index : GetOverlappingTileRange(tile_size, redraw_region)) {
				Layer& layer = tile_cache[tile_index];
				if (layer.Empty()) { layer.Create(tile_size); }
				Vector tile_offset = GetTileOffset(tile_size, tile_index);
				layer.RenderCanvas(canvas, -tile_offset, redraw_region - tile_offset);
			}
			invalid_region.Sub(RoundDown(redraw_region));
		}
		for (TileIndex tile_index : GetOverlappingTileRange(tile_size, composite_region)) {
			Vector tile_offset = GetTileOffset(tile_size, tile_index);
			Rect tile_region = Rect(point_zero + tile_offset, tile_size).Intersect(composite_region);
			canvas.draw(tile_region.point / scale, new LayerFigure(tile_cache.at(tile_index), tile_region - tile_offset, tile_region.size / scale, opacity));
#if !defined(NDEBUG)
			canvas.draw((point_zero + tile_offset) / scale, new Rectangle(tile_size / scale, 1.0f, Color(ColorCode::Red, 0x7F)));
#endif
		}
	}
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override {
		child_redraw_region = child_redraw_region.Intersect(Rect(point_zero, size));
		invalid_region.Union(RoundUp(child_redraw_region * scale));
		Redraw(child_redraw_region);
	}
};

template<class T>
LayerFrameTiled(float, T) -> LayerFrameTiled<TilingFuncDefault, extract_width_trait<T>, extract_height_trait<T>>;

template<class T>
LayerFrameTiled(T) -> LayerFrameTiled<TilingFuncDefault, extract_width_trait<T>, extract_height_trait<T>>;


} // namespace ViewDesign
