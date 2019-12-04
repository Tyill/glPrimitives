
#include <wx/wx.h>
#include "app.h"
#include "mainWin.h"


wxIMPLEMENT_APP(App);
bool App::OnInit()
{
    wxInitAllImageHandlers();

    MainWin *frame = new MainWin(this);
    frame->Show(true);
    return true;
}