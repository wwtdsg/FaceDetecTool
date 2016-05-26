#include "app.h"
#include "frame.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	mainFrame* mFrame = new mainFrame();
	mFrame->Show();
	return true;
}