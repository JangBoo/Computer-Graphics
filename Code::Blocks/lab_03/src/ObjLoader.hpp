//
//  ObjLoader.hpp
//  Lab0102
//
//  Created by Zhao Rui on 2018-02-01.
//  Copyright © 2018 Zhao Rui. All rights reserved.
//

#include "common/stdafx.h"
#ifndef ObjLoader_h
#define ObjLoader_h

#pragma warning(disable:4996)

bool loadOBJ(
             const char * path,
             std::vector<glm::vec3> & out_vertices,
             std::vector<glm::vec3> & out_normals,
             std::vector<glm::vec2> & out_uvs);


#endif /* ObjLoader_h */
