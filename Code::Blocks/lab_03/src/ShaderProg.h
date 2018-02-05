//
//  ShaderProg.h
//  Lab0102
//
//  Created by Zhao Rui on 2018-01-31.
//  Copyright © 2018 Zhao Rui. All rights reserved.
//

#ifndef ShaderProg_h
#define ShaderProg_h
// GLEW
#include "common/stdafx.h"

class ShaderProg{
public:
    unsigned int ID;

    ShaderProg(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath);
    void use();
    void setBoolean(const std::string &name, bool value)const;
    void setInt(const std::string &name, int value)const;
    void setFloat(const std::string &name, float value)const;
    void setVec2(const std::string &name, const glm::vec2 &value)const;
    void setVec2(const std::string &name, float x,float y)const;
    void setVec3(const std::string &name, const glm::vec3 &value)const;
    void setVec3(const std::string &name, float x, float y, float z)const;
    void setVec4(const std::string &name, const glm::vec4 &value)const;
    void setVec4(const std::string &name, float x, float y, float z, float w)const;
    void setMat2(const std::string &name, const glm::mat2 &mat)const;
    void setMat3(const std::string &name, const glm::mat3 &mat)const;
    void setMat4(const std::string &name, const glm::mat4 &mat)const;


private:
    void checkCompileErrors(GLuint shader, std::string type);
};



#endif /* ShaderProg_h */


