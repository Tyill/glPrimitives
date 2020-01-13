
#include "stdafx.h"
#include "glCanvas.h"

#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

using namespace std;

const GLchar* vertexShaderSrc = "     \
  in vec3 position;                   \
  out vec3 setColor;                  \
  uniform vec3 color;                 \
  uniform mat4 model;                 \
  uniform mat4 view;                  \
  uniform mat4 projection;            \
  void main(){                        \
    gl_Position = projection * view * model * vec4(position, 1.0f); \
    setColor = color;                 \
  }";


const GLchar* vertexColorShaderSrc = "\
  in vec3 position;                   \
  in vec3 color;                      \
  out vec3 setColor;                  \
  uniform mat4 model;                 \
  uniform mat4 view;                  \
  uniform mat4 projection;            \
  void main(){                        \
    gl_Position = projection * view * model * vec4(position, 1.0f); \
    setColor = color;                 \
  }";


const GLchar* fragmentShaderSrc = "   \
  in vec3 setColor;                   \
  out vec4 outColor;                  \
  void main(){                        \
    outColor = vec4(setColor, 1.0f);  \
  }";

GLCanvas::GLCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs)
  : wxGLCanvas(parent, canvasAttrs){
    
  wxGLContextAttrs ctxAttrs;

  ctxAttrs.PlatformDefaults().CoreProfile().OGLVersion(3, 3).EndList();
  _glContext = new wxGLContext(this, NULL, &ctxAttrs);

  if (!_glContext->IsOK()){
    statusMess("GLCanvas OGL 3.3 version error"); 
    return;
  }

  SetCurrent(*_glContext);

  glewExperimental = GL_TRUE;
   
  if (glewInit() != GLEW_OK){
    statusMess("GLCanvas glewInit error");
    return;
  }

  // z-буфер отслеживается (gl проверяет глубину объектов)
  glEnable(GL_DEPTH_TEST);

  string err;
  if (!_axisShader.setCode(vertexColorShaderSrc, fragmentShaderSrc, err) || 
      !_prvShader.setCode(vertexShaderSrc, fragmentShaderSrc, err)){
    statusMess(err);
    return;
  }
       
  connects();
}

GLCanvas::~GLCanvas(){
      
  if (_glContext){

    if (_axisVAO.vao > 0){
      glDeleteVertexArrays(1, &_axisVAO.vao);
      glDeleteBuffers(1, &_axisVAO.vbo);
      glDeleteBuffers(1, &_axisVAO.ebo);
    }

    glFinish();

    delete _glContext;
  }
}

void GLCanvas::connects(){

  auto setRotateCam = [this](int key){

    if (key == 65)       // A left by Y
      --_rotateByY;

    else if (key == 68)  // D right by Y
      ++_rotateByY;

    else if (key == 87)  // W up by X
      ++_rotateByX;

    else if (key == 83)  // S down by X
      --_rotateByX;
    
    Refresh(false);
  };

  ui.btnW = new wxButton(this, wxID_ANY, "W", wxPoint(100, 100), wxSize(30, 30));
  ui.btnW->Bind(wxEVT_BUTTON, [this, setRotateCam](wxCommandEvent& evt){
       
    setRotateCam(87);
  });

  ui.btnS = new wxButton(this, wxID_ANY, "S", wxPoint(100, 150), wxSize(30, 30));
  ui.btnS->Bind(wxEVT_BUTTON, [this, setRotateCam](wxCommandEvent& evt){
    setRotateCam(83);
  });

  ui.btnA = new wxButton(this, wxID_ANY, "A", wxPoint(50, 150), wxSize(30, 30));
  ui.btnA->Bind(wxEVT_BUTTON, [this, setRotateCam](wxCommandEvent& evt){
    setRotateCam(65);
  });

  ui.btnD = new wxButton(this, wxID_ANY, "D", wxPoint(150, 150), wxSize(30, 30));
  ui.btnD->Bind(wxEVT_BUTTON, [this, setRotateCam](wxCommandEvent& evt){
    setRotateCam(68);
  });

  ui.btnMP = new wxButton(this, wxID_ANY, "+", wxPoint(200, 100), wxSize(30, 30));
  ui.btnMP->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt){
    
    _pos -= 1;

    Refresh(false);
  });

  ui.btnMN = new wxButton(this, wxID_ANY, "-", wxPoint(200, 150), wxSize(30, 30));
  ui.btnMN->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt){
   
    _pos += 1;

    Refresh(false);
  });

  ui.cmbFigure = new wxChoice(this, wxID_ANY, wxPoint(50, 200));
  ui.cmbFigure->Append(vector<wxString>{"frustum", "cylinder", "tube", "cone",
                                        "pyramid", "disk", "sphere", "ring", "parallepd"});
  ui.cmbFigure->Select(0);
  ui.cmbFigure->Bind(wxEVT_CHOICE, [this](wxCommandEvent& evt){
      Refresh(false);
  });
  
  ui.chbFill = new wxCheckBox(this, wxID_ANY, "fill color", wxPoint(50, 240));
  ui.chbFill->SetBackgroundColour(*wxWHITE);
  ui.chbFill->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent& evt){
    Refresh(false);
  });

  wxFont fnt = GetFont();
  fnt.SetPointSize(18);

  ui.lbX = new wxStaticText(this, wxID_ANY, "X");  
  ui.lbX->SetFont(fnt);
  ui.lbX->SetBackgroundColour(*wxWHITE);
  ui.lbX->SetForegroundColour(*wxRED);

  ui.lbY = new wxStaticText(this, wxID_ANY, "Y");
  ui.lbY->SetFont(fnt);
  ui.lbY->SetBackgroundColour(*wxWHITE);
  ui.lbY->SetForegroundColour(*wxGREEN);

  ui.lbZ = new wxStaticText(this, wxID_ANY, "Z");
  ui.lbZ->SetFont(fnt);
  ui.lbZ->SetBackgroundColour(*wxWHITE);
  ui.lbZ->SetForegroundColour(*wxBLUE);

  Bind(wxEVT_PAINT, &GLCanvas::onPaint, this);
  Bind(wxEVT_KEY_DOWN, [this, setRotateCam](wxKeyEvent& event) {
        
    setRotateCam(event.GetKeyCode());

  });
  Bind(wxEVT_MOUSEWHEEL, [this](wxMouseEvent& evt){

      int sign = evt.GetWheelRotation() < 0 ? 1 : -1;

      _pos += sign;

      Refresh(false);

  }, wxID_ANY);
  Bind(wxEVT_SIZE, [this](wxSizeEvent& event){

    wxSize wsz = GetClientSize();

    int w = wsz.GetWidth(), 
        h = wsz.GetHeight();

    glViewport(0, 0, w, h);

    ui.cmbFigure->SetPosition(wxPoint(50, 200));
    ui.chbFill->SetPosition(wxPoint(50, 240));

    ui.btnW->SetPosition(wxPoint(100, 100));
    ui.btnS->SetPosition(wxPoint(100, 150));
    ui.btnA->SetPosition(wxPoint(50, 150));
    ui.btnD->SetPosition(wxPoint(150, 150));
    ui.btnMP->SetPosition(wxPoint(200, 100));
    ui.btnMN->SetPosition(wxPoint(200, 150));

    ui.lbX->SetPosition(wxPoint(50, h - 100));
    ui.lbY->SetPosition(wxPoint(100, h - 100));
    ui.lbZ->SetPosition(wxPoint(150, h - 100));

    event.Skip();
  });
}

void GLCanvas::onPaint(wxPaintEvent& event){
  
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPolygonMode(GL_FRONT_AND_BACK, ui.chbFill->IsChecked() ? GL_FILL : GL_LINE);

  wxSize plotSz = GetClientSize();
  
  ////// рисование осей
  createAxis();
    
  glm::vec3 position = glm::vec3(0.0f, 0.0f, _pos);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 front(glm::vec3(0.0f, 0.0f, -1.0f));
    
  glm::mat4 view = glm::lookAt(position, position + front, up);
  
  glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)plotSz.GetWidth() / plotSz.GetHeight(), 0.1f, 100.0f);

  glm::mat4 model(1);
  model = glm::rotate(model, glm::radians(_rotateByX), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(_rotateByY), glm::vec3(0.0f, 1.0f, 0.0f));

  _axisShader.use();

  _axisShader.setParam("view", view);
  _axisShader.setParam("projection", projection);
  _axisShader.setParam("model", model);

  glBindVertexArray(_axisVAO.vao);
  glDrawArrays(GL_LINES, 0, 6);
  glBindVertexArray(0);
  

  //////////////////////////////////

  if (_prvVAO.vao == 0){
    glGenVertexArrays(1, &_prvVAO.vao);
    glGenBuffers(1, &_prvVAO.vbo);
    glGenBuffers(1, &_prvVAO.ebo);
  }
   
  string selFigure = ui.cmbFigure->GetStringSelection();
  
  size_t pntsCnt = 0;

  if      (selFigure == "frustum")   pntsCnt = glPV::frustum(_prvVAO, 10, 5, 20);
  else if (selFigure == "cylinder")  pntsCnt = glPV::cylinder(_prvVAO, 10, 20);
  else if (selFigure == "tube")      pntsCnt = glPV::tube(_prvVAO, 10, 15, 20);
  else if (selFigure == "cone")      pntsCnt = glPV::cone(_prvVAO, 10, 20);
  else if (selFigure == "pyramid")   pntsCnt = glPV::pyramid(_prvVAO, 10, 20);
  else if (selFigure == "disk")      pntsCnt = glPV::disk(_prvVAO, 10, 5);
  else if (selFigure == "sphere")    pntsCnt = glPV::sphere(_prvVAO, 10);
  else if (selFigure == "ring")      pntsCnt = glPV::ring(_prvVAO, 10, 30);
  else if (selFigure == "parallepd") pntsCnt = glPV::parallepd(_prvVAO, 10, 10, 10);
  
  if (pntsCnt > 0){
     
    _prvShader.use();

    _prvShader.setParam("color", glm::vec3(0.5f, 0.5f, 0.5f));
    _prvShader.setParam("view", view);
    _prvShader.setParam("projection", projection);
    _prvShader.setParam("model", model);

    glBindVertexArray(_prvVAO.vao);
    glDrawElements(GL_TRIANGLES, pntsCnt, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

  SwapBuffers();

}

void GLCanvas::createAxis(){

  GLfloat axisVertices[] = {

    // ось X              // цвет  
    -100.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
     100.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    // ось Y
    0.0f, -100.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f,  100.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    // ось Z
    0.0f, 0.0f, -100.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f,  100.0f, 0.0f, 0.0f, 1.0f,
  };
   
  if (_axisVAO.vao == 0){
    glGenVertexArrays(1, &_axisVAO.vao);
    glGenBuffers(1, &_axisVAO.vbo);
  }

  glBindVertexArray(_axisVAO.vao);

  glBindBuffer(GL_ARRAY_BUFFER, _axisVAO.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_DYNAMIC_DRAW);

  // атрибут с координатами
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  // атрибут с цветом
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
    
  glBindVertexArray(0);
}

