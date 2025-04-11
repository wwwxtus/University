#ifndef LIGHTING_AND_TEXTURES_H
#define LIGHTING_AND_TEXTURES_H

#include <vector>
#include "glew.h"
#include "models.h"

GLuint loadTexture(const char* filename);
void createShadowMatrix(GLfloat shadowMat[4][4], const GLfloat lightPos[4], const GLfloat plane[4]); 
void drawShadow(const std::vector<Checker>& checkers, bool isBlack, const GLfloat shadowMat[4][4]);
 
#endif // LIGHTING_AND_TEXTURES_H
