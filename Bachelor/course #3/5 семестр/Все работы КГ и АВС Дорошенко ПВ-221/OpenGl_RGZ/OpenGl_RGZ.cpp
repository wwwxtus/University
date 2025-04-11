#include <windows.h>
#include <vector>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "lighting_and_textures.h"
#include "models.h"
#include "view_and_projection.h"

float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationZ = 0.0f;
float scale = 1.0f;

bool boardRotated = false;
bool isDragging = false;
int lastX = 0, lastY = 0;

GLuint whiteCheckerTexture, woodenCheckerTexture, redTexture;

std::vector<Checker> whiteCheckers;
std::vector<Checker> blackCheckers;

void drawCheckers() {
    for (const auto& checker : whiteCheckers) {
        drawChecker(checker.x * cellSize, checker.y * cellSize, false);
    }
    for (const auto& checker : blackCheckers) {
        drawChecker(checker.x * cellSize, checker.y * cellSize, true);
    }
}

void initGame() {
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            if ((row + col) % 2 == 1) {
                blackCheckers.push_back({ true, false, (float)col, (float)row });
            }
        }
    }

    for (int row = 5; row < 8; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            if ((row + col) % 2 == 1) {
                whiteCheckers.push_back({ false, false, (float)col, (float)row });
            }
        }
    }
}

void drawTableSurface(float boardSize, float cellSize) {
    glEnable(GL_TEXTURE_2D);
    glColor3f(0.4f, 0.2f, 0.1f);

    glBindTexture(GL_TEXTURE_2D, woodenCheckerTexture);

    float halfSize = boardSize * cellSize / 2.0f;
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(halfSize, -halfSize, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(halfSize, halfSize, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize, halfSize, 0.0f);

    glVertex3f(-halfSize, -halfSize, -0.1f);
    glVertex3f(halfSize, -halfSize, -0.1f);
    glVertex3f(halfSize, halfSize, -0.1f);
    glVertex3f(-halfSize, halfSize, -0.1f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawTable(float boardSize, float cellSize) {
    drawTableSurface(20, cellSize);

    float legWidth = 0.5f, legDepth = 0.5f, legHeight = 10.5f;
    float halfSize = 20 * cellSize / 2.0f;

    drawLeg(-halfSize, -halfSize, -0.1f, legWidth, legDepth, legHeight);
    drawLeg(halfSize - legWidth, -halfSize, -0.1f, legWidth, legDepth, legHeight);
    drawLeg(-halfSize, halfSize - legDepth, -0.1f, legWidth, legDepth, legHeight);
    drawLeg(halfSize - legWidth, halfSize - legDepth, -0.1f, legWidth, legDepth, legHeight);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(10.0f, 10.0f, 10.0f, 4.0f, 4.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    glPushMatrix();

    glTranslatef(boardSize * cellSize / 2.0f, boardSize * cellSize / 2.0f, 0.0f);

    if (boardRotated) {
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    }

    glTranslatef(-boardSize * cellSize / 2.0f, -boardSize * cellSize / 2.0f, 0.0f);

    glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotationZ, 0.0f, 0.0f, 1.0f);

    glScalef(scale, scale, scale);

    GLfloat plane[4] = { 0.0f, 0.0f, 1.0f, -0.5f };
    GLfloat lightPos[4] = { 0.0f, 0.0f, 20.0f, 1.0f };
    GLfloat shadowMat[4][4];
    createShadowMatrix(shadowMat, lightPos, plane);

    drawShadow(whiteCheckers, false, shadowMat);
    drawShadow(blackCheckers, true, shadowMat);

    drawTable(boardSize, cellSize);
    drawBoard();
    drawCheckers();

    drawChair(4.0f, 10.0f, -3.0f);
    drawChair(4.0f, -12.0f, -3.0f);

    glPopMatrix();
    glutSwapBuffers();
}

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightDiffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat lightSpecular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat lightPosition[] = { 0.0f, 0.0f, 20.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    woodenCheckerTexture = loadTexture("checkers.bmp");
    whiteCheckerTexture = loadTexture("white.bmp");
    redTexture = loadTexture("red.bmp");

    if (whiteCheckerTexture == 0) {
        printf("Failed to load white checker texture!\n");
    }
    if (woodenCheckerTexture == 0) {
        printf("Failed to load wooden checker texture!\n");
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'r':
        boardRotated = !boardRotated;
        break;
    case 'q':
        rotationZ += 5.0f;
        break;
    case 'e':
        rotationZ -= 5.0f;
        break;
    case 'w':
        rotationX += 5.0f;
        break;
    case 's':
        rotationX -= 5.0f;
        break;
    case 'a':
        rotationY -= 5.0f;
        break;
    case 'd':
        rotationY += 5.0f;
        break;
    case 'z':
        scale *= 1.1f;
        break;
    case 'x':
        scale /= 1.1f;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isDragging = true;
        lastX = x;
        lastY = y;
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        isDragging = false;
    }
}

void motion(int x, int y) {
    if (isDragging) {
        int dx = x - lastX;
        int dy = y - lastY;

        rotationX += dy * 0.1f;
        rotationY += dx * 0.1f;

        lastX = x;
        lastY = y;

        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("3D Checkers");

    initOpenGL();
    initGame();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();
    
    return 0;
}