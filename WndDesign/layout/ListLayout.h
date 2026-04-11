#pragma once

#include "../window/wnd_traits.h"

#include <vector>


namespace ViewDesign {


template<class Direction>
class ListLayout;


template<>
class ListLayout<Vertical> : public WndType<Assigned, Auto> {
public:
	using child_type = child_ptr<Assigned, Auto>;

public:
	template<class... Ts>
	ListLayout(float gap, Ts... child_args) : child_list(), gap(gap) {
		child_list.reserve(sizeof...(Ts)); (child_list.emplace_back(std::move(child_args)), ...);
		uint index = 0;	for (auto& info : child_list) { RegisterChild(info.child); SetChildIndex(info.child, index++); }
	}

	// child
protected:
	struct ChildInfo {
	public:
		child_type child;
		float offset;
		float length;
	public:
		ChildInfo(child_type child) : child(std::move(child)), offset(0.0f), length(0.0f) {}
	};
	std::vector<ChildInfo> child_list;
public:
	bool Empty() const { return child_list.empty(); }
	size_t Length() const { return child_list.size(); }
	WndObject& GetChild(size_t index) const { return child_list[index].child; }
	size_t GetChildIndex(WndObject& child) const { return WndObject::GetChildData<size_t>(child); }
protected:
	void SetChildIndex(WndObject& child, size_t index) { WndObject::SetChildData<size_t>(child, index); }
	void UpdateIndex(size_t begin);
public:
	void InsertChild(size_t index, child_type child);
	void InsertChild(size_t begin, std::vector<child_type> children);
	void AppendChild(child_type child) { InsertChild(-1, std::move(child)); }
	void EraseChild(size_t begin, size_t count = 1);
	child_type ExtractChild(size_t index);
	std::vector<child_type> ExtractChild(size_t begin, size_t count);

	// layout
protected:
	Size size;
	float gap;
protected:
	void UpdateLayout(size_t index);
protected:
	Rect GetChildRegion(size_t index) const { return Rect(Point(0.0f, child_list[index].offset), Size(size.width, child_list[index].length)); }
	Rect GetChildRegion(WndObject& child) const { return GetChildRegion(GetChildIndex(child)); }
	size_t HitTestIndex(Point point);
protected:
	virtual Transform GetChildTransform(WndObject& child) const override;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override;
	virtual void OnChildSizeUpdate(WndObject& child, Size child_size) override;

	// paint
protected:
	virtual void OnChildRedraw(WndObject& child, Rect child_redraw_region) override;
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override;

	// event
protected:
	virtual ref_ptr<WndObject> HitTest(MouseEvent& event) override;
};


template<>
class ListLayout<Horizontal> : public WndType<Auto, Assigned> {
public:
	using child_type = child_ptr<Auto, Assigned>;

public:
	template<class... Ts>
	ListLayout(float gap, Ts... child_args) : child_list(), gap(gap) {
		child_list.reserve(sizeof...(Ts)); (child_list.emplace_back(std::move(child_args)), ...);
		uint index = 0;	for (auto& info : child_list) { RegisterChild(info.child); SetChildIndex(info.child, index++); }
	}

	// child
protected:
	struct ChildInfo {
	public:
		child_type child;
		float offset;
		float length;
	public:
		ChildInfo(child_type child) : child(std::move(child)), offset(0.0f), length(0.0f) {}
	};
	std::vector<ChildInfo> child_list;
public:
	bool Empty() const { return child_list.empty(); }
	size_t Length() const { return child_list.size(); }
	WndObject& GetChild(size_t index) const { return child_list[index].child; }
	size_t GetChildIndex(WndObject& child) const { return WndObject::GetChildData<size_t>(child); }
protected:
	void SetChildIndex(WndObject& child, size_t index) { WndObject::SetChildData<size_t>(child, index); }
	void UpdateIndex(size_t begin);
public:
	void InsertChild(size_t index, child_type child);
	void InsertChild(size_t begin, std::vector<child_type> children);
	void AppendChild(child_type child) { InsertChild(-1, std::move(child)); }
	void EraseChild(size_t begin, size_t count = 1);
	child_type ExtractChild(size_t index);
	std::vector<child_type> ExtractChild(size_t begin, size_t count);

	// layout
protected:
	Size size;
	float gap;
protected:
	void UpdateLayout(size_t index);
protected:
	Rect GetChildRegion(size_t index) const { return Rect(Point(child_list[index].offset, 0.0f), Size(child_list[index].length, size.height)); }
	Rect GetChildRegion(WndObject& child) const { return GetChildRegion(GetChildIndex(child)); }
	size_t HitTestIndex(Point point);
protected:
	virtual Transform GetChildTransform(WndObject& child) const override;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override;
	virtual void OnChildSizeUpdate(WndObject& child, Size child_size) override;

	// paint
protected:
	virtual void OnChildRedraw(WndObject& child, Rect child_redraw_region) override;
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override;

	// event
protected:
	virtual ref_ptr<WndObject> HitTest(MouseEvent& event) override;
};


} // namespace ViewDesign
