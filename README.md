# ViewDesign

A C++ GUI library

## Building Steps

The library and test executables can be built with CMake with Visual Studio and Ninja on Windows.

## Concepts

### View

A view is a component that can have a parent view and multiple child views. All views form a view tree. `Desktop` manages all root views.

Each view is derived from `ViewBase` defining its own logic of calculating layout, drawing content and handling events.

`view_ref` holds a reference to a view, and `view_ptr` holds an allocated unique pointer to a view.

### Control / Frame / Layout

Control, frame and layout are different types of views.

- A control has no child view and does basic functions or displays raw content, like `Button`, `TextBox`, `ImageBox`, etc.

- A frame has one child view to decorate, like `BorderFrame`, `BackgroundFrame`, etc.

- A layout may have multiple child views that are visually arranged in a certain way, like `ListLayout`, `SplitLayout`, `FlowLayout`, `OverlapLayout` etc.

### Fixed / Auto / Relative

Each view occupies a rectangular region on its parent view. Usually the position of a view on its parent view is decided by the parent view completely, but the size of a view can be decided by both the view and its parent view.

The size traits `Fixed`, `Auto` and `Relative` marks how the width or the height of a view should be decided.

- `Fixed` means a dimension of a view is assigned by its parent view.

- `Auto` means a dimension of a view is determined by the view itself.

- `Relative` means a dimension of a view is calculated by the view based on some information provided by its parent view.

A view and a `view_ref` or a `view_ptr` can be marked by a certain size trait. And a parent view may only accept child views with certain size traits. This is checked at compile-time.

Some frames act as a conversion layer for converting size traits of child views:

- `ClipFrame`, `CenterFrame` and `ScrollFrame` convert a dimension from `Auto` to `Fixed`, clipping the overflowing part of the child view. `ClipFrame` puts the child view at a corner. `CenterFrame` puts the child view at the center. `ScrollFrame` makes it possible to scroll to the overflowing part of the child view.

- `FixedFrame` converts a dimension from `Fixed` to `Auto`. It sets a fixed value to the dimension for the child view.

- `MaxFrame` and `MinFrame` convert a dimension from `Relative` to `Auto`, normalizing its child view with a max or min size.

Some particular layouts have their own mechanism of calculating child region bypassing the size traits, like `OverlapLayout`.

### Reflow / Redraw

Reflow is the process of propagating size change and calculating the layout of a view, determining the sizes and positions of its child views. Redraw is the process of propagating an updated region of a view and drawing its content including child views on the updated region.

In general, the parent view provides a size reference `size_ref` to a child view, the child view calculates its size based on the `size_ref` or on its own, and returns its actual size to the parent view, which then calculates its layout and its own size.

If a view later changes its size by itself, it notifies its parent view, and the parent view calculates its layout again and might also change its own size. Reflow stops at the first parent view whose layout is updated but doesn't change its own size. This final parent view then initiates redraw over its updated region.

A view whose content is updated without size change can also initiate redraw. A view who changes its size doesn't initiate redraw because the final parent view will.

A view initiating redraw notifies its parent view about the region on it that is updated, and the parent view translates and clips the region as its own region and notifies its own parent view until `DesktopFrame` which then draws all child views in the updated region.

