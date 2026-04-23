#include "ViewDesign/drawing/image.h"
#include "ViewDesign/geometry/tiling.h"


namespace ViewDesign {


void ImageRepeatFigure::DrawOn(RenderTarget& target, Point point) const {
	Size image_size = image.GetSize();
	ImageFigure image_figure(image, region_empty, opacity);
	for (RectPointIterator it(RegionToOverlappingTileRange(region, image_size)); it; ++it) {
		Point tile_offset = ScalePointBySize(*it, image_size);
		Rect tile_region = Rect(tile_offset, image_size).Intersect(region);
		image_figure.region = tile_region - (tile_offset - point_zero);
		image_figure.DrawOn(target, point + (tile_region.point - region.point));
	}
}


} // namespace ViewDesign
