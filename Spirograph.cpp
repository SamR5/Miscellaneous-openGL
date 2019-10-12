/*

Spirograph

*/

#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <cmath>
#include <chrono>
#include <sstream>
#include <utility>
#include <vector>

#define WIDTH 400
#define HEIGHT 400
#define FPS 30

#define TWO_PI 6.283185
#define INI_RADIUS 0.5
#define PRECISION 10000

typedef std::pair<double, double> pair2d;

void init();
void compute_path();
void parameter_increment(int incr);

void draw_parameters();
void draw_circle(double x, double y, double radius);
void draw_path();
void draw_hollow_rect(float x1, float y1, float x2, float y2);

void display_callback();
void reshape_callback(int width, int height);


struct Circle {
    double radius;
    double angle;
    double speed;
    Circle* parent;
    pair2d get_xy() {
        if (parent==nullptr) {
            return {0, 0};
        }
        else {
            pair2d P = parent->get_xy();
            return {P.first + parent->radius * std::cos(parent->angle)
                            + radius * std::cos(parent->angle),
                    P.second + parent->radius * std::sin(parent->angle)
                             + radius * std::sin(parent->angle)};
        }
    }
    void update() {
        angle += speed;
    }
    void show() {
        pair2d coord = get_xy();
        draw_circle(coord.first, coord.second, radius);
    }
} mainC, lastC;


std::vector<pair2d> path;
int circleAmount = 8;
int k = 4; // used for the speed
double RadiusRatio = 0.05;
int currentParameter = 0;

void init() {
    glClearColor(0, 0, 0, 1.0);
    mainC = {INI_RADIUS, 0, 1.0/PRECISION, nullptr};
}

void compute_path() {
    // recompute all circles according to parameters
    mainC.angle = 0;
    Circle* next = &mainC;
    for (int n=2; n<circleAmount-1; n++) {
        next = new Circle {next->radius*RadiusRatio, 0,
                           pow(k, n-1)/PRECISION, next};
    }
    lastC = *next;

    path.clear();
    path.reserve(PRECISION * 7);
    while (mainC.angle < TWO_PI) {
        for (int i=0; i<PRECISION; i++) {
            Circle* next = &lastC;
            path.push_back(next->get_xy());
            while (next != nullptr) {
                next->update();
                next = next->parent;
            }
        }
    }
}

void parameter_increment(int incr) {
    switch (currentParameter) {
      case 0: circleAmount += incr; break;
      case 1: k += incr; break;
      case 2: RadiusRatio += 0.05*incr; break;
    }
    if (circleAmount < 4)
        circleAmount = 4;
    if (RadiusRatio < 0.05)
        RadiusRatio = 0.05;
}

void draw_parameters() {
    /*
    Ratio : radiusFactor
    Speed : k
    Circles : circleAmount
    */
    std::stringstream ci, sp, ra;
    ci << "Circles: " << circleAmount;
    sp << "Speed: " << k;
    ra << "Ratio: " << RadiusRatio;
    float bottom(-1.90), left(-1.90), right(-0.8), h(0.20);
    float bottomRec(bottom-0.05);
    float leftRec(left-0.05);
    glColor3f(1, 1, 0);
    glRasterPos2f(left, bottom);
    for (char c : ci.str()) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }
    glRasterPos2f(left, bottom+h);
    for (char c : sp.str()) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }
    glRasterPos2f(left, bottom+h+h);
    for (char c : ra.str()) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }
    switch (currentParameter) {
      case 0: draw_hollow_rect(leftRec, bottomRec, right, bottomRec+h); break;
      case 1: draw_hollow_rect(leftRec, bottomRec+h, right,bottomRec+h+h); break;
      case 2: draw_hollow_rect(leftRec, bottomRec+h+h, right, bottomRec+h+h+h); break;
    }
}

void draw_circle(double x, double y, double radius) {
    glBegin(GL_LINE_LOOP);
    for (double i=0; i<TWO_PI; i+=TWO_PI/100) {
        glVertex2f(x + radius * std::cos(i),
                   y + radius * std::sin(i));
    }
    glEnd();
}

void draw_path() {
    glBegin(GL_LINE_STRIP);
    for (pair2d p : path) {
        glVertex2f(p.first, p.second);
    }
    glEnd();
}

void draw_hollow_rect(float x1, float y1, float x2, float y2) {
    glColor3f(1, 1, 0);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
      glVertex2f(x1, y1);
      glVertex2f(x2, y1);
      glVertex2f(x2, y2);
      glVertex2f(x1, y2);
    glEnd();
    glLineWidth(1);
}

void display_callback() {
    glClear(GL_COLOR_BUFFER_BIT); // clear screen

    glColor3f(0.4, 1, 0.4);
    draw_path();

    /*glColor3f(1, 1, 1);
    Circle* next = &lastC;
    while (next != nullptr) {
        next->show();
        next = next->parent;
    }*/

    glColor3f(0.5, 0.5, 1);
    draw_parameters();

    glutSwapBuffers();
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float margin(0);
    glOrtho(-2-margin, 2+margin, -2-margin, 2+margin, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard_special_callback(int special, int x, int y) {
    switch (special) {
        case GLUT_KEY_RIGHT:
          parameter_increment(+1);
          compute_path(); break;
        case GLUT_KEY_LEFT:
          parameter_increment(-1);
          compute_path(); break;
        case GLUT_KEY_UP: currentParameter++; break;
        case GLUT_KEY_DOWN: currentParameter--; break;
    }
    if (currentParameter < 0)
        currentParameter = 2;
    currentParameter %= 3;
    //glutPostRedisplay();
    display_callback();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(300, 300); // optional
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Spirograph");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutSpecialFunc(keyboard_special_callback);
    init();
    compute_path();
    glutMainLoop();
    return 0;
}
