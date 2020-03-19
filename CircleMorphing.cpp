/*

Circle shape morphing to triangle

*/


#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <cmath>
#include <random>
#include <ctime>
#include <chrono>
#include <thread>

#define PI 3.14159265
#define TWO_PI PI*2
#define WIDTH 600
#define HEIGHT 600
#define FPS 30

#define N 360 // should be a multiple of three if triangle


float iniPoints[N][2];
float finPoints[N][2];
float currentPoints[N][2];
float statPoints[N][2]; // % of the way (0 to 1) and the direction (-1, 0, 1)

int rt(0);
int sign(1);
int cur(0);

void init();
// make the currentPoints[index] at <ratio> between initial and final
void morph(int index, float ratio);
void update1();
void update2();
void update3();
void update4();
void update5();
void update6();
void update7();



void timer_callback(int);
void display_callback();
void reshape_callback(int width, int height);

void init() {
    glClearColor(0, 0, 0, 1.0);
    srand(std::time(0));
    for (int i=0; i<N; i++) {
        iniPoints[i][0] = 0.9*0.5 * std::cos(i*TWO_PI/N);
        iniPoints[i][1] = 0.9*0.5 * std::sin(i*TWO_PI/N);
        statPoints[i][0] = 0;
    }
    for (int i=0; i<3; i++) {
        finPoints[i*N/3][0] = 0.9*0.5 * std::cos(i*TWO_PI/3);
        finPoints[i*N/3][1] = 0.9*0.5 * std::sin(i*TWO_PI/3);
    }
    for (int i=0; i<N/3; i++) {
        finPoints[i][0] = iniPoints[0][0] + (finPoints[N/3][0]-finPoints[0][0])*i/(N/3);
        finPoints[i+N/3][0] = iniPoints[N/3][0] + (finPoints[2*N/3][0]-finPoints[N/3][0])*i/(N/3);
        finPoints[i+2*N/3][0] = iniPoints[2*N/3][0] + (finPoints[0][0]-finPoints[2*N/3][0])*i/(N/3);
        finPoints[i][1] = iniPoints[0][1] + (finPoints[N/3][1]-finPoints[0][1])*i/(N/3);
        finPoints[i+N/3][1] = iniPoints[N/3][1] + (finPoints[2*N/3][1]-finPoints[N/3][1])*i/(N/3);
        finPoints[i+2*N/3][1] = iniPoints[2*N/3][1] + (finPoints[0][1]-finPoints[2*N/3][1])*i/(N/3);
    }
    for (int i=0; i<N; i++) {
        currentPoints[i][0] = iniPoints[i][0];
        currentPoints[i][1] = iniPoints[i][1];
    }
}

void morph(int index, float ratio) {
    float Dx = finPoints[index][0] - iniPoints[index][0];
    float Dy = finPoints[index][1] - iniPoints[index][1];
    currentPoints[index][0] = iniPoints[index][0] + Dx*ratio;
    currentPoints[index][1] = iniPoints[index][1] + Dy*ratio;
}

void update1() {
    for (int i=0; i<N; i++) {
        statPoints[i][0] += 0.01*sign;
        morph(i, statPoints[i][0]);
    }
    if (statPoints[0][0]<=0 || statPoints[0][0]>=1) {
        sign *= -1;
    }
}

void update2() {
    statPoints[cur][0] = sign==1 ? 1 : 0;
    morph(cur, statPoints[cur][0]);
    cur++;
    if (cur==N) {
        sign *= -1;
        cur = 0;
    }
}

void update3() {
    for (int i=0; i<3; i++) {
        statPoints[cur+i*N/3][0] = sign==1 ? 1 : 0;
        morph(cur+i*N/3, statPoints[cur+i*N/3][0]);
        if (cur==N/3) {
            sign *= -1;
            cur = 0;
        }
    }
    cur++;
}

int theta(0);
void update4() {
    for (int i=0; i<3; i++) {
        statPoints[cur+i*N/3][0] = sign==1 ? 1 : 0;
        morph(cur+i*N/3, statPoints[cur+i*N/3][0]);
        if (cur==N/3) {
            sign *= -1;
            cur = 0;
        }
    }
    cur++;
    theta -= 1;
}

void update5() {
    statPoints[cur][0] = sign==1 ? 1 : 0;
    statPoints[N-1-cur][0] = sign==1 ? 1 : 0;
    morph(cur, statPoints[cur][0]);
    morph(N-1-cur, statPoints[N-1-cur][0]);
    cur++;
    if (cur==N/2) {
        sign *= -1;
        cur = 0;
    }
}

void update6() {
    statPoints[cur][0] = sign==1 ? 1 : 0;
    statPoints[N-1-cur][0] = sign==1 ? 1 : 0;
    morph(cur, statPoints[cur][0]);
    morph(N-1-cur, statPoints[N-1-cur][0]);
    cur+=sign;
    if (cur>N/2 || cur<=-1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        sign *= -1;
        cur+=sign;
    }
}

int beg(0);
void update7() {
    for (int i=beg; i<N; i+=2) {
        statPoints[i][0] += 0.04*sign;
        morph(i, statPoints[i][0]);
    }
    if (statPoints[0][0]<=0 || statPoints[0][0]>=1) {
        sign *= -1;
        beg=1;
    }
    if (beg==1) {
        beg = 0;
    } else {
        beg = 1;
    }
}

void display_callback() {
    glClear(GL_COLOR_BUFFER_BIT); // clear screen
    glColor3f(1, 1, 1);
    glPushMatrix();
    glRotatef(theta, 0, 0 ,1);
    glBegin(GL_LINE_LOOP);
    for (int i=0; i<N; i++) {
        glVertex2f(currentPoints[i][0], currentPoints[i][1]);
    }
    glEnd();
    glPopMatrix();
    glutSwapBuffers(); // display / update
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}


void timer_callback(int) {
    auto start(std::chrono::steady_clock::now());
    update4();
    glutPostRedisplay(); // run the display_callback function
    auto stop(std::chrono::steady_clock::now());
    auto duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start));
    glutTimerFunc(std::abs(1000.0/FPS - duration.count()), timer_callback, 0);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(30, 30); // optional
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Windows Loading Logo");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(1000/FPS, timer_callback, 0);
    init();
    glutMainLoop();
    return 0;
}
