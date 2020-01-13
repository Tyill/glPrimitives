
#pragma once

#include "stdafx.h"
#include <glew/include/glew.h>
#include <glm/include/gtc/type_ptr.hpp>

class GLShader
{
public:
	GLuint ProgramId = 0;
	
  GLShader() = default;
  ~GLShader(){

    if (ProgramId > 0)
      glDeleteProgram(ProgramId);
  }
	
  bool setCode(const char* vertexCode, const char* fragmentCode, std::string& out_error){

    std::string vers = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (vers.size() >= 4)
      vers = std::string() + vers[0] + vers[2] + vers[3];

    // vertex shader
    std::string vertexStr = std::string("#version ") + vers + " core \n" + vertexCode;
    const GLchar* vertShCode = vertexStr.c_str();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertShCode, NULL);
    glCompileShader(vertex);
    out_error = checkErrors(vertex, "VERTEX");

    if (!out_error.empty()){
      out_error = "GLShader vertex shader " + out_error;
      return false;
    }

    // fragment shader
    std::string fragmentStr = std::string("#version ") + vers + " core \n" + fragmentCode;
    const GLchar* fragmCode = fragmentStr.c_str();

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmCode, NULL);
    glCompileShader(fragment);
    out_error = checkErrors(fragment, "FRAGMENT");

    if (!out_error.empty()){
      out_error = "GLShader fragment shader " + out_error;
      return false;
    }

    // shader program
    ProgramId = glCreateProgram();
    glAttachShader(ProgramId, vertex);
    glAttachShader(ProgramId, fragment);

    glBindAttribLocation(ProgramId, 0, "position");
    glBindAttribLocation(ProgramId, 1, "color");

    glLinkProgram(ProgramId);
    out_error = checkErrors(ProgramId, "PROGRAM");

    if (!out_error.empty()){
      out_error = "GLShader shader program " + out_error;
      return false;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return true;
  }

	void use(){
    if (ProgramId > 0)
		  glUseProgram(ProgramId);
	}

	bool setParam(const std::string &name, bool value) const{

    GLint inx = glGetUniformLocation(ProgramId, name.c_str());
   
    if (inx >= 0){
      glUseProgram(ProgramId);
      glUniform1i(inx, (int)value);
    }
    return inx >= 0;
	}

  bool setParam(const std::string &name, int value) const {

    GLint inx = glGetUniformLocation(ProgramId, name.c_str());
        
    if (inx >= 0){
      glUseProgram(ProgramId);
      glUniform1i(inx, value);
    }
    return inx >= 0;
	}

  bool setParam(const std::string &name, float value) const	{

    GLint inx = glGetUniformLocation(ProgramId, name.c_str());
     
    if (inx >= 0){
      glUseProgram(ProgramId);
      glUniform1f(inx, value);
    }
    return inx >= 0;
	}

  bool setParam(const std::string &name, const glm::mat4& value) const{

    GLint inx = glGetUniformLocation(ProgramId, name.c_str());
       
    if (inx >= 0){
      glUseProgram(ProgramId);
      glUniformMatrix4fv(inx, 1, GL_FALSE, glm::value_ptr(value));
    }
    return inx >= 0;
	}

  bool setParam(const std::string &name, const glm::vec3& value) const {

    GLint inx = glGetUniformLocation(ProgramId, name.c_str());
       
    if (inx >= 0){
      glUseProgram(ProgramId);
      glUniform3fv(glGetUniformLocation(ProgramId, name.c_str()), 1, glm::value_ptr(value));
    }
    return inx >= 0;
	}

private:
		
	std::string checkErrors(unsigned int shader, const std::string& type){

		int success = 0;
    char infoLog[1024] = {};
		if (type != "PROGRAM"){

			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success){

				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				return "compilation error of type " + type + " info " + infoLog;
			}
		}
		else{

			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success){

				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				return "linking error of type " + type + " info " + infoLog;
			}
		}

		return "";
	}	
};
