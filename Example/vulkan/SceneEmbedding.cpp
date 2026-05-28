// rendering custom 3D shapes on a Vulkan offscreen framebuffer and composite it as content of a view component

#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/CenterFrame.h>
#include <ViewDesign/view/frame/BorderFrame.h>
#include <ViewDesign/view/control/Placeholder.h>

#include <ViewDesign/platform/vulkan/framebuffer.h>
#include <ViewDesign/platform/vulkan/pipeline/composite.h>

#include "pipeline/tetra.h"

#include <chrono>
#include <optional>

#include <glm/gtc/matrix_transform.hpp> 


using namespace ViewDesign;


class EmbeddedView : public Placeholder<Auto, Auto> {
private:
	class Scene : public Framebuffer {
	public:
		static constexpr SizeU size = SizeU(400, 400);
	public:
		// using the same Framebuffer constructor as for Layer (depth not enabled)
		Scene() : Framebuffer(size) {}
	public:
		// the existing Framebuffer::Render() is used for Layer to render a Canvas with transform/clip context
		// therefore, we define our own render function
		void Render() {
			// called during EmbeddedView::OnDraw(), reusing the command buffer of the current frame-in-flight of the window's surface
			// the frame-in-flight itself is currently not being rendered
			FrameInFlight& frame_in_flight = FrameInFlight::GetCurrent();
			vk::raii::CommandBuffer& command_buffer = frame_in_flight.command_buffer;

			Vulkan::Render(command_buffer, *image, image_layout, image_view, vk::ImageLayout::eShaderReadOnlyOptimal, extent, vk::Rect2D({}, extent), [&]() {
				using Layout = TetraPipeline::Layout;
				using Vertex = TetraPipeline::Input;

				static constexpr float s = 0.8165f; // sqrt(2/3)
				static constexpr float h = 0.5774f; // 1/sqrt(3)

				static constexpr std::array<Vertex, 4> v = {
					Vertex({ s, -h, 0.0f }, { 1, 0, 0 }),
					Vertex({ -s, -h, 0.0f }, { 0, 1, 0 }),
					Vertex({ 0.0f, h, -1.0f }, { 0, 0, 1 }),
					Vertex({ 0.0f, h, 1.0f }, { 1, 1, 0 }),
				};

				static constexpr std::array<Vertex, 12> vertices = {
					v[0], v[1], v[2],
					v[0], v[2], v[3],
					v[0], v[1], v[3],
					v[1], v[2], v[3],
				};

				command_buffer.setViewport(0, vk::Viewport(0.0f, 0.0f, extent.width, extent.height, 0.0f, 1.0f));
				command_buffer.setScissor(0, vk::Rect2D({}, extent));
				command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *GetPipeline<TetraPipeline>());

				float t = std::chrono::duration<float>(std::chrono::steady_clock::now().time_since_epoch()).count();
				glm::mat4 model = glm::rotate(glm::mat4(1.0f), t, glm::vec3(0.4f, 1.0f, 0.3f));
				glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.5f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)extent.width / extent.height, 0.1f, 100.0f);
				glm::mat4 mvp = proj * view * model;
				command_buffer.pushConstants<glm::mat4>(*GetPipelineLayout<Layout>(), vk::ShaderStageFlagBits::eVertex, offsetof(Layout::Context, vertex.mvp), mvp);

				auto [vertex_buffer, offset] = frame_in_flight.PushVertices(vertices.data(), sizeof(vertices));
				command_buffer.bindVertexBuffers(0, vertex_buffer, { offset });
				command_buffer.draw(vertices.size(), 1, 0, 0);
			});
		}
	};

	struct SceneFigure : Figure {
		Scene& scene;

		SceneFigure(Scene& scene) : scene(scene) {}

		virtual void DrawOn(RenderTarget& target, Point point) const override {
			// the frame-in-flight is currently being rendered
			// Scene::Render() should not be called here
			target.BindPipeline<CompositePipeline>();
			CompositePipeline::SetOpacity(target, 1.0f);
			CompositePipeline::BindTexture(target, *static_cast<ref_ptr<Texture>>(&scene));
			target.DrawVertices(Zip(GetVertices(AsQuad(Rect(point, scene.size))), GetVertices(AsQuad(Rect(0.0f, 0.0f, 1.0f, 1.0f)))));
		}
	};

public:
	EmbeddedView() : Placeholder(Scene::size) {}

private:
	std::optional<Scene> scene;
public:
	void Redraw() { Placeholder::Redraw(rect_infinite); }
private:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		// Scene should also be created during EmbeddedView::OnDraw(), because it uses the command buffer of the current frame-in-flight as well
		if (!scene.has_value()) { scene.emplace(); }
		scene.value().Render();
		canvas.draw(point_zero, new SceneFigure(*scene));
	}
};


void App() {
	ref_ptr<EmbeddedView> embedded_view;
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"SceneEmbedding",
			new CenterFrame<Fixed, Fixed>(
				new BorderFrame(
					Border(2.0f, Color::Orange),
					embedded_view = new EmbeddedView()
				)
			)
		)
	);

	for (;;) {
		desktop.PollEvents();

		if (desktop.WindowListEmpty()) {
			break;
		}

		// embedded_view becomes a dangling reference after the window is closed
		embedded_view->Redraw();
	}
}
