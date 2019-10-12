/*

*/

#include <GL/gl.h>
#include <GL/glut.h>

//#include "Constants.h"
#include <cmath>
#include <iostream>
#include <chrono>

#define HEIGHT 600
#define WIDTH 800
#define FPS 60


double current[HEIGHT+2][WIDTH+2] = {0};
double previous[HEIGHT+2][WIDTH+2] = {0};
double dampening(0.98);
double currentColor[3];

void timer_callback(int);
void display_callback();
void reshape_callback(int width, int height);
void mouse_callback(int state, int button, int x, int y);

void update() {
    for (int i=1; i<HEIGHT-1; i++) {
        for (int j=1; j<WIDTH-1; j++) {
            current[i][j] = (previous[i+1][j] +
                             previous[i-1][j] +
                             previous[i][j+1] +
                             previous[i][j-1])/2 -
                             current[i][j];
            current[i][j] *= dampening;
        }
    }
}

void swap_grid() {
    double tempVal;
    for (int i=0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH; j++) {
            tempVal = current[i][j];
            current[i][j] = previous[i][j];
            previous[i][j] = tempVal;
        }
    }
}

void init() {
    glClearColor(0.0, 0.0, 0.5, 1.0);
}

void display_callback() {
    glClear(GL_COLOR_BUFFER_BIT);
    for (int i=0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH; j++) {
            double c(current[i][j]);
            glColor3d(c, c, c);
            glRectf(j, i, j+1, i+1);
        }
    }
    glutSwapBuffers();
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, WIDTH, 0.0, HEIGHT, -1.0, 1.0); // 0, xmax, 0, ymax, z0, zmax
    glMatrixMode(GL_MODELVIEW);
}

void timer_callback(int) {
    auto start(std::chrono::steady_clock::now());

    for (int i=0; i<5; i++) {
        update();
        swap_grid();
    }
    glutPostRedisplay();
    
    auto stop(std::chrono::steady_clock::now());
    auto duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start));

    glutTimerFunc(std::abs(1000.0/FPS - duration.count()), timer_callback, 0);
}

void mouse_callback(int state, int button, int x, int y) {
    if (state == 1 || button!=0)
        return;
    y = HEIGHT-y;
    previous[y][x] = 100;
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(30, 30); // optional
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Blob");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(1000/FPS, timer_callback, 0);
    glutMouseFunc(mouse_callback);
    init();
    glutMainLoop();
    return 0;
}
