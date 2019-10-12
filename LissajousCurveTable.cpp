/*

    Lissajous curve table

*/

#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <cstdio>
#include <chrono>
#include <cmath>
#include <vector>

#define WIDTH 1000
#define HEIGHT 600
#define FPS 40

#define PI 3.1415926
#define TWO_PI 2*PI

#define CIRCLE_RADIUS 30
#define CIRCLE_OFFSET 20
#define DOT_SIZE 5
#define DELTA_ANGLE TWO_PI/400.0 // variation each step
#define CIRCLE_AMOUNT_HRZ (WIDTH-CIRCLE_RADIUS) / (CIRCLE_RADIUS*2 + CIRCLE_OFFSET)-2
#define CIRCLE_AMOUNT_VRT (HEIGHT-CIRCLE_RADIUS) / (CIRCLE_RADIUS*2 + CIRCLE_OFFSET)-2
#define CURVE_AMOUNT_HRZ CIRCLE_AMOUNT_HRZ
#define CURVE_AMOUNT_VRT CIRCLE_AMOUNT_VRT


float angle;

struct Curve {
    float centerX, centerY;
    float currentX, currentY;
    std::vector<std::pair<float, float>> path;
    void update(int row, int col) {
        currentX = centerX + std::cos(angle*(col+1))*CIRCLE_RADIUS;
        currentY = centerY + std::sin(angle*(row+1))*CIRCLE_RADIUS;
    }
    void add_point() {
        path.push_back(std::pair<float, float> (currentX, currentY));
    }
    void reset() {
        path.clear();
    }
};

struct Circle {
    float centerX, centerY;
    int row, col;
};

void draw_curves();
void draw_circles();
void draw_circle(float x, float y);
void draw_dot_line(Circle);
void draw_dot(float x, float y);


Curve curves[CURVE_AMOUNT_VRT][CURVE_AMOUNT_HRZ];
Circle circles[CIRCLE_AMOUNT_HRZ+CIRCLE_AMOUNT_VRT];

void init_curves() {
    // the row=0 / col=0 are for circles
    for (int row=0; row<CURVE_AMOUNT_VRT; row++) {
        for (int col=0; col<CURVE_AMOUNT_HRZ; col++) {
            Curve c = {(float)(CIRCLE_OFFSET+CIRCLE_RADIUS)*(2*col+3),
                       (float)HEIGHT-(CIRCLE_OFFSET+CIRCLE_RADIUS)*(2*row+3),
                       0.0, 0.0}; // path empty
            curves[row][col] = c;
        }
    }
}

void init_circles() {
    for (int col=0; col<CIRCLE_AMOUNT_HRZ; col++) {
        Circle c = {(float)(CIRCLE_OFFSET+CIRCLE_RADIUS)*(2*col+3),
                    (float)HEIGHT-(CIRCLE_OFFSET+CIRCLE_RADIUS),
                    0, col+1};
        circles[col] = c;
    }
    for (int row=0; row<CIRCLE_AMOUNT_VRT; row++) {
        Circle c = {(float)CIRCLE_OFFSET+CIRCLE_RADIUS,
                    (float)HEIGHT-(CIRCLE_OFFSET+CIRCLE_RADIUS)*(2*row+3),
                    row+1, 0};
        circles[row+CIRCLE_AMOUNT_HRZ] = c;
    }
}

void update_curves() {
    for (int row=0; row<CURVE_AMOUNT_VRT; row++) {
        for (int col=0; col<CURVE_AMOUNT_HRZ; col++) {
            curves[row][col].update(row, col);
            curves[row][col].add_point();
        }
    }
}

void reset_curves() {
    for (int row=0; row<CURVE_AMOUNT_VRT; row++) {
        for (int col=0; col<CURVE_AMOUNT_HRZ; col++) {
            curves[row][col].reset();
        }
    }
}

void draw_curves() {
    glColor3f(0.0, 1.0, 0.0);
    for (int row=0; row<CURVE_AMOUNT_VRT; row++) {
        for (int col=0; col<CURVE_AMOUNT_HRZ; col++) {
            glBegin(GL_LINE_STRIP);
            for (std::pair<float, float> p : curves[row][col].path) {
                glVertex2f(p.first, p.second);
            }
            glEnd();
        }
    }
}

void draw_circles() {
    for (int i=0; i<CIRCLE_AMOUNT_HRZ+CIRCLE_AMOUNT_VRT; i++) {
        glColor3f(1.0, 1.0, 1.0);
        draw_circle(circles[i].centerX, circles[i].centerY);
        draw_dot_line(circles[i]);
    }
}

void draw_circle(float x, float y) {
    glBegin(GL_LINE_LOOP);
    for (float i=0; i<TWO_PI; i+=TWO_PI/50) {
        glVertex2f(x + std::cos(i)*CIRCLE_RADIUS,
                   y + std::sin(i)*CIRCLE_RADIUS);
    }
    glEnd();
}

void draw_dot_line(Circle c) {
    float newX, newY;
    glColor3f(0.6, 0.6, 0.6);
    if (c.col==0) {
        newX = c.centerX+std::cos(angle*c.row)*CIRCLE_RADIUS;
        newY = c.centerY+std::sin(angle*c.row)*CIRCLE_RADIUS;
        glBegin(GL_LINE_STRIP);
          glVertex2f(-1, newY);
          glVertex2f(WIDTH+1, newY);
        glEnd();
    } else {
        newX = c.centerX+std::cos(angle*c.col)*CIRCLE_RADIUS;
        newY = c.centerY+std::sin(angle*c.col)*CIRCLE_RADIUS;
        glBegin(GL_LINE_STRIP);
          glVertex2f(newX, -1);
          glVertex2f(newX, HEIGHT+1);
        glEnd();
    }
    draw_dot(newX, newY);
}


void draw_dot(float x, float y) {
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    for (float i=0; i<TWO_PI; i+=TWO_PI/7) {
        glVertex2f(x + std::cos(i)*DOT_SIZE, y + std::sin(i)*DOT_SIZE);
    }
    glEnd();
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    angle = PI/2;
    init_curves();
    init_circles();
}

void display_callback() {
    glClear (GL_COLOR_BUFFER_BIT);

    draw_curves();
    draw_circles();

    glFlush();
    glutSwapBuffers();
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int margin(0);
    glOrtho(0.0, (double)WIDTH, 0.0, (double)HEIGHT, -1.0, 0.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer_callback(int) {
    auto start(std::chrono::steady_clock::now());

    // reset one round out of two
    if (angle > 2*TWO_PI + PI/2) {
        angle = PI / 2;
        reset_curves();
    }
    else
        angle += DELTA_ANGLE;

    update_curves();
    glutPostRedisplay(); // run the display_callback function

    auto stop(std::chrono::steady_clock::now());
    auto duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start));
    glutTimerFunc(std::abs(1000.0/FPS - duration.count()), timer_callback, 0);
}


int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(15, 15); // optional
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Lissajous Curve Table");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(1000/FPS, timer_callback, 0);
    init();
    glutMainLoop();
    return 0;
}
