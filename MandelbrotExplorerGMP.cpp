#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <cstdio>
#include <cmath>
#include <random>
#include <ctime>
#include <chrono>
#include <gmpxx.h>



#define WIDTH 600
#define HEIGHT 600
#define FPS 60

int ITERATIONS_MAX = 64; // for the precision
int pixels[HEIGHT][WIDTH]; // number of iterations for every pixel

bool condition1(const mpf_t A, const mpf_t B);
void update_za(mpf_t &za, const mpf_t zaa, const mpf_t zbb, const mpf_t x);
void update_zb(mpf_t &zb, const mpf_t zaa, const mpf_t zbb, const mpf_t y);
int iteration_required(const mpf_t& x, const mpf_t& y);
void update_pixels();
void update_color(float h, float colors[]);
void draw_set();
void init();

void timer_callback(int);
void display_callback();
void reshape_callback(int width, int height);
void mouse_callback(int button, int state, int x, int y);

// borders that will change according to the mose click
mpf_t Xmin, Ymin, Xmax, Ymax;
mpf_t deltaX, deltaY;

// temporary variables used for iteration_required
// avoid creation, initialization and clear each call
// gain 25-30%
mpf_t AA, BB, result; // condition1
mpf_t temp; // update_za
mpf_t za, zb, zaa, zbb; // iteration_required

// A*A + B*B < 4 (the distance from center is lower than 2 ie diverge)
bool condition1(const mpf_t A, const mpf_t B) {
    mpf_mul(AA, A, A);
    mpf_mul(BB, B, B);
    mpf_add(result, AA, BB);
    return mpf_cmp_d(result, 4) < 0;
}

void update_za(mpf_t &za, const mpf_t zaa, const mpf_t zbb, const mpf_t x) {
    //zaa*zaa-zbb*zbb+x
    mpf_set(za, zaa);
    mpf_mul(za, za, zaa);
    mpf_mul(temp, zbb, zbb);
    mpf_sub(za, za, temp);
    mpf_add(za, za, x);
}

void update_zb(mpf_t &zb, const mpf_t zaa, const mpf_t zbb, const mpf_t y) {
    // 2*zaa*zbb+y
    mpf_set_d(zb, 2.0);
    mpf_mul(zb, zb, zaa);
    mpf_mul(zb, zb, zbb);
    mpf_add(zb, zb, y);
}

int iteration_required(const mpf_t& x, const mpf_t& y) {
    mpf_set_d(za, 0);
    mpf_set_d(zb, 0);
    int counter(0);
    while (condition1(za, zb) && counter < ITERATIONS_MAX) {
        mpf_set(zaa, za);
        mpf_set(zbb, zb);
        update_za(za, zaa, zbb, x);
        update_zb(zb, zaa, zbb, y);
        ++counter;
    }
    return (counter<ITERATIONS_MAX) ? counter : 0;
}

void update_pixels() {
    mpf_t realX, realY, tmp;
    mpf_inits(realX, realY, tmp, nullptr);
    for (int x=0; x<WIDTH; ++x) {
        //realX = xmin + interval*x/WIDTH
        mpf_set(realX, Xmax);
        mpf_sub(realX, realX, Xmin);
        mpf_set_d(tmp, (double)x/WIDTH);
        mpf_mul(realX, realX, tmp);
        mpf_add(realX, realX, Xmin);
        for (int y=0; y<HEIGHT; y++) {
            //realY = ymin + interval*y/HEIGHT
            mpf_set(realY, Ymax);
            mpf_sub(realY, realY, Ymin);
            mpf_set_d(tmp, (double)y/HEIGHT);
            mpf_mul(realY, realY, tmp);
            mpf_add(realY, realY, Ymin);
            pixels[x][y] = iteration_required(realX, realY);
        }
    }
    mpf_clears(realX, realY, tmp, nullptr);
}

void update_color(float h, float colors[]) {
    // h goes from 0 to 1
    int i = h*6;
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
    glBegin(GL_POINTS);
    for (int x=0; x<WIDTH; x++) {
        for (int y=0; y<HEIGHT; y++) {
            if (pixels[x][y]) {
                update_color((float)(pixels[x][y])/ITERATIONS_MAX, color);
                glColor3fv(color);
                glVertex2f(x, y);
            }
        }
    }
    glEnd();
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    std::srand(std::time(0));
    mpf_inits(Xmin, Ymin, Xmax, Ymax, deltaX, deltaY, nullptr);
    mpf_set_d(Xmin, -2.0);
    mpf_set_d(Ymin, -2.0);
    mpf_set_d(Xmax, 2.0);
    mpf_set_d(Ymax, 2.0);
    //deltaX = (Xmax - Xmin) / WIDTH;
    mpf_set_d(deltaX, (mpf_get_d(Xmax)-mpf_get_d(Xmin)) / WIDTH);
    //deltaY = (Ymax - Ymin) / HEIGHT;
    mpf_set_d(deltaY, (mpf_get_d(Ymax)-mpf_get_d(Ymin)) / HEIGHT);
    // 
    mpf_inits(AA, BB, result, // condition1
              temp, // update_za
              za, zb, zaa, zbb, // iteration_required
              nullptr);
}

void display_callback() {
    auto start(std::chrono::steady_clock::now());
    glClear(GL_COLOR_BUFFER_BIT); // clear screen
    update_pixels();
    draw_set();
    glutSwapBuffers(); // display / update
    auto stop(std::chrono::steady_clock::now());
    auto duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start));
    std::cout << duration.count() << std::endl;
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

void timer_callback(int) {
    
    glutTimerFunc(1000/FPS, timer_callback, 0);
}

void update_real(mpf_t &real, mpf_t min, mpf_t max, int pos, int size) {
    //real = min + (max-min) * x / WIDTH;
    mpf_sub(max, max, min);
    mpf_t ratio;
    mpf_init_set_d(ratio, (double)pos/size);
    mpf_mul(max, max, ratio);
    mpf_add(real, min, max);
    mpf_clear(ratio);
}

void mouse_callback(int button, int state, int x, int y) {
    // any button released
    if (state==1 || button==1 || button>2)
        return;
    mpf_t realX, realY, intervalX, intervalY;
    mpf_inits(realX, realY, intervalX, intervalY, nullptr);
    // right click
    if (button==2) {
        mpf_sub(intervalX, Xmax, Xmin);
        mpf_sub(intervalY, Ymax, Ymin);
        mpf_mul_ui(deltaX, deltaX, 2);
        mpf_mul_ui(deltaY, deltaY, 2);
    }
    // left click
    if (button==0) {
        y = HEIGHT - y;
        // x, y in pixels from top-left to x, y on the orthonormal space
        mpf_sub(intervalX, Xmax, Xmin); mpf_div_ui(intervalX, intervalX, 4);
        mpf_sub(intervalY, Ymax, Ymin); mpf_div_ui(intervalY, intervalY, 4);
        mpf_div_ui(deltaX, deltaX, 2);
        mpf_div_ui(deltaY, deltaY, 2);
    }
    update_real(realX, Xmin, Xmax, x, WIDTH);
    update_real(realY, Ymin, Ymax, y, HEIGHT);
    mpf_sub(Xmin, realX, intervalX);
    mpf_add(Xmax, realX, intervalX);
    mpf_sub(Ymin, realY, intervalY);
    mpf_add(Ymax, realY, intervalY);
    mpf_clears(realX, realY, intervalX, intervalY, nullptr);
    display_callback();
}

void keyboard_callback(unsigned char key, int x, int y) {
    if (key == '+')
        ITERATIONS_MAX *= 2;
    else if (key == '-')
        ITERATIONS_MAX *= (ITERATIONS_MAX==1 ? 1 : 0.5);
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
    mpf_clears(Xmin, Ymin, Xmax, Ymax, deltaX, deltaY, nullptr);
    mpf_clears(AA, BB, result, // condition1
               temp, // update_za
               za, zb, zaa, zbb, // iteration_required
               nullptr);
    return 0;
}
