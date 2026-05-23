#include "ViewDesign/view/figure/image.h"
#include "ViewDesign/geometry/tiling.h"


namespace ViewDesign {


void ImageRepeatFigure::DrawOn(RenderTarget& target, Point point) const {
	Size image_size = image.GetSize();
	ImageFigure image_figure(image, rect_empty, opacity);
	for (TileIndex tile_index : GetOverlappingTileRange(image_size, region)) {
		Vector tile_offset = GetTileOffset(image_size, tile_index);
		Rect tile_region = Rect(point_zero + tile_offset, image_size).Intersect(region);
		image_figure.region = tile_region - tile_offset;
		image_figure.DrawOn(target, point + (tile_region.point - region.point));
	}
}


} // namespace ViewDesign
