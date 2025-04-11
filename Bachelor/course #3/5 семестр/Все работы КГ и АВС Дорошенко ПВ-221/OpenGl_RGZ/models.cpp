#include "models.h"

void drawSquare(float x, float y, float size, bool isBlack) {
    glColor3f(isBlack ? 0.0f : 1.0f, isBlack ? 0.0f : 1.0f, isBlack ? 0.0f : 1.0f);
    glBegin(GL_QUADS);
    glEnd();
}

void drawChecker(float x, float y, bool isBlack) {
    float boardHeight = 0.9f;
    glColor3f(isBlack ? 1.0f : 1.0f, isBlack ? 0.0f : 1.0f, isBlack ? 0.0f : 1.0f);

    GLUquadric* quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);

    glPushMatrix();
    glTranslatef(x, y, boardHeight + checkerHeight / 2.0f);
    gluCylinder(quadric, checkerRadius, checkerRadius, checkerHeight, 32, 32);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x, y, boardHeight + checkerHeight);
    gluDisk(quadric, 0.0f, checkerRadius, 32, 1);
    glPopMatrix();

    gluDeleteQuadric(quadric);
}

void drawBoard() {
    float boardHeight = 0.5f;
    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            bool isBlackCell = (row + col) % 2 == 1;
            glPushMatrix();
            glTranslatef(col * cellSize, row * cellSize, boardHeight);
            drawSquare(0.0f, 0.0f, cellSize, isBlackCell);
            glutSolidCube(cellSize);
            glPopMatrix();
        }
    }
}

void drawLeg(float x, float y, float z, float legWidth, float legDepth, float legHeight) {
    glBegin(GL_QUADS);

    glVertex3f(x, y, z);
    glVertex3f(x + legWidth, y, z);
    glVertex3f(x + legWidth, y, z - legHeight);
    glVertex3f(x, y, z - legHeight);

    glVertex3f(x, y + legDepth, z);
    glVertex3f(x + legWidth, y + legDepth, z);
    glVertex3f(x + legWidth, y + legDepth, z - legHeight);
    glVertex3f(x, y + legDepth, z - legHeight);

    glVertex3f(x, y, z);
    glVertex3f(x, y + legDepth, z);
    glVertex3f(x, y + legDepth, z - legHeight);
    glVertex3f(x, y, z - legHeight);

    glVertex3f(x + legWidth, y, z);
    glVertex3f(x + legWidth, y + legDepth, z);
    glVertex3f(x + legWidth, y + legDepth, z - legHeight);
    glVertex3f(x + legWidth, y, z - legHeight);

    glVertex3f(x, y, z - legHeight);
    glVertex3f(x + legWidth, y, z - legHeight);
    glVertex3f(x + legWidth, y + legDepth, z - legHeight);
    glVertex3f(x, y + legDepth, z - legHeight);

    glVertex3f(x, y, z);
    glVertex3f(x + legWidth, y, z);
    glVertex3f(x + legWidth, y + legDepth, z);
    glVertex3f(x, y + legDepth, z);
    glEnd();
}

void drawChair(float x, float y, float z) {
    float legWidth = 0.5f, legDepth = 0.5f, legHeight = 7.5f;
    float seatWidth = 4.0f, seatDepth = 4.0f, seatHeight = 4.0f;
    glColor3f(0.4f, 0.2f, 0.1f);

    drawLeg(x, y, z, legWidth, legDepth, legHeight);
    drawLeg(x + seatWidth - legWidth, y, z, legWidth, legDepth, legHeight);
    drawLeg(x, y + seatDepth - legDepth, z, legWidth, legDepth, legHeight);
    drawLeg(x + seatWidth - legWidth, y + seatDepth - legDepth, z, legWidth, legDepth, legHeight);

    glColor3f(0.4f, 0.2f, 0.1f);
    glBegin(GL_QUADS);
    glVertex3f(x, y, z);
    glVertex3f(x + seatWidth, y, z);
    glVertex3f(x + seatWidth, y + seatDepth, z);
    glVertex3f(x, y + seatDepth, z);
    glEnd();
}