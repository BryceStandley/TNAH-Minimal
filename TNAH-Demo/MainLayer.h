#pragma once
#include <TNAH-App.h>

class MainLayer : public tnah::Layer
{
public:
	MainLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(tnah::Timestep ts) override;
	void OnImGuiRender() override;
	void OnEvent(tnah::Event& event) override;

private:
	tnah::Ref<tnah::Scene> m_Scene;
	tnah::GameObject m_Camera;
};
