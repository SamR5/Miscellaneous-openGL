
/* Diffusion Reaction */

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <iostream>
#include <cstdio>
#include <chrono>
#include <cmath>
#include <sstream>

#define WIDTH 400
#define HEIGHT 400
#define FPS 30
#define CELL_SIZE 1
#define STATUSBAR_HEIGHT 25


// Diffusion Reaction

struct Cell {
    double A;
    double B;
};

Cell oldGrid[HEIGHT][WIDTH];
Cell grid[HEIGHT][WIDTH];


// no #define in case they vary in the grid
double diffusionA = 1.0;
double diffusionB = 0.35;
double feed = 0.055;//55;
double kill = 0.062;//62;

int currentParam; // current parameter selectionned

void init() {
    glClearColor(1.0, 0.0, 0.0, 1.0);
    for (int i=0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH; j++) {
            oldGrid[i][j] = Cell {1.0, 0.0};
            grid[i][j] = Cell {1.0, 0.0};
        }
    }
    for (int i=10; i<20; i++) {
        for (int j=10; j<20; j++) {
            grid[i][j].B = 1.0;
        }
    }
}

// faster like this than with the neighbors arrays
double LaplaceA(const int& i, const int& j) {
    return (oldGrid[i+1][j+1].A +
            oldGrid[i+1][j-1].A +
            oldGrid[i-1][j-1].A +
            oldGrid[i-1][j+1].A) * 0.05 +
           (oldGrid[i+1][j].A +
            oldGrid[i][j+1].A +
            oldGrid[i-1][j].A +
            oldGrid[i][j-1].A) * 0.2 -
           oldGrid[i][j].A;
}

double LaplaceB(const int& i, const int& j) {
    return (oldGrid[i+1][j+1].B +
            oldGrid[i+1][j-1].B +
            oldGrid[i-1][j-1].B +
            oldGrid[i-1][j+1].B) * 0.05 +
           (oldGrid[i+1][j].B +
            oldGrid[i][j+1].B +
            oldGrid[i-1][j].B +
            oldGrid[i][j-1].B) * 0.2 -
           oldGrid[i][j].B;
}



double newA(const int& i, const int& j, const double& A, const double& B) {
    // DeltaT = 1
    double res(diffusionA * LaplaceA(i, j));
    //Cell c(oldGrid[i][j]);
    if (A != 1)
        res += feed * (1 - A);
    return res - A * B * B + A;
    /*
    double res(0);
    Cell c(oldGrid[i][j]);
    res += c.A;
    res += diffusionA * LaplaceA(i, j);
    res -= c.A * c.B * c.B;
    res += feed*(1 + (float)j/WIDTH) * (1 - c.A);
    return res;
    */
}

double newB(const int& i, const int& j, const double& A, const double& B) {
    // DeltaT = 1
    double res(diffusionB * LaplaceB(i, j));
    //Cell c(oldGrid[i][j]);
    if (B != 0)
        res -= (kill + feed) * B;
    return res + A * B * B + B;
    /*
    double res(0);
    Cell c(oldGrid[i][j]);
    res += c.B;
    res += diffusionB * LaplaceB(i, j);
    res += c.A * c.B * c.B;
    res -= (kill*(1+(float)i/HEIGHT) + feed*(1+(float)j/WIDTH)) * c.B;
    return res;
    */
}

void update_old_grid() {
    for (int i=0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH; j++) {
            oldGrid[i][j] = grid[i][j];
        }
    }
}

void update_grid() {
    for (int i=1; i<HEIGHT-1; i++) {
        for (int j=1; j<WIDTH-1; j++) {
            const double A = oldGrid[i][j].A;
            const double B = oldGrid[i][j].B;
            double nA = newA(i, j, A, B);
            double nB = newB(i, j, A, B);
            if (nA > 1)
                nA = 1.0;
            if (nB < 0)
                nB = 0.0;
            grid[i][j].A = nA;
            grid[i][j].B = nB;
            
        }
    }
}

void update_parameter(int val) {
    switch (currentParam) {
        case 0:
            feed += 0.001 * val; break;
        case 1:
            kill += 0.001 * val; break;
    }
    if (feed < 0)
        feed = 0.0;
    if (kill < 0)
        kill = 0.0;
}

void draw_grid() {
    glBegin(GL_POINTS);
    for (int i=0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH; j++) {
            double col(grid[i][j].A - grid[i][j].B);
            glColor3f(col, col, col);
            glVertex2f(j, i);
            //glRecti(j, i, j+CELL_SIZE, i+CELL_SIZE);
        }
    }
    glEnd();
}

void draw_parameters() {
    std::stringstream fd, kl;
    fd << "Feed rate: " << feed;
    kl << "Kill rate: " << kill;
    glColor3f(0, 1, 1);
    glRasterPos2f(10, -STATUSBAR_HEIGHT+8);
    for (char c : fd.str()) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }
    glRasterPos2f(180, -STATUSBAR_HEIGHT+8);
    for (char c : kl.str()) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }
    glBegin(GL_LINE_LOOP);
      glVertex2f(5+currentParam*170, -STATUSBAR_HEIGHT+3);
      glVertex2f((currentParam+1)*170, -STATUSBAR_HEIGHT+3);
      glVertex2f((currentParam+1)*170, -3);
      glVertex2f(5+currentParam*170, -3);
    glEnd();
}

void display_callback() {
    glClear (GL_COLOR_BUFFER_BIT);
    draw_grid();
    draw_parameters();

    glFlush();
    glutSwapBuffers();
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int margin(0);
    glOrtho(0.0-margin, (double)WIDTH+margin,
            -STATUSBAR_HEIGHT-margin, (double)HEIGHT+margin,
            -1.0, 0.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer_callback(int) {
    auto start(std::chrono::steady_clock::now());

    for (int i=0; i<10; i++) {
        update_old_grid();
        update_grid();
    }

    glutPostRedisplay(); // run the display_callback function

    auto stop(std::chrono::steady_clock::now());
    auto duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start));
    //std::cout << duration.count() << std::endl;
    glutTimerFunc(std::abs(1000.0/FPS - duration.count()), timer_callback, 0);
}

void mouse_callback(int button, int state, int x, int y) {
    if (button==2 && state==0) {
        std::cout << grid[HEIGHT-y][x].A << " " << grid[HEIGHT-y][x].B << std::endl;
    }
    if (button!=0 || state!=0)
        return;
    y = HEIGHT-y;
    if (y <= 0)
        return;
    for (int i=y-20; i<y+20; i++) {
        for (int j=x-20; j<x+20; j++) {
            if (j<=1 || j>=WIDTH-1 || i<=1 || i>=HEIGHT-1)
                continue;
            grid[i][j].B = 1.0;
            grid[i][j].A = 0.0;
        }
    }
}

void keyboard_special_callback(int special, int x, int y) {
    switch (special) {
        case GLUT_KEY_UP: update_parameter(1); break;
        case GLUT_KEY_DOWN: update_parameter(-1); break;
        case GLUT_KEY_RIGHT: currentParam++; break;
        case GLUT_KEY_LEFT: currentParam--;
    }
    if (currentParam < 0)
        currentParam = 1;
    currentParam %= 2;
}

int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(15, 15); // optional
    glutInitWindowSize(WIDTH*CELL_SIZE, HEIGHT*CELL_SIZE + STATUSBAR_HEIGHT);
    glutCreateWindow("Diffusion Reaction");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(1000/FPS, timer_callback, 0);
    glutMouseFunc(mouse_callback);
    glutSpecialFunc(keyboard_special_callback);
    init();
    glutMainLoop();
    return 0;
}
