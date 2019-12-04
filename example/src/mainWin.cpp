
#include <wx/statline.h>
#include <wx/splitter.h>
#include <wx/valnum.h>

#include "glCanvas.h"
#include "mainWin.h"

using namespace std;


void statusMess(const std::string& mess){


}

void MainWin::createTools(){
   
  enum class ID_TYPES{
    onExit = 6001,
  };

  wxMenu *menuFile = new wxMenu;
  menuFile->Append(int(ID_TYPES::onExit), "Выход");
  menuFile->Bind(wxEVT_TOOL, [this](wxCommandEvent& event){

    Close();

  }, int(ID_TYPES::onExit));

  
  wxMenuBar *menuBar = new wxMenuBar;

  menuBar->Append(menuFile, "&Файл");
 
  SetMenuBar(menuBar);
  
  ////////////////////////////////

  wxToolBar* toolBar = this->CreateToolBar();
   
  ////////////////////////////////
   
  toolBar->Realize();
}

void MainWin::load(){
          
  this->SetBackgroundColour(wxSystemSettings::GetColour(wxSystemColour::wxSYS_COLOUR_BTNFACE));

  wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizerAndFit(mainSizer);

  CreateStatusBar();
  SetStatusText("");

  createTools();
    
  app->Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent& event){

    event.Skip();
  });

  wxSplitterWindow* mainSplitter = new wxSplitterWindow(this);
  
  mainSplitter->SetSize(GetClientSize());
  mainSplitter->SetSashGravity(0.15);
   
  ////////////////////////////////
   
  wxPanel* leftPanel = new wxPanel(mainSplitter);
  leftPanel->SetWindowStyle(wxBORDER_SIMPLE);
      
  ////////////////////////////////

  wxGLAttributes vAttrs;
  vAttrs.PlatformDefaults().Defaults().EndList();

  GLCanvas* glCanvas = new GLCanvas(mainSplitter, vAttrs);
  glCanvas->SetWindowStyle(wxBORDER_SIMPLE);

  ////////////////////////////////

  mainSplitter->SplitVertically(leftPanel, glCanvas);

  mainSizer->Add(mainSplitter, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT, 5));
}

MainWin::MainWin(App* app_) :    
    wxFrame(NULL, wxID_ANY, "glPrExample", wxDefaultPosition, wxDefaultSize,
    wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE | wxMAXIMIZE){
     
    app = app_;
   
    load();
    
    this->Maximize(true);        
}
