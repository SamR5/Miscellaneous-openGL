/*

Colored blobs

*/

#include <GL/gl.h>
#include <GL/glut.h>

#include <cmath>
#include <iostream>
#include <chrono>
#include <ctime>
#include <random>
#include <vector>

#define HEIGHT 400
#define WIDTH 400
#define FPS 60

#define AMOUNT 2

struct Blob {
    int x, y;
    int xV, yV;
    void go() {
        x += xV;
        y += yV;
        if (x <= 0) {
            x = 0;
            xV *= -1;
        }
        if (x >= WIDTH) {
            x = WIDTH;
            xV *= -1;
        }
        if (y <= 0) {
            y = 0;
            yV *= -1;
        }
        if (y >= HEIGHT) {
            y = HEIGHT;
            yV *= -1;
        }
    }
};


float pixels[HEIGHT][WIDTH];
Blob blobs[AMOUNT];
float currentColor[3];

float square_distance(float x1, float y1, float x2, float y2);
void init();
void reset_pixels();
void move_blobs();
void update_color(float h);
void update_pixels();

void display_callback();
void reshape_callback(int width, int height);
void timer_callback(int);


void init() {
    glClearColor(0.0, 0.0, 0.5, 1.0);
    for (int i=0; i<AMOUNT; i++) {
        blobs[i] = {rand()%WIDTH, rand()%HEIGHT,
                    10-rand()%10, 10-rand()%10};
    }
    reset_pixels();
}

float square_distance(float x1, float y1, float x2, float y2) {
    float x(x2-x1), y(y2-y1);
    return x*x + y*y;
}

void reset_pixels() {
    for (int i=0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH; j++) {
            pixels[i][j] = 0.0;
        }
    }
}

void move_blobs() {
    for (int i=0; i<AMOUNT; i++) {
        blobs[i].go();
    }
}

void update_color(float h) {
    h /= 360;
    int i = std::floor(h*6);
    float f, q;
    f = h*6 - i;
    q = 1 - f;
    switch (i%6) {
      case 0:
        currentColor[0] = 1;
        currentColor[1] = f;
        currentColor[2] = 0; break;
      case 1:
        currentColor[0] = q;
        currentColor[1] = 1;
        currentColor[2] = 0; break;
      case 2:
        currentColor[0] = 0;
        currentColor[1] = 1;
        currentColor[2] = f; break;
      case 3:
        currentColor[0] = 0;
        currentColor[1] = q;
        currentColor[2] = 1; break;
      case 4:
        currentColor[0] = f;
        currentColor[1] = 0;
        currentColor[2] = 1; break;
      case 5:
        currentColor[0] = 1;
        currentColor[1] = 0;
        currentColor[2] = q; break;
    }
}

void update_pixels() {
    float dist2;
    for (int i=0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH; j++) {
            for (int x=0; x<AMOUNT; x++) {
                dist2 = square_distance(blobs[x].x, blobs[x].y, j, i);
                //pixels[i][j] += 300000.0/dist2;
                pixels[i][j] += 180*(1+std::cos(0.1*std::sqrt(dist2)));
                //pixels[i][j] += 1000*std::tanh(50000/dist2);
                //pixels[i][j] += 500*std::sinh(0.00001*dist2);
            }
        }
    }
}

void display_callback() {
    //glClear(GL_COLOR_BUFFER_BIT);
    std::vector<float> color;
    for (int i=0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH; j++) {
            update_color(pixels[i][j]);
            glColor3fv(currentColor);
            /*if (pixels[i][j] < 5) {
                glColor3f(1, 0, 0);
            }
            else if (pixels[i][j] > 2000){
                glColor3f(1, 0, 1);
            }
            else {
                update_color(pixels[i][j]);
                glColor3fv(currentColor);
            }*/
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

    move_blobs();
    for (int i=0; i<1; i++) {
        reset_pixels();
        update_pixels();
    }
    display_callback();//glutPostRedisplay();

    auto stop(std::chrono::steady_clock::now());
    auto duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start));
    //std::cout << duration.count() << std::endl;
    glutTimerFunc(std::abs(1000.0/FPS - duration.count()), timer_callback, 0);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(600, 400); // optional
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("HSV Blobs");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(1000/FPS, timer_callback, 0);
    init();
    glutMainLoop();
    return 0;
}
