#include "MainLayer.h"


MainLayer::MainLayer()
	:tnah::Layer("MainLayer")
{
	m_Scene = tnah::Scene::CreateEmptyScene();
	m_Camera = m_Scene->GetSceneCamera();

	m_Camera.GetComponent<tnah::CameraComponent>().ClearMode = tnah::CameraClearMode::Color;
	m_Camera.GetComponent<tnah::CameraComponent>().ClearColor = {0.5f, 0.5f, 0.5f, 1.0f};

	m_Camera.GetComponent<tnah::CameraComponent>().Camera.SetViewportSize(1280, 720);
	
}

void MainLayer::OnAttach()
{
	Layer::OnAttach();
}

void MainLayer::OnDetach()
{
	Layer::OnDetach();
}

void MainLayer::OnUpdate(tnah::Timestep ts)
{
	Layer::OnUpdate(ts);
}
static bool s_Demo = true;
void MainLayer::OnImGuiRender()
{
	ImGui::Begin("Demo");
	ImGui::TextColored({.5,.5,.5,1},"This is a demo window");
	ImGui::End();

	ImGui::ShowDemoWindow(&s_Demo);
}

void MainLayer::OnEvent(tnah::Event& event)
{
	Layer::OnEvent(event);
}
