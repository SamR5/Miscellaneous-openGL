/*

    Ray casting

*/

#include <GL/gl.h>
#include <GL/glut.h>

#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>
#include <utility>
#include <random>
#include <ctime>
//#include "Constants.h"

#define WIDTH 800
#define HEIGHT 600
#define FPS 40
#define DIAGO std::sqrt(WIDTH*WIDTH + HEIGHT*HEIGHT)

#define RAY_AMOUNT 200
#define SHAPE_AMOUNT 5
#define PI 3.1415926
#define TWO_PI 2*PI
#define DEG_TO_RAD PI / 180.0
#define RAY_TO_RAD 360.0 * DEG_TO_RAD / RAY_AMOUNT

#define DOT_SIZE 1

typedef std::pair<float, float> pair2f;

// ray = {angle, distance from mouse}
pair2f rays[RAY_AMOUNT];
std::vector<std::vector<int>> shapes;

int mouseX;
int mouseY;

void draw_points();
void draw_rays();
void draw_shapes();


void mouse_motion_callback(int x, int y) {
    mouseX = x;
    mouseY = HEIGHT - y;
}

pair2f intersection(pair2f ray, std::vector<int> line) {
    // the mouse ray line
    int x1(mouseX);
    int y1(mouseY);
    float x2(mouseX + std::cos(ray.first) * ray.second);
    float y2(mouseY + std::sin(ray.first) * ray.second);
    // the shape line
    int x3(line[0]);
    int y3(line[1]);
    int x4(line[2]);
    int y4(line[3]);
    float denominator = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4);
    // if parallel
    if (denominator == 0) {
        return pair2f (0, 0);
    }
    // t
    float t = (x1-x3) * (y3-y4) - (y1-y3) * (x3-x4);
    t /= denominator;
    // u
    float u = -((x1-x2) * (y1-y3) - (y1-y2) * (x1-x3));
    u /= denominator;
    // if intersection
    if (0<=t && t<=1 && 0<=u && u<=1) {
        return pair2f (x1 + t * (x2-x1),
                       y1 + t * (y2-y1));
    }
    // if intersection outside but of one segment
    return pair2f (0, 0);
}

float distance_to_mouse(pair2f A) {
    int deltaX = A.first - mouseX;
    int deltaY = A.second - mouseY;
    return std::sqrt(deltaX*deltaX + deltaY*deltaY);
}

void find_intersec_points() {
    for (int j=0; j<RAY_AMOUNT; j++) {
        pair2f tempP;
        float dst;
        // reset the ending point
        rays[j].second = DIAGO;
        bool intersecFound = false;
        float minDst(1e20);
        for (int i=0; i<shapes.size(); i++) {
            tempP = intersection(rays[j], shapes[i]);
            // if intersection
            if (tempP.first != 0 || tempP.second != 0) {
                intersecFound = true;
                dst = distance_to_mouse(tempP);
                if (dst < minDst) {
                    minDst = dst;
                }
            }
        }
        if (intersecFound)
            rays[j].second = minDst;
    }
}


void draw_shapes() {
    glColor3f(1.0, 0, 0);
    for (std::vector<int> sh : shapes) {
        glBegin(GL_LINES);
          glVertex2f(sh[0], sh[1]);
          glVertex2f(sh[2], sh[3]);
        glEnd();
    }
}
float RAY_COLOR_ALPHA[] = {1.0f, 1.0f, 1.0f, 1.0f};
void draw_rays() {
    glColor4fv(RAY_COLOR_ALPHA);
    float endPointX, endPointY;
    for (int i=0; i<RAY_AMOUNT; i++) {
        endPointX = mouseX + std::cos(rays[i].first) * rays[i].second;
        endPointY = mouseY + std::sin(rays[i].first) * rays[i].second;
        glBegin(GL_LINES);
          glVertex2f(mouseX, mouseY);
          glVertex2f(endPointX, endPointY);
        glEnd();
        if (DOT_SIZE < 2)
            continue;
        glBegin(GL_POLYGON);
          for (float j=0; j<TWO_PI; j+=TWO_PI/6) {
              glVertex2f(endPointX + std::cos(j)*DOT_SIZE,
                         endPointY + std::sin(j)*DOT_SIZE);
          }
        glEnd();
    }
}

void init_rays() {
    for (int i=0; i<RAY_AMOUNT; i++) {
        rays[i] = pair2f (i * RAY_TO_RAD, DIAGO);
    }
}

void init_shapes() {
    shapes.clear();
    for (int i=0; i<SHAPE_AMOUNT; i++) {
        shapes.push_back({rand()%WIDTH, rand()%HEIGHT, rand()%WIDTH, rand()%HEIGHT});
    }
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    std::srand(std::time(0));
    init_rays();
    init_shapes();

    /*
    shapes.push_back({100, 100, 300, 100});
    shapes.push_back({300, 100, 300, 300});
    shapes.push_back({300, 300, 100, 300});
    shapes.push_back({100, 300, 100, 100});
    */
}

void display_callback() {
    glClear (GL_COLOR_BUFFER_BIT);

    draw_shapes();
    draw_rays();

    glFlush();
    glutSwapBuffers();
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (double)WIDTH, 0.0, (double)HEIGHT, -1.0, 0.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer_callback(int) {
    auto start(std::chrono::steady_clock::now());
    find_intersec_points();
    glutPostRedisplay(); // run the display_callback function
    auto stop(std::chrono::steady_clock::now());
    auto duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start));
    glutTimerFunc(std::abs(1000.0/FPS - duration.count()), timer_callback, 0);
}

void keyboard_callback(unsigned char c, int x, int y) {
    if (c == ' ')
        return init_shapes();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(15, 15); // optional
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Ray Casting");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(1000/FPS, timer_callback, 0);
    glutPassiveMotionFunc(mouse_motion_callback);
    glutKeyboardFunc(keyboard_callback);
    init();
    glutMainLoop();
    return 0;
}
