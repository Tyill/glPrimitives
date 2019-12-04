//
// glPrimitives Project
// Copyright (C) 2018 by Contributors <https://github.com/Tyill/glPrimitives>
//
// This code is licensed under the MIT License.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#define GLEW_STATIC 1
#include <glew/glew.h>

namespace glPV{

  struct glObject{
    GLuint vao = 0,
           vbo = 0,
           ebo = 0;

    glObject(GLuint _vao = 0, GLuint _vbo = 0, GLuint _ebo = 0) :
      vao(_vao), vbo(_vbo), ebo(_ebo){}
  };

  template<typename T = glObject>
  int cylinder(const T& obj, uint32_t diam, uint32_t leng){

    std::vector<glm::vec3> vertices(362);

    GLfloat deg = glm::pi<GLfloat>() / 180.f,
      rad = diam / 2.f;

    for (int i = 0; i < 360; ++i){

      if (i % 2)
        vertices[i].x = leng;
      else
        vertices[i].x = 0;

      vertices[i].y = rad * sin(deg * i);
      vertices[i].z = rad * cos(deg * i);
    }

    vertices[360].x = 0;
    vertices[360].y = 0;
    vertices[360].z = 0;

    vertices[361].x = leng;
    vertices[361].y = 0;
    vertices[361].z = 0;

    ////////////////////////////////////////

    std::vector<int> indices;
    indices.reserve(360 + 360 + 3 + 360 * 3);

    for (int i = 0; i < 360; ++i)
      indices.push_back(i);

    for (int i = 1; i < 360; ++i)
      indices.push_back(i);

    indices.push_back(1);
    indices.push_back(0);
    indices.push_back(359);

    ////////////////////////////////////////

    for (int i = 0; i < 360; ++i){

      if (i % 2){
        indices.push_back(361);
        indices.push_back(i);
        indices.push_back(i + 2);
      }
      else{
        indices.push_back(360);
        indices.push_back(i);
        indices.push_back(i + 2);
      }
    }

    ////////////////////////////////////////
    
    glBindVertexArray(obj.vao);

    glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
    glBufferData(GL_ARRAY_BUFFER, 362 * sizeof(GLfloat), vertices.data(), GL_DYNAMIC_DRAW);

    int isz = int(indices.size());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, isz * sizeof(GLfloat), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    return isz;

  }

  template<typename T = glObject>
  int cone(const T&, uint32_t diam, uint32_t height){

    return 0;
  }

  template<typename T = glObject>
  int rectangle(const T&, uint32_t width, uint32_t height, uint32_t leng){

    return 0;
  }

  template<typename T = glObject>
  int sphere(const T&, uint32_t diam){

    return 0;
  }  
}