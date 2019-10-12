/*

Sandpile

*/

#include <GL/gl.h>
#include <GL/glut.h>

#include <cmath>
#include <iostream>
#include <chrono>

#define ROWS 300
#define COLUMNS 300
#define CELL_SIZE 2
#define FPS 1
#define SEED 40000


// since symetric, only do a quarter
// the reflections are drawn on display_callback
// 2.2 times faster
double sand[ROWS/2+1][COLUMNS/2+1] = {0};
double sand2[ROWS/2+1][COLUMNS/2+1] = {0};

void timer_callback(int);
void display_callback();
void reshape_callback(int width, int height);

void init() {
    glClearColor(0.0, 0.0, 0.5, 1.0);
    sand2[ROWS/2-1][COLUMNS/2-1] = SEED;
}

bool is_completed() {
    for (int i=0; i<ROWS/2; i++) {
        for (int j=0; j<COLUMNS/2; j++) {
            if (sand2[i][j] > 4)
                return false;
        }
    }
    return true;
}

void update() {
    for (int i=1; i<ROWS/2; i++) {
        for (int j=1; j<COLUMNS/2; j++) {
            if (sand2[i][j] <= 4)
                continue;
            double val(sand2[i][j]/4);
            // if on bottom, the sand given is "reflected" against the border
            // to avoid side effect
            if (i==(ROWS/2-1)) {
                sand[i-1][j] += val*2;
            }
            else {
                sand[i-1][j] += val;
                sand[i+1][j] += val;
            }
            if (j==(COLUMNS/2-1)) {
                sand[i][j-1] += val*2;
            }
            else {
                sand[i][j-1] += val;
                sand[i][j+1] += val;
            }
            sand[i][j] = 0;
        }
    }
    for (int i=1; i<ROWS/2; i++) {
        for (int j=1; j<COLUMNS/2; j++) {
            sand2[i][j] = sand[i][j];
        }
    }
}

void display_callback() {
    glClear(GL_COLOR_BUFFER_BIT);
    for (int i=0; i<ROWS/2; i++) {
        for (int j=0; j<COLUMNS/2; j++) {
            if (sand2[i][j] < 1)
                glColor3f(1, 1, 1);
            else if (sand2[i][j] < 2)
                glColor3f(0, 0, 1);
            else if (sand2[i][j] < 3)
                glColor3f(0, 1, 1);
            else if (sand2[i][j] < 4)
                glColor3f(1, 1, 0);
            else
                glColor3f(1, 0, 0);
            glRectf(j, i, j+1, i+1);
            glRectf(COLUMNS-(j+1), i, COLUMNS-j, i+1);
            glRectf(j, ROWS-(i+1),j+1, ROWS-i);
            glRectf(COLUMNS-(j+1), ROWS-(i+1), COLUMNS-j, ROWS-i);
        }
    }
    glutSwapBuffers();
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, COLUMNS, 0.0, ROWS, -1.0, 1.0); // 0, xmax, 0, ymax, z0, zmax
    glMatrixMode(GL_MODELVIEW);
}


void timer_callback(int) {
    if (is_completed())
        return;
    
    //while (!is_completed())
    for (int i=0; i<10; i++)
        update();
    
    glutPostRedisplay();
    glutTimerFunc(0, timer_callback, 0);
}


int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(30, 30); // optional
    glutInitWindowSize(COLUMNS*CELL_SIZE, ROWS*CELL_SIZE);
    glutCreateWindow("Sandpile");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(1000/FPS, timer_callback, 0);
    init();
    glutMainLoop();
    return 0;
}

