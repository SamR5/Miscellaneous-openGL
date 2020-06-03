#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <cstdio>
#include <cmath>
#include <random>
#include <ctime>
#include <chrono>


#define WIDTH 600
#define HEIGHT 600
#define FPS 60

int ITERATIONS_MAX = 64; // for the precision

int iteration_required(const long double& x, const long double& y);
void update_color(float h, float colors[]);
void draw_set();
void init();

void timer_callback(int);
void display_callback();
void reshape_callback(int width, int height);
void mouse_callback(int button, int state, int x, int y);
void keyboard_special_callback(char key, int x, int y);

// borders that will change according to the mose click
long double Xmin, Ymin, Xmax, Ymax;
long double deltaX, deltaY;

int iteration_required(const long double& x, const long double& y) {
    // try with z = cos(z) + c & z = exp(-(z*z)) + c
    long double za(0), zb(0), zaa, zbb;
    int counter(0);
    while ((za*za+zb*zb) < 4 && counter < ITERATIONS_MAX) {
        zaa = za;
        zbb = zb;
        za = zaa*zaa-zbb*zbb+x;
        zb = 2*zaa*zbb+y;
        ++counter;
    }
    return (counter<ITERATIONS_MAX) ? counter : 0;
}

void update_color(float h, float colors[]) {
    // h goes from 0 to 1
    int i = std::floor(h*6);
    float f, q;
    f = h*6 - i;
    q = 1 - f;
    switch (i%6) {
      case 0:
        colors[0] = 1;
        colors[1] = f;
        colors[2] = 0; break;
      case 1:
        colors[0] = q;
        colors[1] = 1;
        colors[2] = 0; break;
      case 2:
        colors[0] = 0;
        colors[1] = 1;
        colors[2] = f; break;
      case 3:
        colors[0] = 0;
        colors[1] = q;
        colors[2] = 1; break;
      case 4:
        colors[0] = f;
        colors[1] = 0;
        colors[2] = 1; break;
      case 5:
        colors[0] = 1;
        colors[1] = 0;
        colors[2] = q; break;
    }
}

void draw_set() {
    float color[3];
    float it; // temp value for the number of iterations
    glBegin(GL_POINTS);
    for (long double x=Xmin; x<Xmax; x+=deltaX) {
        for (long double y=Ymin; y<Ymax; y+=deltaY) {
            it = iteration_required(x, y);
            if (it) {
                update_color(it/ITERATIONS_MAX, color);
                glColor3fv(color);
                glVertex2f(x, y);
            }
        }
    }
    glEnd();
    // cross in the center
    glColor3f(0.0, 1.0, 0.0);
    long double Dx = Xmax-Xmin;
    long double Dy = Ymax-Ymin;
    glBegin(GL_LINES);
      glVertex2f(Xmin + Dx/2, Ymin + 0.52*Dy);
      glVertex2f(Xmin + Dx/2, Ymax - 0.52*Dy);
      glVertex2f(Xmin + 0.52*Dx, Ymin + Dy/2);
      glVertex2f(Xmax - 0.52*Dx, Ymin + Dy/2);
    glEnd();
}


void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    std::srand(std::time(0));
    Xmin = Ymin = -2;
    Xmax = Ymax = 2;
    deltaX = (Xmax - Xmin) / WIDTH;
    deltaY = (Ymax - Ymin) / HEIGHT;
}

void display_callback() {
    auto start = std::chrono::high_resolution_clock::now();
    glClear(GL_COLOR_BUFFER_BIT); // clear screen
    draw_set();
    glutSwapBuffers(); // display / update
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(Xmin, Xmax, Ymin, Ymax, -1.0, 1.0); // 0, xmax, 0, ymax, z0, zmax
    glMatrixMode(GL_MODELVIEW);
}

void timer_callback(int) {
    
    glutTimerFunc(1000/FPS, timer_callback, 0);
}

void mouse_callback(int button, int state, int x, int y) {
    // any button released
    if (state==1 || button==1 || button>2)
        return;
    long double realX, realY, intervalX, intervalY;
    // right click
    if (button==2) {
        intervalX = Xmax - Xmin;
        intervalY = Ymax - Ymin;
        deltaX *= 2;
        deltaY *= 2;
    }
    // left click
    if (button==0) {
        //bounds.push_back({Xmin, Xmax, Ymin, Ymax});
        y = HEIGHT - y;
        // x, y in pixels from top-left to x, y on the orthonormal space
        intervalX = (Xmax-Xmin)/4;
        intervalY = (Ymax-Ymin)/4;
        deltaX /= 2;
        deltaY /= 2;
    }
    realX = Xmin + (Xmax-Xmin) * (long double)x / WIDTH;
    realY = Ymin + (Ymax-Ymin) * (long double)y / HEIGHT;
    Xmin = realX - intervalX;
    Xmax = realX + intervalX;
    Ymin = realY - intervalY;
    Ymax = realY + intervalY;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(Xmin, Xmax, Ymin, Ymax, -1.0, 1.0); // 0, xmax, 0, ymax, z0, zmax
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
}

void keyboard_callback(unsigned char key, int x, int y) {
    if (key == '+')
        ITERATIONS_MAX *= 2;
    else if (key == '-')
        ITERATIONS_MAX *= 0.5;
    else
        return;
    display_callback();
    std::cout << "iterations: " << ITERATIONS_MAX << std::endl;
}

int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    //glutInitWindowPosition(); // optional
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Mandelbrot Set Explorer");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(0, timer_callback, 0);
    glutMouseFunc(mouse_callback);
    glutKeyboardFunc(keyboard_callback);
    init();
    glutMainLoop();
    return 0;
}
