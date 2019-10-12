/*

Islamic tiles

*/

#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <cmath>
#include <chrono>
#include <sstream>

#define WIDTH 800
#define HEIGHT 400
#define FPS 30

#define ROWS 6
#define COLUMNS 6


void display_callback();
void reshape_callback(int width, int height);
void timer_callback(int);


struct Tile {
    float x, y;
} mainTile;

void init() {
    glClearColor(1, 1, 1, 1.0);
    mainTile = {0, 0};
}


void draw_parameters() {
    std::stringstream sn;
    std::stringstream sd;
    sn << "N: ";
    sd << "D: ";
    glRasterPos2f(-1, -1);
    for (char c : sn.str()) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }
    glRasterPos2f(-1, -0.93);
    for (char c : sd.str()) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }
}

void draw_tile(float Cx, float Cy, float x, float y,
               float w, float h) {
    float right(Cx+w/2), left(Cx-w/2);
    float top(Cy+h/2), bottom(Cy-h/2);

    glBegin(GL_LINES);
      // down right
      glVertex2f(Cx + x, bottom);
      glVertex2f(right, Cy - y);
      // up right
      glVertex2f(right, Cy + y);
      glVertex2f(Cx + x, top);
      // up left
      glVertex2f(Cx - x, top);
      glVertex2f(left, Cy + y);
      // down left
      glVertex2f(left, Cy - y);
      glVertex2f(Cx - x, bottom);
    glEnd();
}

void draw_tiles() {
    // the main tile on the left
    draw_tile(WIDTH/4, HEIGHT/2,
              mainTile.x, mainTile.y,
              WIDTH/2, HEIGHT);
    // copies on a grid to the right

    for (float i=0; i<ROWS; i++) {
        for (float j=0; j<COLUMNS; j++) {
            draw_tile(WIDTH/2 + (j+0.5) * (WIDTH/2)/COLUMNS,
                      (i + 0.5) * HEIGHT/ROWS,
                      mainTile.x / ROWS, mainTile.y / COLUMNS,
                      WIDTH/(2*COLUMNS), HEIGHT/ROWS);
        }
    }

    glBegin(GL_LINES);
      glVertex2f(WIDTH/2, 0);
      glVertex2f(WIDTH/2, HEIGHT);
    glEnd();
}

void draw_lines() {
    glColor3f(0.7, 0.7, 0.7);
    glBegin(GL_LINES);
    for (float i=1; i<ROWS; i++) {
        glVertex2f(WIDTH/2, i * HEIGHT/ROWS);
        glVertex2f(WIDTH, i * HEIGHT/ROWS);
    }
    for (float j=1; j<COLUMNS; j++) {
        glVertex2f(WIDTH/2 + j * (WIDTH/2)/COLUMNS, 0);
        glVertex2f(WIDTH/2 + j * (WIDTH/2)/COLUMNS, HEIGHT);
    }
    glEnd();
}


void display_callback() {
    glClear(GL_COLOR_BUFFER_BIT); // clear screen

    glColor3f(0, 0, 0);

    //glColor3f(0.3, 0.3, 1);
    //draw_parameters();

    draw_tiles();
    draw_lines();

    glutSwapBuffers(); // display / update
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float margin(0);
    glOrtho(0-margin, WIDTH+margin, 0-margin, HEIGHT+margin, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer_callback(int) {
    auto start(std::chrono::steady_clock::now());

    glutPostRedisplay(); // run the display_callback function

    auto stop(std::chrono::steady_clock::now());
    auto duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start));

    glutTimerFunc(std::abs(1000.0/FPS - duration.count()), timer_callback, 0);
}

void keyboard_special_callback(int special, int x, int y) {
    switch (special) {
        case GLUT_KEY_RIGHT: mainTile.x++; break;
        case GLUT_KEY_LEFT: mainTile.x--; break;
        case GLUT_KEY_UP: mainTile.y++; break;
        case GLUT_KEY_DOWN: mainTile.y--; break;
    }
    //glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(30, 30); // optional
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Maurer Rose");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(1000/FPS, timer_callback, 0);
    glutSpecialFunc(keyboard_special_callback);
    init();
    glutMainLoop();
    return 0;
}
