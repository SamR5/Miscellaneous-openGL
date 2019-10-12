/*

    Flood-it game

*/

#include <GL/gl.h>
#include <GL/glut.h>

#include <ctime>
#include <random>
#include <vector>
#include <utility>

#define WIDTH 600
#define HEIGHT 600


const float colors[6][3] = {
        {1.0, 0.0, 0.0}, // red
        {0.0, 1.0, 0.0}, // green
        {0.0, 0.0, 1.0}, // blue
        {1.0, 1.0, 0.0}, // yellow
        {0.0, 1.0, 1.0}, // cyan
        {1.0, 0.0, 1.0}  // purple
};


#define RED 0
#define GREEN 1
#define BLUE 2
#define YELLOW 3
#define CYAN 4
#define PURPLE 5

#define TO_RED 6
#define TO_GREEN 7
#define TO_BLUE 8
#define TO_YELLOW 9
#define TO_CYAN 10
#define TO_PURPLE 11

int** grid = nullptr;
int rows;
int columns;
float cellHeight;
float cellWidth;
int neighbors[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
int currentColor;
std::vector<std::pair<int, int>> visited;

void timer_callback(int);
void display_callback();
void reshape_callback(int width, int height);
void mouse_callback(int button, int state, int x, int y);
void glutTimerFunc(int);

void init(int r, int c);
void delete_grid();
void init_grid();

bool already_visited(int r, int c);
void change_color(int row, int col);
void update_visited();
bool is_completed();

void init(int c, int r) {
    srand(time(0));
    rows = r;
    columns = c;
    cellHeight = (float)HEIGHT/rows;
    cellWidth = (float)WIDTH/columns;

    glClearColor(0.0, 0.0, 0.0, 1.0);
    init_grid();
    currentColor = grid[0][0];
    visited.push_back({0, 0});
    update_visited();
}

void delete_grid() {
    for (int i=0; i<rows; i++) {
        delete grid[i];
    }
    delete grid;
}

void init_grid() {
    grid = new int* [rows];
    for (int i=0; i<rows; i++) {
        grid[i] = new int [rows];
        for (int j=0; j<columns; j++) {
            grid[i][j] = rand()%6;
        }
    }
}

bool already_visited(int r, int c) {
    for (std::pair<int, int> p : visited) {
        if (p.first == r && p.second == c)
            return true;
    }
    return false;
}

void change_color(int row, int col) {
    col /= cellWidth;
    row /= cellHeight;
    if (currentColor == grid[row][col])
        return;

    currentColor = grid[row][col];

    for (auto p : visited) {
        grid[p.first][p.second] = currentColor;
    }
}

void update_visited() {
    bool change = false;
    for (auto cell : visited) {
        for (int i=0; i<4; i++) {
            int newR = cell.first + neighbors[i][0];
            int newC = cell.second + neighbors[i][1];
            if (newR<0 || newC<0 || newR>=rows || newC>=columns)
                continue;
            if (already_visited(newR, newC))
                continue;
            if (grid[newR][newC] == currentColor) {
                visited.push_back({newR, newC});
                change = true;
            }
        }
    }
    if (change)
        update_visited();
}

void display_callback() {
    glClear(GL_COLOR_BUFFER_BIT); // clear screen
    for (int i=0; i<rows; i++) {
        for (int j=0; j<columns; j++) {
            glColor3fv(colors[grid[i][j]]);
            glRectd(j, i, (j+1), (i+1));
        }
    }
    glutSwapBuffers(); // display / update
}

bool is_completed() {
    int color(grid[0][0]);
    for (int i=0; i<rows; i++) {
        for (int j=0; j<columns; j++) {
            if (grid[i][j] != color)
                return false;
        }
    }
    return true;
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, rows, columns, 0.0, -1.0, 1.0); // 0, xmax, 0, ymax, z0, zmax
    glMatrixMode(GL_MODELVIEW);
}

void mouse_callback(int button, int state, int x, int y) {
    if (state == 1 || button > 0)
        return;
    // changes the color of visited cells to the clicked cell
    change_color(y, x);
    // changes the color of cells around the visited
    update_visited();
    glutPostRedisplay();
    
    if (is_completed()) {
        delete_grid();
        init_grid();
    }
}

int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(30, 30); // optional
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Flood it !");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutMouseFunc(mouse_callback);
    init(15, 15);
    glutMainLoop();
    return 0;
}
