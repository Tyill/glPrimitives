
#pragma once

#include <wx/wx.h>

#define GLEW_STATIC 1
#include <glew/include/glew.h>

#include <glm/include/glm.hpp>
#include <glm/include/gtc/matrix_transform.hpp>
#include <glm/include/gtc/type_ptr.hpp>
#include <wx/glcanvas.h>
#include "glShader.h"
#include "../../include/glPrimitives.h"

class GLCanvas : public wxGLCanvas
{
public:
    GLCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs);
    ~GLCanvas();
    
private:
   
    wxGLContext* _glContext = nullptr;
    
    glPV::glObject _axisVAO, _prvVAO;   
   
    GLShader _axisShader, _prvShader;
          
    float _rotateByX = 0, _rotateByY = 0;

   
    void connects();

    void createAxis();
       
    void onPaint(wxPaintEvent& event);
   
};
