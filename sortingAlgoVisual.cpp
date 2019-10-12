/*

Sorting algorithm visualization

*/

#include <GL/gl.h>
#include <GL/glut.h>

#include <chrono>
#include <thread>
#include <cmath>
#include <ctime>
#include <random>
#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>

#define COLUMNS 100
#define LINE_WIDTH 6

#define HEIGHT 600
#define WIDTH COLUMNS*LINE_WIDTH
#define FPS 60


void timer_callback(int);
void display_callback(int greenIndex=-1, int redIndex=-1, int millisec=0);
void reshape_callback(int width, int height);

void init();
void shuffle_array();
bool is_array_sorted();
int find_index(int item);
void bubble_sort();
void insertion_sort();
void selection_sort();
void merge_sort(int low, int high);
void merge(int low, int mid, int high);
int partition(int low, int high);
void quick_sort(int low, int high);

std::vector<int> theArray;


void init() {
    glClearColor(0, 0, 0, 1);
    for (int i=0; i<COLUMNS; i++) {
        theArray.push_back(i);
    }
    shuffle_array();
}

void shuffle_array() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(theArray.begin(), theArray.end(), g);
}

void display_array() {
    for (int i=0; i<COLUMNS-1; i++) {
        std::cout << theArray[i] << "-";
    }
    std::cout << theArray[COLUMNS-1] << std::endl;
}

bool is_array_sorted() {
    for (int i=1; i<COLUMNS; i++) {
        if (theArray[i] < theArray[i-1])
            return false;
    }
    return true;
}

int find_index(int item) {
    std::vector<int>::iterator I = std::find(theArray.begin(), theArray.end(), item);
    // return directly because the item is always in the array
    return std::distance(theArray.begin(), I);
}

// the end is sorted by +1 each loop through the bbs
void bubble_sort() { // one go through the array
    float temp;
    for (int min=0; min<COLUMNS; min++) {
        for (int i=0; i<COLUMNS-min; i++) {
            if (theArray[i] < theArray[i-1]) {
                temp = theArray[i];
                theArray[i] = theArray[i-1];
                theArray[i-1] = temp;
            }
            if (i%2)  // one frame out of two to speed up
                display_callback(i, COLUMNS-min, 0);
        }
    }
}

void insertion_sort() {
    for (int i=0; i<COLUMNS; i++) {
        float key;
        int j;
        key = theArray[i];
        j = i-1;
        while (j >= 0 && theArray[j] > key) {
            if (j%2)
                display_callback(j+1, i, 0);
            theArray[j+1] = theArray[j];
            j--;
        }
        theArray[j+1] = key;
    }
}

void selection_sort() {
    for (int x=COLUMNS; x>=0; x--) {
        float tempLargest(0);
        for (int i=0; i<=x; i++) {
            if (theArray[i] > theArray[tempLargest]) {
                tempLargest = i;
            }
            if (i%2) // one frame out of two to speed up
                display_callback(i, tempLargest, 0);
        }
        std::iter_swap(theArray.begin()+x, theArray.begin()+tempLargest);
        if (x%2)
            display_callback(x, tempLargest, 0);
    }
}

void merge_sort(int low, int high) {
    display_callback(low, high, 20);
    if (low < high) {
        int mid = (low+high) / 2;
        merge_sort(low, mid);
        merge_sort(mid+1, high);
        merge(low, mid, high);
    }
}

void merge(int low, int mid, int high) {
    int n = high - low+1;
    int* b = new int[n];
    int left(low);
    int right(mid+1);
    int bIdx(0);
    while (left <= mid && right <= high) {
        if (theArray[left] <= theArray[right])
            b[bIdx++] = theArray[left++];
        else
            b[bIdx++] = theArray[right++];
    }
    while (left <= mid)
        b[bIdx++] = theArray[left++];
    while (right <= high)
        b[bIdx++] = theArray[right++];
    for (int k=0; k<n; k++) {
        //theArray[low+k] = b[k];
        display_callback(low+k, low, 20);
        std::iter_swap(theArray.begin()+low+k, theArray.begin()+find_index(b[k]));
        display_callback(low+k, low, 20);
    }
    delete [] b;
}

void quick_sort(int low, int high) {
    if (low < high) {
        int pivotIndex = partition(low, high);
        quick_sort(low, pivotIndex-1);
        display_callback(low, pivotIndex-1, 16);
        quick_sort(pivotIndex+1, high);
        display_callback(pivotIndex-1, high, 16);
    }
}

int partition(int low, int high) {
    int pivot(theArray[high]);
    int m = high;
    //float temp;
    for (int k=high-1; k >= low; k--) {
        if (theArray[k] > pivot) {
            m--;
            std::iter_swap(theArray.begin()+k, theArray.begin()+m);
        //    display_callback(k, m, 16);
        }
        display_callback(k, m, 16);
    }
    display_callback(high, m, 16);
    std::iter_swap(theArray.begin()+high, theArray.begin()+m);
    display_callback(high, m, 16);
    return m;
}

void display_callback(int greenIndex, int redIndex, int millisec) {
    glClear(GL_COLOR_BUFFER_BIT);
    for (int i=0; i<COLUMNS; i++) {
        glColor3f(1, 1, 1);
        if (i==greenIndex)
            glColor3f(0, 1, 0);
        else if (i==redIndex)
            glColor3f(1, 0, 0);
        else
            glColor3f(1, 1, 1);
        glRectd(i, 0, i+1, theArray[i]*(float)HEIGHT/COLUMNS);
    }
    glutSwapBuffers();
    if (millisec > 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(millisec));
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, COLUMNS, 0.0, HEIGHT, -1.0, 1.0); // 0, xmax, 0, ymax, z0, zmax
    glMatrixMode(GL_MODELVIEW);
}

int count(0);
void timer_callback(int) {
    count %= 5;
    switch (count) {
        //case 0: bubble_sort(); break;
        case 1: insertion_sort(); break;
        case 2: selection_sort(); break;
        case 3: merge_sort(0, COLUMNS); break;
        case 4: quick_sort(0, COLUMNS);
        
    }
    count++;
    shuffle_array();
    glutTimerFunc(1000.0/FPS, timer_callback, 0);
}


int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(30, 30); // optional
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Sorting algorithm visualization");
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(0, timer_callback, 0);
    init();
    glutMainLoop();
    return 0;
}
