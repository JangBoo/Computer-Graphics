//
//  ShaderProg.cpp
//  Lab0102
//
//  Created by Zhao Rui on 2018-02-01.
//  Copyright © 2018 Zhao Rui. All rights reserved.
//

#include "ShaderProg.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

ShaderProg::ShaderProg(const char* vertexShaderPath, const char* fragmentShaderPath)
{
    string vertexCode, fragCode;
    stringstream vertexStream, fragmentStream;
    ifstream vertexInput(vertexShaderPath);
    if(!vertexInput)
    {
        cout<<"Cannot open vertex shader file."<<endl;
        exit(EXIT_FAILURE);
    }
    ifstream fragmentInput(fragmentShaderPath);
    if(!fragmentInput)
    {
        cout<<"Cannot open fragment shader file."<<endl;
    }
    
    vertexStream<<vertexInput.rdbuf();
    fragmentStream<<fragmentInput.rdbuf();
    
    vertexCode=vertexStream.str();
    fragCode=fragmentStream.str();
    
    const char* vShaderCode=vertexCode.c_str();
    const char* fShaderCode=fragCode.c_str();
    
    vertexInput.close();
    fragmentInput.close();
    
    //compile
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
    
    //vertex shader
    vertex=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex,1,&vShaderCode,NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    
    
    //fragment shader
    fragment=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment,1,&fShaderCode,NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    
    //link
    ID=glCreateProgram();
    glAttachShader(ID,vertex);
    glAttachShader(ID,fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void ShaderProg::use()
{
    glUseProgram(ID);
}

void ShaderProg::setBoolean(const string &name, bool value)const
{
    glUniform1i(glGetUniformLocation(ID,name.c_str()),(int)value);
}

void ShaderProg::setInt(const string &name, int value)const
{
    glUniform1i(glGetUniformLocation(ID,name.c_str()),value);
}

void ShaderProg::setFloat(const string &name, float value)const
{
    glUniform1f(glGetUniformLocation(ID,name.c_str()),value);
}

void ShaderProg::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void ShaderProg::setVec2(const std::string &name, float x,float y)const
{
    glUniform2f(glGetUniformLocation(ID,name.c_str()),x,y);
}
void ShaderProg::setVec3(const std::string &name, const glm::vec3 &value)const
{
    glUniform3fv(glGetUniformLocation(ID,name.c_str()),1,&value[0]);
}
void ShaderProg::setVec3(const std::string &name, float x, float y, float z)const
{
    glUniform3f(glGetUniformLocation(ID,name.c_str()),x,y,z);
}
void ShaderProg::setVec4(const std::string &name, const glm::vec4 &value)const
{
    glUniform4fv(glGetUniformLocation(ID,name.c_str()),1,&value[0]);
}
void ShaderProg::setVec4(const std::string &name, float x, float y, float z, float w)const
{
    glUniform4f(glGetUniformLocation(ID,name.c_str()),x,y,z,w);
}
void ShaderProg::setMat2(const std::string &name, const glm::mat2 &mat)const
{
    glUniformMatrix2fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,&mat[0][0]);
}
void ShaderProg::setMat3(const std::string &name, const glm::mat3 &mat)const
{
    glUniformMatrix3fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,&mat[0][0]);
}
void ShaderProg::setMat4(const std::string &name, const glm::mat4 &mat)const
{
    glUniformMatrix4fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,&mat[0][0]);
}

void ShaderProg::checkCompileErrors(GLuint shader, string type)
{
    GLint success;
    GLchar infoLog[1024];
    if(type!="PROGRAM")
    {
        glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
        if(!success)
        {
            glGetShaderInfoLog(shader,1024,NULL,infoLog);
            cout<<"ERROR::SHADER_COMPILATION_ERROR of type: "<<type<<"\n"<<infoLog<<"\n -- --------------------------------------------------- -- " << endl;
        }
    }
    else
    {
        glGetProgramiv(shader,GL_LINK_STATUS,&success);
        if(!success)
        {
            glGetProgramInfoLog(shader,1024,NULL,infoLog);
            cout<<"ERROR::PROGRAM_LINKING_ERROR of type: "<<type<<"\n"<<infoLog<<"\n -- --------------------------------------------------- -- " << endl;
        }
    }
}
