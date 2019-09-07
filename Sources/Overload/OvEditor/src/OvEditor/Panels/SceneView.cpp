/**
* @project: Overload
* @author: Overload Tech.
* @restrictions: This software may not be resold, redistributed or otherwise conveyed to a third party.
*/

#include <OvUI/Plugins/DDTarget.h>

#include "OvEditor/Core/EditorRenderer.h"
#include "OvEditor/Core/EditorActions.h"
#include "OvEditor/Panels/SceneView.h"

OvEditor::Panels::SceneView::SceneView
(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings
) : AViewControllable(p_title, p_opened, p_windowSettings, true),
	m_sceneManager(EDITOR_CONTEXT(sceneManager))
{
	m_camera.SetClearColor({ 0.278f, 0.278f, 0.278f });

	m_image->AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent += [this](auto p_data)
	{
		std::string path = p_data.first;

		switch (OvTools::Utils::PathParser::GetFileType(path))
		{
		case OvTools::Utils::PathParser::EFileType::SCENE:	EDITOR_EXEC(LoadSceneFromDisk(path));			break;
		case OvTools::Utils::PathParser::EFileType::MODEL:	EDITOR_EXEC(CreateActorWithModel(path, true));	break;
		}
	};
}

void OvEditor::Panels::SceneView::_Render_Impl()
{
	auto& baseRenderer = *EDITOR_CONTEXT(renderer).get();

	uint8_t glState = baseRenderer.FetchGLState();
	baseRenderer.ApplyStateMask(glState);

	RenderScene(glState);
	baseRenderer.ApplyStateMask(glState);
	HandleActorPicking();
	baseRenderer.ApplyStateMask(glState);
}

void OvEditor::Panels::SceneView::RenderScene(uint8_t p_defaultRenderState)
{
	auto& baseRenderer = *EDITOR_CONTEXT(renderer).get();

	m_fbo.Bind();

	baseRenderer.SetStencilMask(0xFF);
	baseRenderer.Clear(m_camera);
	baseRenderer.SetStencilMask(0x00);

	m_editorRenderer.RenderGrid(m_cameraPosition, m_gridColor);
	m_editorRenderer.RenderCameras();
	m_editorRenderer.RenderScene(m_cameraPosition);

	if (EDITOR_EXEC(IsAnyActorSelected()))
	{
		auto& selectedActor = EDITOR_EXEC(GetSelectedActor());

		if (selectedActor.IsActive())
		{
			m_editorRenderer.RenderActorAsSelected(selectedActor, true);
			baseRenderer.ApplyStateMask(p_defaultRenderState);
			m_editorRenderer.RenderActorAsSelected(selectedActor, false);
		}

		baseRenderer.ApplyStateMask(p_defaultRenderState);
		baseRenderer.Clear(false, true, false);
		m_editorRenderer.RenderGuizmo(selectedActor.transform.GetWorldPosition(), selectedActor.transform.GetWorldRotation());
	}

	m_fbo.Unbind();
}

void OvEditor::Panels::SceneView::HandleActorPicking()
{
	if (IsHovered() && EDITOR_CONTEXT(inputManager)->IsMouseButtonPressed(OvWindowing::Inputs::EMouseButton::MOUSE_BUTTON_LEFT))
	{
		/* Prevent losing focus on actor while resizing a window */
		if (auto cursor = ImGui::GetMouseCursor();
			cursor != ImGuiMouseCursor_ResizeEW &&
			cursor != ImGuiMouseCursor_ResizeNS &&
			cursor != ImGuiMouseCursor_ResizeNWSE &&
			cursor != ImGuiMouseCursor_ResizeNESW &&
			cursor != ImGuiMouseCursor_ResizeAll)
		{
			auto& baseRenderer = *EDITOR_CONTEXT(renderer).get();

			auto [winWidth, winHeight] = GetSafeSize();

			m_actorPickingFramebuffer.Resize(winWidth, winHeight);
			m_actorPickingFramebuffer.Bind();
			baseRenderer.SetClearColor(1.0f, 1.0f, 1.0f);
			baseRenderer.Clear();
			m_editorRenderer.RenderSceneForActorPicking();

			// Look actor under mouse
			auto mousePosition = EDITOR_CONTEXT(inputManager)->GetMousePosition();
			auto mouseX = static_cast<uint16_t>(mousePosition.first);
			auto mouseY = static_cast<uint16_t>(mousePosition.second);
			mouseX -= static_cast<uint16_t>(m_position.x);
			mouseY -= static_cast<uint16_t>(m_position.y);
			mouseY = GetSafeSize().second - mouseY + 25;

			uint8_t pixels[3];
			glReadPixels(static_cast<int>(mouseX), static_cast<int>(mouseY), 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixels);
			m_actorPickingFramebuffer.Unbind();

			uint32_t actorID = (0 << 24) | (pixels[2] << 16) | (pixels[1] << 8) | (pixels[0] << 0);

			if (auto actor = EDITOR_CONTEXT(sceneManager).GetCurrentScene()->FindActorByID(actorID))
			{
				EDITOR_EXEC(SelectActor(*actor));
			}
			else
			{
				EDITOR_EXEC(UnselectActor());
			}

		}
	}
}
