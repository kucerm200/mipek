#include <iostream>
#include <stack>


#define SPACE -1
#define INVALID_POSITION 0

/*

11.10. 13:00 - fialaka1
    - upravil jsem ty swap -> logiku jsem strcil do swapThem() a ty jednotlive fce jen urci jak

11.10. 12:00 - fialaka1
    - pripadne zmeny jsem zakomentovaval
    - dimension jsem udelal globalni a opravil v getPositionOfSpace
    - swap() -- univerzalni prohozeni prvku
    - checkCoord() -- kontrola souradnic pohybu
    - nejak jsem vyprasil ty swap*() -- lisi se jen +/- u souradnice
    - printTriangle() -- vypise triangle
    - printTriangleExt() -- vypise a prida popisek
    - debugSwaps() -- k otestovani pohybu
    - do mainloop jsem naznacil reseni dale

 */

using namespace std;


int maxMoves;
int dimension;

typedef struct {
    int x;
    int y;
} Coord;

// print triangle like "triangle"
void printTriangle(int **triangle) {
    for (int i=0; i<dimension; i++) {
        for (int j=0; j<=i; j++) {
            cout << triangle[i][j] << " ";
        }
        cout << endl;
    }
}

bool isTriangleSolved(int **triangle) {
    
    
    return false;
}

bool canMoves(int moves) {
    return (moves < maxMoves);
}

Coord getPositionOfSpace(int **triangle, int dimension) {
    Coord coord;
    
    for (int i=0; i<dimension; i++) {
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

// swap two numbers in triangle
void swap(int **triangle, Coord num, Coord space) {
    int tmp = triangle[num.x][num.y];
    triangle[num.x][num.y] = triangle[space.x][space.y];
    triangle[space.x][space.y] = tmp;
}

// check coordinates
bool checkCoord(Coord coord) {
    if (coord.x < 0)
        return false;
    if (coord.y < 0)
        return false;
    if (coord.x < coord.y)
        return false;
    if (coord.x >= dimension)
        return false;
    if (coord.y >= dimension)
        return false;
    return true;
}

// swap* logic
void swapThem(int **triangle, int add_x, int add_y) {
    Coord coord_space = getPositionOfSpace(triangle, dimension);
    Coord coord_num;
    coord_num.x = coord_space.x + add_x;
    coord_num.y = coord_space.y + add_y;
    
    if (checkCoord(coord_num)) {
        swap(triangle, coord_num, coord_space);
    }    
}

void swapLeft(int **triangle) {
    swapThem(triangle, 0, -1);
}

void swapRight(int **triangle) {
    swapThem(triangle, 0, 1);
}

void swapUpLeft(int **triangle) {
    swapThem(triangle, -1, -1);
}

void swapUpRight(int **triangle) {
    swapThem(triangle, -1, 0);
}

void swapDownLeft(int **triangle) {
    swapThem(triangle, 1, 0);
}

void swapDownRight(int **triangle) {
    swapThem(triangle, 1, 1);
}

void saveResult(int steps, int **triangle) {
    
}

// call printTriangle and add description
void printTriangleExt(int **triangle, string desc) {
    cout << "Triangle - " << desc << endl;    
    printTriangle(triangle);
    cout << "===== =====" << endl << endl;
}

// debug swaps
void debugSwaps(int **triangle) {
    printTriangleExt(triangle, "vstup");
    swapLeft(triangle);
    printTriangleExt(triangle, "left");
    swapRight(triangle);
    printTriangleExt(triangle, "right");    
    swapUpLeft(triangle);
    printTriangleExt(triangle, "up-left");    
    swapUpLeft(triangle);
    printTriangleExt(triangle, "up-left");    
    swapUpRight(triangle);
    printTriangleExt(triangle, "up-right");    
    swapDownRight(triangle);
    printTriangleExt(triangle, "down-right");    
    swapDownLeft(triangle);
    printTriangleExt(triangle, "down-left");    
}

void mainProccesLoop(int **triangle/*, Coord coord, int dimension*/) {
    debugSwaps(triangle);
    
    /* // ZAKOMENTOVANO
    stack<int**> cstack;
    cstack.push(triangle); // tohle bude spatne.. potrebujeme znat jeste pocet provedenych kroku.. nejaka struktura ve strukture???
    
    while (cstack.size() > 0) {
        // vezmu jednu konfiguraci
        triangle = cstack.top();
        cstack.pop();
        
        // debug vypis
        printTriangle(triangle);
        
        if (!canMoves(1)) {
            cout << "Wrong way" << endl;
            continue;
        }
        if (isTriangleSolved(triangle)) {
            // mame jedno reseni
            cout << "Result!" << endl;
            saveResult(1,triangle);
            continue;
        }
        
        // !!! tady asi budeme muset udelat hardcopy toho trojuhelniku a udelat na nem tah a ulozit.. to vse 6x pro kazdy tah !!!
        // nebo muzeme mit jen jeden trojuhelnik a na zasobnik ukladat pole kroku (0 je left, 1 je right, ...)
        // a to pole bude mit max velikost dimension^2 ... dalsi krok by pak byl ==> vezmi pocatek -> proved rekonstrukci konfigurace -> proved tah -> uloz tahy
        
        
        //copy(&triangle[0][0], &triangle[0][0]+dimension*dimension,&new_tr[0][0]);
        
        //swapLeft(new_tr);
        //printTriangleExt(triangle, "puvodni");
        //printTriangleExt(new_tr, "novy");
        //cstack.push(new_tr);
    }*/
}


int main () {
    int **triangle = NULL;
    
    dimension = 5;
    maxMoves = dimension * dimension;
    
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

    //Coord coord = getPositionOfSpace(triangle, dimension);

    mainProccesLoop (triangle/*, coord, dimension*/);
    
    
    // Clean up
    for (int i=0; i<dimension; i++) {
        delete triangle[i];
    }
    delete triangle;
    
    return 0;
}
