#include "App.h"
#include "MainLayer.h"

App::App()
	:tnah::Application("TNAH Minimal Demo Application")
{
	PushLayer(new MainLayer());
	
	//Add any init code here for the app window
}

App::~App()
{
	//Clean up here
}

void App::OnEvent(tnah::Event& e)
{
	//Handle any events directly in the window.
	//eg keypress that manipulates the window

	if(e.GetEventType() == tnah::EventType::WindowClose)
	{
		tnah::WindowCloseEvent c = tnah::WindowCloseEvent();
		tnah::Application::OnEvent(c);
	}

	tnah::Application::OnEvent(e);
}


