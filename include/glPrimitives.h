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
#include <glew/include/glew.h>

namespace glPV{

  struct glObject{
    GLuint vao = 0,
           vbo = 0,
           ebo = 0;

    glObject(GLuint _vao = 0, GLuint _vbo = 0, GLuint _ebo = 0) :
      vao(_vao), vbo(_vbo), ebo(_ebo){}
  };
    
  static void _baseObj(float diamTop,
                    float diamBottom,
                        float height,
                    uint32_t degStep, 
                         float xOffs,
                      uint32_t vOffs,
    std::vector<glm::vec3>& outVertices,
           std::vector<int>& outIndices){

    degStep = std::max(uint32_t(1), std::min(uint32_t(10), degStep));

    float deg2rad = glm::pi<float>() / 180.f,
          radTop = diamTop / 2.f,
          radBottom = diamBottom / 2.f;

    size_t vsz = 360 / degStep * 2;

    outVertices.resize(vsz);

    for (int i = 0; i < vsz / 2; ++i){

      GLfloat deg = i * degStep * deg2rad;

      outVertices[i * 2].x = xOffs;
      outVertices[i * 2].y = radTop * sin(deg);
      outVertices[i * 2].z = radTop * cos(deg);

      outVertices[i * 2 + 1].x = xOffs + height;
      outVertices[i * 2 + 1].y = radBottom * sin(deg + degStep / 2.f * deg2rad);
      outVertices[i * 2 + 1].z = radBottom * cos(deg + degStep / 2.f * deg2rad);
    }

    ////////////////////////////////////////
       
    outIndices.clear();
    outIndices.reserve(vsz * 3);

    for (int i = 0; i < (vsz - 2); ++i){

      outIndices.push_back(i + vOffs);
      outIndices.push_back(i + 1 + vOffs);
      outIndices.push_back(i + 2 + vOffs);
    }

    outIndices.push_back(vsz - 2 + vOffs);
    outIndices.push_back(vsz - 1 + vOffs);
    outIndices.push_back(0 + vOffs);

    outIndices.push_back(vsz - 1 + vOffs);
    outIndices.push_back(0 + vOffs);
    outIndices.push_back(1 + vOffs);
  }

  static size_t frustum(const glObject& obj, uint32_t diamTop, uint32_t diamBottom, uint32_t height, uint32_t degStep = 10){
      
    std::vector<glm::vec3> topVertices;
    std::vector<int> topIndices;

    _baseObj(diamTop, 0, 0, degStep, 0, 0, topVertices, topIndices);

    size_t tsz = topVertices.size();

    ////////////////////////////////////////

    std::vector<glm::vec3> bodyVertices;
    std::vector<int> bodyIndices;

    _baseObj(diamTop, diamBottom, height, degStep, 0, uint32_t(tsz), bodyVertices, bodyIndices);

    size_t bsz = bodyVertices.size();

    ////////////////////////////////////////

    std::vector<glm::vec3> bottVertices;
    std::vector<int> bottIndices;

    size_t bmsz = bodyVertices.size();

    _baseObj(0, diamBottom, 0, degStep, height, uint32_t(tsz + bsz), bottVertices, bottIndices);

    ////////////////////////////////////////

    size_t vsz = tsz + bsz + bmsz;

    std::vector<glm::vec3> commVertices(vsz);
    
    memcpy(commVertices.data(), topVertices.data(), tsz * sizeof(glm::vec3));
    memcpy(commVertices.data() + tsz, bodyVertices.data(), bsz * sizeof(glm::vec3));
    memcpy(commVertices.data() + tsz + bsz, bottVertices.data(), bmsz * sizeof(glm::vec3));

    ////////////////////////////////////////

    tsz = topIndices.size();
    bsz = bodyIndices.size();
    bmsz = bottIndices.size();

    size_t isz = tsz + bsz + bmsz;

    std::vector<int> commIndices(isz);

    memcpy(commIndices.data(), topIndices.data(), tsz * sizeof(int));
    memcpy(commIndices.data() + tsz, bodyIndices.data(), bsz * sizeof(int));
    memcpy(commIndices.data() + tsz + bsz, bottIndices.data(), bmsz * sizeof(int));
    
    ////////////////////////////////////////       

    glBindVertexArray(obj.vao);

    glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
    glBufferData(GL_ARRAY_BUFFER, vsz * 3 * sizeof(GLfloat), commVertices.data(), GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, isz * sizeof(GLfloat), commIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    return isz;
  }

  static size_t cylinder(const glObject& obj, uint32_t diam, uint32_t height, uint32_t degStep = 10){

    return frustum(obj, diam, diam, height, degStep);
  }

  static size_t cone(const glObject& obj, uint32_t diam, uint32_t height, uint32_t degStep = 10){

    return frustum(obj, 0, diam, height, degStep);
  }

  static size_t sphere(const glObject& obj, uint32_t diam, uint32_t degStep = 10){
       
    uint32_t slices = diam * 3;

    float diamStep = diam / float(slices);
    size_t vsz = 0,
           isz = 0, 
           vOffs = 0,
           iOffs = 0;
    
    std::vector<glm::vec3> commVertices, vertices;
    std::vector<int> commIndices, indices;

    for (uint32_t i = 0; i < slices; ++i){
     
      float diamTop = 2 * sqrt(std::max(0.f, diam * diam / 4.f - pow((slices - i) * diamStep / 2.f, 2))),
            diamBott = 2 * sqrt(diam * diam / 4.f - pow((slices - i - 1) * diamStep / 2.f, 2));

      if (i >= slices / 2){
        diamTop = 2 * sqrt(diam * diam / 4.f - pow(i * diamStep / 2.f, 2));
        diamBott = 2 * sqrt(std::max(0.f, diam * diam / 4.f - pow((i + 1) * diamStep / 2.f, 2)));
      }
        
      _baseObj(diamTop, diamBott, diamStep, degStep, i * diamStep, uint32_t(vOffs), vertices, indices);

      if (i == 0){
        vsz = vertices.size();
        isz = indices.size();

        commVertices.resize(vsz * slices);
        commIndices.resize(isz * slices);
      }

      memcpy(commVertices.data() + vOffs, vertices.data(), vsz * sizeof(glm::vec3));
      memcpy(commIndices.data() + iOffs, indices.data(), isz * sizeof(int));

      vOffs += vsz;
      iOffs += isz;
    }

    vsz = vOffs;
    isz = iOffs;

    ////////////////////////////////////////       

    glBindVertexArray(obj.vao);

    glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
    glBufferData(GL_ARRAY_BUFFER, vsz * 3 * sizeof(GLfloat), commVertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, isz * sizeof(GLfloat), commIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    return isz;
  }

  static size_t rectangle(const glObject& obj, uint32_t width, uint32_t height, uint32_t leng){

    return 0;//frustum(obj, 0, diam, height, degStep);
  }   
}