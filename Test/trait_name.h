#include <ViewDesign/view/view_traits.h>


using namespace ViewDesign;


template<class> constexpr bool always_false = false;


template<class Trait>
struct Name {
	static_assert(always_false<Trait>, "Unknown Trait");
};

template<class T>
const u16string& GetName() { return Name<T>::text; }


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
