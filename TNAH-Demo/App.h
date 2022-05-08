#pragma once
#include <TNAH.h>

class App : public tnah::Application
{
public:

	App();
	~App();
	void OnEvent(tnah::Event& e) override;
	
	
};

inline tnah::Application* tnah::CreateApplication()
{
	return new App();
}