#pragma once

#include "../window/view_traits.h"

#include <vector>


namespace ViewDesign {


class FlowLayout : public ViewType<Assigned, Auto> {
public:
	using child_type = view_ptr<Auto, Assigned>;

public:
	FlowLayout(float row_height, float column_gap, float row_gap);

	// style
protected:
	float row_height;
	float column_gap;
	float row_gap;

	// child
protected:
	struct ChildInfo {
		child_type child;
		float width = 0;
		float offset = 0;
		ChildInfo(child_type child) : child(std::move(child)) {}
	};
	std::vector<ChildInfo> child_list;
protected:
	using child_index = uint;
	std::vector<child_index> row_list;
protected:
	using row_index = uint; using column_index = uint;
	using ChildData = std::pair<row_index, column_index>;
protected:
	void SetChildData(ViewBase& child, ChildData data) { ViewBase::SetChildData<ChildData>(child, data); }
	ChildData GetChildData(ViewBase& child) const { return ViewBase::GetChildData<ChildData>(child); }
protected:
	Rect GetChildRegion(ViewBase& child) const;
	float GetRowOffset(uint row) const;
	uint GetRowNumber() const;
	uint HitTestRow(float y) const;
	auto HitTestColumn(row_index row, float x) const;
public:
	void AppendChild(child_type child);

	// layout
protected:
	Size size;
protected:
	bool UpdateLayout(child_index child_index);
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override;
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override;

	// paint
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override;
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override;

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override;
};


} // namespace ViewDesign
