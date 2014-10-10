#include <iostream>
#include <stack>


#define SPACE -1
#define INVALID_POSITION 0

using namespace std;


int maxMoves;

typedef struct {
    int x;
    int y;
} Coord;

bool isTriangleSolved(int **triangle) {
    
    
    return false;
}

bool canMoves(int moves) {
    return moves >= maxMoves;
}

Coord getPositionOfSpace(int **triangle, int dimension) {
    Coord coord;
    
    for (int i=0; i<5; i++) {
        for (int j=0; j<=i; j++) {
            if (triangle[i][j] == SPACE) {
                coord.x = i;
                coord.y = j;
                
                return coord;
            }
        }
    }
    
    return coord;
}

void swapLeft(int **triangle) {
    
}

void swapRight(int **triangle) {
    
}

void swapUpLeft(int **triangle) {
    
}

void swapUpRight(int **triangle) {
    
}

void swapDownLeft(int **triangle) {
    
}

void swapDownRight(int **triangle) {
    
}

void saveResult(int steps, int **triangle) {
    
}

void mainProccesLoop(int **triangle, Coord coord, int dimension) {
    
}


int main () {
    int **triangle = NULL;
    int dimension = 5;
    
    stack<int> sampleStack;
    
    sampleStack.push(9);
    sampleStack.push(10);
    sampleStack.push(8);
    
    for (int i=0; i<sampleStack.size(); i++) {
        cout << sampleStack.top() << endl;
        sampleStack.pop();
    }
    
    
    
    triangle = new int*[dimension];
    
    triangle[0] = new int[1];
    triangle[1] = new int[2];
    triangle[2] = new int[3];
    triangle[3] = new int[4];
    triangle[4] = new int[5];
    
    triangle[0][0] = 4;
    triangle[1][0] = 5;
    triangle[1][1] = 1;
    triangle[2][0] = 8;
    triangle[2][1] = 10;
    triangle[2][2] = 9;
    triangle[3][0] = 6;
    triangle[3][1] = SPACE;
    triangle[3][2] = 14;
    triangle[3][3] = 12;
    triangle[4][0] = 3;
    triangle[4][1] = 2;
    triangle[4][2] = 7;
    triangle[4][3] = 11;
    triangle[4][4] = 13;

    Coord coord = getPositionOfSpace(triangle, dimension);

    mainProccesLoop (triangle, coord, dimension);
    
    
    // Clean up
    for (int i=0; i<dimension; i++) {
        delete triangle[i];
    }
    delete triangle;
    
    return 0;
}
