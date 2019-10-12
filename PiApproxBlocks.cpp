/*

Most unefficient way to compute PI

*/

#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <cmath>
#include <cstdio>
#include <sstream>


#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 300

#define STEPS 1000000
#define N 7 // Big.mass = Small.mass * 100^N


struct Block {
    long double position;
    long double velocity;
    long double mass;
    int width, height;
    void update() {
        position += velocity;
    }
}Big, Small;

int counter;

void timer_callback(int);
void display_callback();
void reshape_callback(int width, int height);
void glutTimerFunc(int);

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    counter = 0;
    Big.position = 2*SCREEN_WIDTH/10;
    Small.position = SCREEN_WIDTH/10;
    Big.velocity = -1.0 / STEPS;
    Small.velocity = 0;
    Big.mass = std::pow(100, N);
    Small.mass = 1;
    Big.width = Big.height = 100;
    Small.width = Small.height = 10;
}

void update() {
    Big.update();
    Small.update();
}

bool is_impact() {
    if (Big.position <= Small.position+Small.width)
        return true;
    return false;
}

void update_velocities() {
    // bounce
    if (Small.position <= 0) {
        Small.position = 0;
        Small.velocity *= -1;
        counter++;
    }
    if (!is_impact())
        return;

    counter++;
    long double tempBV = Big.velocity;
    long double invSumMass = 1.0 / (Small.mass + Big.mass);
    Big.velocity = (Big.mass-Small.mass) * Big.velocity *invSumMass + 2 * Small.mass * Small.velocity *invSumMass;
    Small.velocity = 2 * Big.mass * tempBV *invSumMass + (Small.mass-Big.mass) * Small.velocity *invSumMass;
}

bool is_simulation_ended() {
    return Big.velocity > 0 && Small.velocity > 0 && Big.velocity>Small.velocity;
}

void display_callback() {
    glClear(GL_COLOR_BUFFER_BIT); // clear screen
    // bottom line
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP);
      glVertex2f(0, 10);
      glVertex2f(SCREEN_WIDTH, 10);
    glEnd();
    // big block
    glColor3f(1.0, 0.0, 0.0);
    glRectd(Big.position, 10,
            Big.position+Big.width, Big.height+10);
    // small block
    glColor3f(0.0, 1.0, 0.0);
    glRectd(Small.position, 10,
            Small.position+Small.width, Small.height+10);
    // current pi
    glColor3f(0, 0, 0);
    glRasterPos2f(5, SCREEN_HEIGHT-20);
    for (char c : std::to_string(counter)) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }
    glutSwapBuffers();
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width, 0.0, height, -1.0, 1.0); // 0, xmax, 0, ymax, z0, zmax
    glMatrixMode(GL_MODELVIEW);
}

void timer_callback(int) {
    for (int i=0; i<STEPS; i++) {
        update_velocities();
        update();
    }
    glutPostRedisplay();
    if (is_simulation_ended()) {
        return;
    }
    return glutTimerFunc(1000/40, timer_callback, 0);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(30, 30); // optional
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("Blocks");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(0, timer_callback, 0);
    init();
    glutMainLoop();
    return 0;
}
