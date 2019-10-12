/*

Maurer rose

*/

#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <cmath>
#include <chrono>
#include <sstream>

#define WIDTH 600
#define HEIGHT 600
#define FPS 1

#define PI 3.14159265
#define DEG_TO_RAD PI/180.0

// changeable later
int n = 3;
int d = 3;

float points[361][2];
float rosePoints[361][2];

void display_callback();
void reshape_callback(int width, int height);
void timer_callback(int);


void update_rose_points() {
    for (int i=0; i<361; i++) {
        rosePoints[i][0] = points[i][0];
        rosePoints[i][1] = points[i][1];
    }
    float temp;
    bool change(true);
    while (change) {
        change = false;
        for (int i=0; i<360; i++) {
            if (rosePoints[i][1] > rosePoints[i+1][1]) {
                temp = rosePoints[i][0];
                rosePoints[i][0] = rosePoints[i+1][0];
                rosePoints[i+1][0] = temp;
                temp = rosePoints[i][1];
                rosePoints[i][1] = rosePoints[i+1][1];
                rosePoints[i+1][1] = temp;
                change = true;
            }
        }
    }
}

void init() {
    glClearColor(1, 1, 1, 1.0);
}

void update_maurer_points() {
    float k;
    for (int i=0; i<360; i++) {
        k = (i*d)%360;
        points[i][0] = std::sin(n * k * DEG_TO_RAD);
        points[i][1] = k * DEG_TO_RAD;
    }
}

void draw_rose() {
    float r, theta;
    glBegin(GL_LINE_LOOP);
    for (int i=0; i<=360; i++) {
        r = rosePoints[i][0];
        theta = rosePoints[i][1];
        glVertex2f(r * std::cos(theta),
                   r * std::sin(theta));
    }
    glEnd();
}

void draw_maurer_rose() {
    float r, theta;
    glBegin(GL_LINE_LOOP);
    for (int i=0; i<=360; i++) {
        r = points[i][0];
        theta = points[i][1];
        glVertex2f(r * std::cos(theta),
                   r * std::sin(theta));
    }
    glEnd();
}

void draw_parameters() {
    std::stringstream sn;
    std::stringstream sd;
    sn << "N: " << n;
    sd << "D: " << d;
    glRasterPos2f(-1, -1);
    for (char c : sn.str()) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }
    glRasterPos2f(-1, -0.93);
    for (char c : sd.str()) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }

}

void display_callback() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0, 0, 0);
    draw_rose(); // the curve

    glColor3f(1, 0.3, 0.3);
    draw_maurer_rose(); // the path

    glColor3f(0.3, 0.3, 1);
    draw_parameters();


    glutSwapBuffers();
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float margin(0.1);
    glOrtho(-1-margin, 1+margin,
            -1-margin, 1+margin,
            -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard_special_callback(int special, int x, int y) {
    switch (special) {
        case GLUT_KEY_UP: d++; break;
        case GLUT_KEY_DOWN: d--; break;
        case GLUT_KEY_RIGHT: n++; break;
        case GLUT_KEY_LEFT: n--; break;
    }
    update_maurer_points();
    update_rose_points();
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(30, 30); // optional
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Maurer Rose");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutSpecialFunc(keyboard_special_callback);
    init();
    glutMainLoop();
    return 0;
}
