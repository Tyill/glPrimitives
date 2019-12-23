
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

  Bind(wxEVT_PAINT, &GLCanvas::onPaint, this);
  Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent& event) {

    int key = event.GetKeyCode();

    if (key == 65)       // left by Y
      --_rotateByY;

    else if (key == 68)  // right by Y
      ++_rotateByY;

    else if (key == 87)  // up by X
      ++_rotateByX;

    else if (key == 83)  // down by X
      --_rotateByX;

    Refresh(false);
  });
  Bind(wxEVT_SIZE, [this](wxSizeEvent& event){

    wxSize wsz = GetClientSize();

    glViewport(0, 0, wsz.GetWidth(), wsz.GetHeight());

    event.Skip();
  });
}

void GLCanvas::onPaint(wxPaintEvent& event){
  
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  wxSize plotSz = GetClientSize();
  
  ////// рисование осей
  createAxis();
    
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 33.0f);
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

  int pntsCnt = glPV::frustum(_prvVAO, 10, 10, 10, 2);
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

