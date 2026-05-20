#include "ViewDesign/view/view_traits.h"
#include "ViewDesign/view/layout/ListLayout.h"
#include "ViewDesign/view/layout/SplitLayout.h"


using namespace ViewDesign;


template<class> inline constexpr bool always_false = false;


template<class Trait>
struct Name {
	static_assert(always_false<Trait>, "Unknown Trait");
};

template<>
struct Name<Fixed> {
	inline static const u16string text = u"Fixed";
};

template<>
struct Name<Auto> {
	inline static const u16string text = u"Auto";
};

template<>
struct Name<Relative> {
	inline static const u16string text = u"Relative";
};


template<class WidthTrait>
struct Name<ListLayoutVertical<WidthTrait>> {
	inline static const u16string text = u"ListLayoutVertical<" + Name<WidthTrait>::text + u">";
};

template<class HeightTrait>
struct Name<ListLayoutHorizontal<HeightTrait>> {
	inline static const u16string text = u"ListLayoutHorizontal<" + Name<HeightTrait>::text + u">";
};


template<class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond>
struct Name<SplitLayoutVertical<WidthTraitFirst, HeightTraitFirst, WidthTraitSecond, HeightTraitSecond>> {
	inline static const u16string text = u"SplitLayoutVertical<" + Name<WidthTraitFirst>::text + u", " + Name<HeightTraitFirst>::text + u", " + Name<WidthTraitSecond>::text + u", " + Name<HeightTraitSecond>::text + u">";
};

template<class WidthTraitFirst, class HeightTraitFirst, class WidthTraitSecond, class HeightTraitSecond>
struct Name<SplitLayoutHorizontal<WidthTraitFirst, HeightTraitFirst, WidthTraitSecond, HeightTraitSecond>> {
	inline static const u16string text = u"SplitLayoutHorizontal<" + Name<WidthTraitFirst>::text + u", " + Name<HeightTraitFirst>::text + u", " + Name<WidthTraitSecond>::text + u", " + Name<HeightTraitSecond>::text + u">";
};
