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

#include <vector>
#include <cstdint>
#include <cmath>

#define GLEW_STATIC 1
#include <glew/include/glew.h>
#include <glm/include/glm.hpp>

namespace glPV{

  struct glObject{
    GLuint vao = 0, // Vertex Array Object
           vbo = 0, // Vertex Buffer Object
           ibo = 0; // Index Buffer Object

    glObject(GLuint _vao = 0, GLuint _vbo = 0, GLuint _ibo = 0) :
      vao(_vao), vbo(_vbo), ibo(_ibo){}
  };
    
  static void _baseObj(float diamTop,
                    float diamBottom,
                        float height,
                    uint32_t degStep, 
          const glm::vec3& beginOffs,
            const glm::vec3& endOffs,
                      uint32_t vOffs,
    std::vector<glm::vec3>& outVertices,
           std::vector<int>& outIndices){

    degStep = std::max(uint32_t(1), std::min(uint32_t(120), degStep));

    float deg2rad = glm::pi<float>() / 180.f,
          radTop = diamTop / 2.f,
          radBottom = diamBottom / 2.f;

    size_t vsz = 360 / degStep * 2;

    outVertices.resize(vsz);

    for (int i = 0; i < vsz / 2; ++i){

      GLfloat deg = i * degStep * deg2rad;

      outVertices[i * 2].x = beginOffs.x;
      outVertices[i * 2].y = radTop * sin(deg) + beginOffs.y;
      outVertices[i * 2].z = radTop * cos(deg) + beginOffs.z;

      outVertices[i * 2 + 1].x = height + endOffs.x;
      outVertices[i * 2 + 1].y = radBottom * sin(deg + degStep / 2.f * deg2rad) + endOffs.y;
      outVertices[i * 2 + 1].z = radBottom * cos(deg + degStep / 2.f * deg2rad) + endOffs.z;
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

  static void _fillGLObject(const glObject& obj, size_t vsz, const glm::vec3* pVertData, size_t isz, const int* pIndexData){

      glBindVertexArray(obj.vao);

      glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
      glBufferData(GL_ARRAY_BUFFER, vsz * 3 * sizeof(GLfloat), pVertData, GL_DYNAMIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ibo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, isz * sizeof(GLfloat), pIndexData, GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
      glEnableVertexAttribArray(0);

      glBindVertexArray(0);
  }

  /// frustum
  /// @param[in] obj - glObject struct
  /// @param[in] diamTop - diametr top
  /// @param[in] diamBottom - diametr bottom
  /// @param[in] height - height of figure
  /// @param[in] degStep - coef smooth of surface
  /// @return Index Buffer size
  static size_t frustum(const glObject& obj, uint32_t diamTop, uint32_t diamBottom, uint32_t height, uint32_t degStep = 10){
   
    std::vector<glm::vec3> topVertices;
    std::vector<int> topIndices;

    _baseObj(diamTop, 0, 0, degStep, glm::vec3(0), glm::vec3(0), 0, topVertices, topIndices);

    size_t tsz = topVertices.size();

    ////////////////////////////////////////

    std::vector<glm::vec3> bodyVertices;
    std::vector<int> bodyIndices;

    _baseObj(diamTop, diamBottom, height, degStep, glm::vec3(0), glm::vec3(0), uint32_t(tsz), bodyVertices, bodyIndices);

    size_t bsz = bodyVertices.size();

    ////////////////////////////////////////

    std::vector<glm::vec3> bottVertices;
    std::vector<int> bottIndices;

    size_t bmsz = bodyVertices.size();

    _baseObj(0, diamBottom, 0, degStep, glm::vec3(height, 0, 0), glm::vec3(height, 0, 0), uint32_t(tsz + bsz), bottVertices, bottIndices);

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

    _fillGLObject(obj, vsz, commVertices.data(), isz, commIndices.data());

    return isz;
  }

  /// cylinder
  /// @param[in] obj - glObject struct
  /// @param[in] diam - diametr 
  /// @param[in] height - height of figure
  /// @param[in] degStep - coef smooth of surface
  /// @return Index Buffer size
  static size_t cylinder(const glObject& obj, uint32_t diam, uint32_t height, uint32_t degStep = 10){

    diam = std::max(uint32_t(1), diam);

    return frustum(obj, diam, diam, height, degStep);
  }

  /// tube
  /// @param[in] obj - glObject struct
  /// @param[in] innerDiam - inner diametr 
  /// @param[in] extDiam - external diametr 
  /// @param[in] height - height of figure
  /// @param[in] degStep - coef smooth of surface
  /// @return Index Buffer size
  static size_t tube(const glObject& obj, uint32_t innerDiam, uint32_t extDiam, uint32_t height, uint32_t degStep = 10){

    innerDiam = std::max(uint32_t(1), innerDiam);
    extDiam = std::max(uint32_t(1), extDiam);

    std::vector<glm::vec3> commVertices;
    std::vector<int> commIndices;

    size_t vsz = 0, isz = 0;
    
    if (innerDiam == extDiam){
        
      _baseObj(innerDiam, innerDiam, height, degStep, glm::vec3(0), glm::vec3(0), 0, commVertices, commIndices);

      vsz = commVertices.size();
      isz = commIndices.size();

    }
    else{

      std::vector<glm::vec3> innerVertices;
      std::vector<int> innerIndices;

      _baseObj(innerDiam, innerDiam, height, degStep, glm::vec3(0), glm::vec3(0), 0, innerVertices, innerIndices);

      size_t insz = innerVertices.size();

      ////////////////////////////////////////       

      std::vector<glm::vec3> extVertices;
      std::vector<int> extIndices;

      _baseObj(extDiam, extDiam, height, degStep, glm::vec3(0), glm::vec3(0), uint32_t(insz), extVertices, extIndices);

      size_t exsz = extVertices.size();

      ////////////////////////////////////////       

      std::vector<glm::vec3> beginVertices;
      std::vector<int> beginIndices;

      _baseObj(innerDiam, extDiam, 0, degStep, glm::vec3(0), glm::vec3(0), uint32_t(insz + exsz), beginVertices, beginIndices);

      size_t bgsz = beginVertices.size();

      ////////////////////////////////////////       

      std::vector<glm::vec3> endVertices;
      std::vector<int> endIndices;

      _baseObj(innerDiam, extDiam, 0, degStep, glm::vec3(height, 0, 0), glm::vec3(height, 0, 0), uint32_t(insz + exsz + bgsz), endVertices, endIndices);

      size_t endsz = endVertices.size();

      ////////////////////////////////////////       

      vsz = insz + exsz + bgsz + endsz;

      commVertices.resize(vsz);

      memcpy(commVertices.data(), innerVertices.data(), insz * sizeof(glm::vec3));
      memcpy(commVertices.data() + insz, extVertices.data(), exsz * sizeof(glm::vec3));
      memcpy(commVertices.data() + insz + exsz, beginVertices.data(), bgsz * sizeof(glm::vec3));
      memcpy(commVertices.data() + insz + exsz + bgsz, endVertices.data(), endsz * sizeof(glm::vec3));

      ////////////////////////////////////////

      insz = innerIndices.size();
      exsz = extIndices.size();
      bgsz = beginIndices.size();
      endsz = endIndices.size();

      isz = insz + exsz + bgsz + endsz;

      commIndices.resize(isz);

      memcpy(commIndices.data(), innerIndices.data(), insz * sizeof(int));
      memcpy(commIndices.data() + insz, extIndices.data(), exsz * sizeof(int));
      memcpy(commIndices.data() + insz + exsz, beginIndices.data(), bgsz * sizeof(int));
      memcpy(commIndices.data() + insz + exsz + bgsz, endIndices.data(), endsz * sizeof(int));

    }

    ////////////////////////////////////////    

    _fillGLObject(obj, vsz, commVertices.data(), isz, commIndices.data());

    return isz;
  }

  /// cone
  /// @param[in] obj - glObject struct
  /// @param[in] diam - diametr 
  /// @param[in] height - height of figure
  /// @param[in] degStep - coef smooth of surface
  /// @return Index Buffer size
  static size_t cone(const glObject& obj, uint32_t diam, uint32_t height, uint32_t degStep = 10){

    diam = std::max(uint32_t(1), diam);

    return frustum(obj, 0, diam, height, degStep);
  }

  /// pyramid
  /// @param[in] obj - glObject struct
  /// @param[in] diam - diametr 
  /// @param[in] height - height of figure
  /// @param[in] angles - angles of pyramid
  /// @return Index Buffer size
  static size_t pyramid(const glObject& obj, uint32_t diam, uint32_t height, uint32_t angles = 3){

    diam = std::max(uint32_t(1), diam);

    return frustum(obj, 0, diam, height, 360 / std::max(uint32_t(1), angles));
  }

  /// disk
  /// @param[in] obj - glObject struct
  /// @param[in] diam - diametr 
  /// @param[in] height - height of figure
  /// @param[in] degStep - coef smooth of surface
  /// @return Index Buffer size
  static size_t disk(const glObject& obj, uint32_t diam, uint32_t height, uint32_t degStep = 10){

    diam = std::max(uint32_t(1), diam);
  
    uint32_t slices = diam;

    float diamStep = diam / float(slices),
          heightStep = height / float(slices);
    size_t vsz = 0,
           isz = 0,
           vOffs = 0,
           iOffs = 0;

    std::vector<glm::vec3> commVertices, vertices;
    std::vector<int> commIndices, indices;

    for (uint32_t i = 0; i < slices; ++i){

      float diamTop = 2 * i * diamStep,
        diamBott = 2 * (i + 1) * diamStep;

      if (i >= slices / 2){
        diamTop = 2 * (slices - i) * diamStep;
        diamBott = 2 * (slices - i - 1) * diamStep;
      }

      _baseObj(diamTop,
              diamBott,
            heightStep,
               degStep,
       glm::vec3(i * heightStep, 0, 0),
       glm::vec3(i * heightStep, 0, 0),
       uint32_t(vOffs),
              vertices,
               indices);

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

    _fillGLObject(obj, vsz, commVertices.data(), isz, commIndices.data());

    return isz;
  }

  /// sphere
  /// @param[in] obj - glObject struct
  /// @param[in] diam - diametr 
  /// @param[in] smooth - coef smooth of figure
  /// @param[in] degStep - coef smooth of surface
  /// @return Index Buffer size
  static size_t sphere(const glObject& obj, uint32_t diam, uint32_t smooth = 10, uint32_t degStep = 10){
    
    diam = std::max(uint32_t(1), diam);

    uint32_t slices = diam * smooth;

    float diamStep = diam / float(slices);
    size_t vsz = 0,
           isz = 0, 
           vOffs = 0,
           iOffs = 0;
    
    std::vector<glm::vec3> commVertices, vertices;
    std::vector<int> commIndices, indices;
        
    for (uint32_t i = 0; i < slices; ++i){
     
      float diamTop = 2 * sqrt(std::max(0.f, diam * diam / 4.f - pow((slices / 2.f - i) * diamStep, 2))),
            diamBott = 2 * sqrt(diam * diam / 4.f - pow((slices / 2.f - i - 1) * diamStep, 2));

      if (i >= slices / 2){
        diamTop = 2 * sqrt(diam * diam / 4.f - pow((i - slices / 2.f) * diamStep, 2));
        diamBott = 2 * sqrt(std::max(0.f, diam * diam / 4.f - pow((i - slices / 2.f + 1) * diamStep, 2)));
      }
        
      _baseObj(diamTop, 
              diamBott,
              diamStep,
               degStep, 
       glm::vec3(i * diamStep, 0, 0), 
       glm::vec3(i * diamStep, 0, 0),
       uint32_t(vOffs), 
              vertices, 
               indices);

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

    _fillGLObject(obj, vsz, commVertices.data(), isz, commIndices.data());

    return isz;
  }

  /// ring
  /// @param[in] obj - glObject struct
  /// @param[in] ringDiam - ring diametr 
  /// @param[in] extDiam - external diametr 
  /// @param[in] smooth - coef smooth of figure
  /// @param[in] degStep - coef smooth of surface
  /// @return Index Buffer size
  static size_t ring(const glObject& obj, uint32_t ringDiam, uint32_t extDiam, uint32_t smooth = 10, uint32_t degStep = 10){

    ringDiam = std::max(uint32_t(1), ringDiam);
    extDiam = std::max(uint32_t(1), extDiam);

    uint32_t slices = ringDiam * smooth;

    float deg2rad = glm::pi<float>() / 180.f,
          extRad = extDiam / 2.f,
          extRadianStep = 360.f / slices * deg2rad;
    size_t vsz = 0,
           isz = 0,
           vOffs = 0,
           iOffs = 0;

    std::vector<glm::vec3> commVertices, vertices;
    std::vector<int> commIndices, indices;

    auto baseObj = [](float ringDiam,
                      float extDiam,
                      float beginExtRadian,
                      float endExtRadian,
                      uint32_t degStep,
                      uint32_t vOffs,
                      std::vector<glm::vec3>& outVertices,
                      std::vector<int>& outIndices){

      degStep = std::max(uint32_t(1), std::min(uint32_t(120), degStep));

      float deg2rad = glm::pi<float>() / 180.f,
            ringRad = ringDiam / 2.f,
            extRad = extDiam / 2.f;

      size_t vsz = 360 / degStep * 2;

      outVertices.resize(vsz);

      for (int i = 0; i < vsz / 2; ++i){

        GLfloat deg = i * degStep * deg2rad;

        float dYb = ringRad * sin(deg),
              dYe = ringRad * sin(deg + degStep / 2.f * deg2rad);

        outVertices[i * 2].x = extRad * cos(beginExtRadian) + dYb * cos(beginExtRadian);
        outVertices[i * 2].y = ringRad * sin(deg) + extRad * sin(beginExtRadian) - dYb * (1 - sin(beginExtRadian));
        outVertices[i * 2].z = ringRad * cos(deg);

        outVertices[i * 2 + 1].x = extRad * cos(endExtRadian) + dYe * cos(endExtRadian);
        outVertices[i * 2 + 1].y = ringRad * sin(deg + degStep / 2.f * deg2rad) + extRad * sin(endExtRadian) - dYe * (1 - sin(endExtRadian));
        outVertices[i * 2 + 1].z = ringRad * cos(deg + degStep / 2.f * deg2rad);
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
    };

    for (uint32_t i = 0; i < slices; ++i){
     
       baseObj(ringDiam,
                extDiam,
      i * extRadianStep,
(i + 1) * extRadianStep,
                degStep,      
        uint32_t(vOffs),
               vertices,
               indices);

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

    _fillGLObject(obj, vsz, commVertices.data(), isz, commIndices.data());

    return isz;
  }
  
  /// parallepd
  /// @param[in] obj - glObject struct
  /// @param[in] width - width of figure
  /// @param[in] height - height of figure
  /// @param[in] leng - leng of figure
  /// @return Index Buffer size
  static size_t parallepd(const glObject& obj, uint32_t width, uint32_t height, uint32_t leng){

    std::vector<glm::vec3> vertices{ glm::vec3(0, 0, 0),             // 0 left  top    front 
                                     glm::vec3(width, 0, 0),         // 1 right top    front
                                     glm::vec3(0, height, 0),        // 2 left  bottom front
                                     glm::vec3(width, height, 0),    // 3 right bottom front
                                     glm::vec3(0, 0, leng),          // 4 left  top    back 
                                     glm::vec3(width, 0, leng),      // 5 right top    back
                                     glm::vec3(0, height, leng),     // 6 left  bottom back
                                     glm::vec3(width, height, leng), // 7 right bottom back
                                   };

    std::vector<int> indices{ 0, 1, 5,
                              0, 4, 5,
                              1, 3, 5,
                              3, 5, 7, 
                              3, 6, 7,
                              3, 2, 6,
                              0, 2, 6,
                              0, 4, 6,
                              0, 1, 3,
                              0, 2, 3,
                              5, 6, 7,
                              4, 5, 6,
                            };

    size_t vsz = 8,
           isz = 36;

    ////////////////////////////////////////       

    _fillGLObject(obj, vsz, vertices.data(), isz, indices.data());

    return isz;
  
  }   
}