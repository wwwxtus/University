#include "lighting_and_textures.h"
#include <iostream>
#include "models.h"

GLuint loadTexture(const char* filename) {
    FILE* file;
    errno_t err = fopen_s(&file, filename, "rb");
    if (err != 0 || !file) {
        printf("Failed to open texture file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    unsigned char header[54];
    if (fread(header, sizeof(unsigned char), 54, file) != 54) {
        printf("Invalid BMP file: %s\n", filename);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (header[0] != 'B' || header[1] != 'M') {
        printf("Not a BMP file: %s\n", filename);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int imageSize = width * height * 3;

    unsigned char* data = new unsigned char[imageSize];
    if (fread(data, sizeof(unsigned char), imageSize, file) != imageSize) {
        printf("Error reading BMP data: %s\n", filename);
        delete[] data;
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);

    for (int i = 0; i < imageSize; i += 3) {
        std::swap(data[i], data[i + 2]);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] data;
    return textureID;
}

void createShadowMatrix(GLfloat shadowMat[4][4], const GLfloat lightPos[4], const GLfloat plane[4]) {
    GLfloat dot = plane[0] * lightPos[0] + plane[1] * lightPos[1] + plane[2] * lightPos[2] + plane[3] * lightPos[3];

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            shadowMat[i][j] = dot - (i == 0 ? plane[j] * lightPos[i] : plane[i] * lightPos[j]);
        }
    }
}

void drawShadow(const std::vector<Checker>& checkers, bool isBlack, const GLfloat shadowMat[4][4]) {
    glPushMatrix();
    glMultMatrixf(&shadowMat[0][0]);

    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.0f, 0.0f);

    for (const auto& checker : checkers) {
        drawChecker(checker.x * cellSize, checker.y * cellSize, isBlack);
    }

    glEnable(GL_LIGHTING);
    glPopMatrix();
}

