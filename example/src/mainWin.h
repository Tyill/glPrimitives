
#pragma once

#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>
#include "stdafx.h"
#include "app.h"

class MainWin : public wxFrame
{
    friend void statusMess(const std::string& mess, bool onlyLog);
   
public:
    MainWin(App*);
    ~MainWin() = default;        
      
    App* app = nullptr;

private:
 
  
               
};


