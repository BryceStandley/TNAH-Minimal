#include "MainLayer.h"


MainLayer::MainLayer()
	:tnah::Layer("MainLayer")
{
	m_Scene = tnah::Scene::CreateEmptyScene();
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
