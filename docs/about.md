# About

## History

This project spans several years of development under a former name `WndDesign`. The design principle and the coding style has shifted largely after multiple initial rewrites until reaching its current pattern at the year of 2021, and was since then under maintenance and further underwent several major refactorings in the course of refinement without changing the core architectural design. So as to say, this current project is not completely built up from scratch, but rather a simplification from more complex previous implementations that are not satisfactory.

The very first version sticks to Win32 API, without much object-oriented and modular design, even without proper version controlling with git. I was at an early stage of learning C++/STL and Win32 APIs and tried to implement basic data structures on my own and manually draw everything on bitmaps by manipulating the pixels. I had to stop at text rendering because of its huge complexity, and turned to using Direct2D and DirectWrite libraries.

The next two versions over-complicated the framework by having both a dynamic library for the core APIs and a static library wrapper, and enforcing every window (the former name for a view) to have a non-client region and styles for layout calculation of both client and non-client regions, making it difficult to check all scenarios of layout dependencies for abstracting a minimal interface. There were multiple interfaces for layout calculation, and no size traits were introduced at this time. A queue for reflowing was used to store the windows whose size has changed, and the windows in the queue are then checked if their size change will affect other windows bottom-up by the order of the depth of the window on the tree, whose layouts are then finally updated top-down. It also tried to optimize drawing of large windows, introducing layers and tiling mechanism for every window, making it costly for creating a single window. Nevertheless, in the previous versions a lot of helpful ideas, structures and also critics were established, providing a strong basis and reference for designing the current and final version. (the previous versions can be found as archived repositories in https://github.com/hchenqiarchive)

By adopting a lightweight view definition without non-client region and layer by default, and with the concept of size reference, frames and layout traits, the current version abstracted the minimal logic for reflow, redraw and event handling, and became modular and elegant.

## Thoughts

- (object-oriented and functional programming) At this stage of developing *ViewDesign*, I highly doubt that there could be other possible ways of developing a GUI library as simple and powerful as this one. It has adopted so many designs that can hardly be replaced. Object-oriented programming fits more with the construction of view components and makes it easy to extend existing interfaces by virtual functions, otherwise the library itself would have to try to include all possible variants of the components by flags, with hardly any flexibility to be custom extended. Frames are particularly important components in *ViewDesign*. A frame as a decorating parent view of a single view component can be seen as a function that transforms the view, with the base `ViewFrame` as the identity function. Thus all components can also be regarded in a functional way.

- (example) I started examining a lot of examples about layout dependencies in developing the next versions of *WndDesign* as it is indented to be general-purpose. One typical example was a list of text boxes and how it resizes when the text changes. A non-trivial example can be so powerful as to completely change the design, so the earlier I take all possible scenarios in consideration, the easier it is to maintain afterwards avoiding more complex fixes. But it can also lead to over-complicated design if it is not well-abstracted from the examples. Sometimes an example can also help to abstract and simplify the logic that might solve an existing issue. For example, by implementing hot reload for `TextEditor`, I started to think about states which helps to implement undo/redo in `TextEditor`.

- (publishing) I have been developing some initial versions of this library, and for many times I thought it could be the final one and get published. But there were always necessary refactorings coming after that either changed the design or simplified the logic. And if it was published and used by people, it will be harder to make such structural changes. It is not quite small for one person to maintain it, and it took me huge time and effort, but as a solo-project, it is more flexible and easier to be kept consistent.

- (documentation) I didn't pay much attention in writing documentation before, and I thought the code is enough. But I realized that documentation isn't just about introducing the project to people and reporting how everything is implemented, but it also helps me to reflect on the designs, to sort out the logic, to fix potential issues and inconsistent styles and to come up with new ideas. It should be an important part in developing any project, at least at the last stage, to make sure the code is clear and consistent.

- (AI) I started to use AI agents and chat bots heavily for the development and refinement of *ViewDesign* in the past few months (2026). Even though I personally dislike AI and the boom of AI research in computer science, they can be indeed of practical help. It is much easier with AI to get explanations of APIs, to decide on a naming, to generate and to check short code pieces or repetitive codes, and to get suggestions for a particular design. It has taken me great effort to go through the API documentations by myself when developing the initial versions. But using AI agents for large-scale refactoring are sometimes more slowly and less controllable than simple regular expressions. They can produce codes that don't fit with my coding conventions, and they won't always understand my exact intention, complicating the process and posing risks. They are well-trained with old data and therefore often displaying difficulties appreciating new ideas. Because of the rather strict coding styles for this project, I always have to finally check and fix every parts by myself.

## Sponsorship Options

This project originates from my personal needs and interests. I've been having fun developing it most of the time so it is already rewarding. I don't seek any sponsorship, and I might not be able to devote to it full-time afterwards. If you find it interesting and useful, please feel free to use it, share it or contribute to it. Nevertheless, I am open to job or research opportunities.

## About Me

I hold a master's degree in computer science. My master's thesis was related with SAT-solving and program verification.

This project is just a side project of mine. I started learning Win32 development and developing the first version of *WndDesign* during COVID-19 at home at the second year of college. The next early versions were developed shortly after that, each taking around two months. In 2021, before my last year of college, I came to the final design that is adopted in this version, and it stayed until now without fundamental changes.

In addition to computer science and programming, I'm interested in mathematics, language, psychology, philosophy, and other aspects related with cognitive science. I've been as well learning multiple foreign languages. 

## Other Projects

Another project of mine *BlockStore* deals with data storage that can be used together with *ViewDesign*. It is currently SQL-based, using SQLite3 as backend but offering a schema-free and stl-like interface for reading and writing data in common data structures like list, deque, tree and graph.

The project *DynamicEditor* (currently being developed) is an example using *ViewDesign* together with *BlockStore* that supports hierarchical text editing with references. I intend to use it as a dictionary tool for building and storing language constructions.
