/**
* @project: Overload
* @author: Overload Tech.
* @restrictions: This software may not be resold, redistributed or otherwise conveyed to a third party.
*/

#include "OvEditor/Panels/AViewControllable.h"

OvEditor::Panels::AViewControllable::AViewControllable
(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings,
	bool p_enableFocusInputs
) : AView(p_title, p_opened, p_windowSettings), m_cameraController(*this, m_camera, m_cameraPosition, p_enableFocusInputs)
{

}

void OvEditor::Panels::AViewControllable::Update(float p_deltaTime)
{
	m_cameraController.HandleInputs(p_deltaTime);

	AView::Update(p_deltaTime);
}

OvEditor::Core::CameraController& OvEditor::Panels::AViewControllable::GetCameraController()
{
	return m_cameraController;
}
