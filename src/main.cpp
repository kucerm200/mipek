#include <iostream>
#include <stack>
#include <cstring> // memcpy
#include <cstdio>


#define SPACE -1
#define INVALID_POSITION 0

#define TRIANGLE_SOLVED 200
#define INVALID_STEP 400
#define TRIANGLE_NOT_SOLVED 201

#define TOP_LEFT '7'
#define TOP_RIGHT '9'
#define RIGHT '6'
#define LEFT '4'
#define DOWN_LEFT '1'
#define DOWN_RIGHT '3'

/*

19.10. 15:30 - fialaka1
    - tak asi to funguje dobre, ale nekde to dela sileny leaky
    - velikost zasobniku se relativne ustali

17.10. 17:00 - fialaka1
    - zmen je hodne... dost jsem to prekopal a cca to funguje
    - nepada to na segfault... primitivni a hotovy priklady to resi dobre...

    

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
int ** triangle = NULL;
int result;
int results_num;
int ** copyOfOriginalTriangle = NULL;

typedef struct {
    int x;
    int y;
} Coord;

typedef struct {
    int movesCount;
    char * moves;
} Configuration;

// print triangle like "triangle"
void printTriangle(int **triangle) {
    for (int i=0; i<dimension; i++) {
        for (int j=0; j<=i; j++) {
            cout << triangle[i][j] << " ";
        }
        cout << endl;
    }
}

void cleanUp(int **triangle) {
    for (int i=0; i<dimension; i++) {
        delete triangle[i];
    }
    delete triangle;
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
bool swapThem(int **triangle, int add_x, int add_y) {
    Coord coord_space = getPositionOfSpace(triangle, dimension);
    Coord coord_num;
    coord_num.x = coord_space.x + add_x;
    coord_num.y = coord_space.y + add_y;
    
    if (checkCoord(coord_num)) {
        swap(triangle, coord_num, coord_space);
        return true;
    } else {
        return false;
    }
}

bool swapLeft(int **triangle) {
    return swapThem(triangle, 0, -1);
}

bool swapRight(int **triangle) {
    return swapThem(triangle, 0, 1);
}

bool swapUpLeft(int **triangle) {
    return swapThem(triangle, -1, -1);
}

bool swapUpRight(int **triangle) {
    return swapThem(triangle, -1, 0);
}

bool swapDownLeft(int **triangle) {
    return swapThem(triangle, 1, 0);
}

bool swapDownRight(int **triangle) {
    return swapThem(triangle, 1, 1);
}

void saveResult(int steps, int **triangle) {
    //cout << "Result: " << steps << endl;
    if (steps < result)
        result = steps;
    results_num++;
}

bool reconstruction (int **copyOfOriginalTriangle, Configuration configuration) {
    
    bool result = true;
    
//     printTriangle(copyOfOriginalTriangle);
//     for (int x = 0; x < configuration.movesCount; x++)
//         cout << configuration.moves[x] << " ";
//     cout << endl << endl;

    
    for (int i=0; i < configuration.movesCount; i++) {
        // rozhodne o kroku
        if (configuration.moves[i] == TOP_LEFT)
            result = swapUpLeft(copyOfOriginalTriangle);
        
        else if (configuration.moves[i] == TOP_RIGHT)
            result = swapUpRight(copyOfOriginalTriangle);
        
        else if (configuration.moves[i] == RIGHT)
            result = swapRight(copyOfOriginalTriangle);
        
        else if (configuration.moves[i] == LEFT)
            result = swapLeft(copyOfOriginalTriangle);
        
        else if (configuration.moves[i] == DOWN_LEFT)
            result = swapDownLeft(copyOfOriginalTriangle);
        
        else if (configuration.moves[i] == DOWN_RIGHT)
            result = swapDownRight(copyOfOriginalTriangle);
        
        // check every step
        if (!result)
            return false;
    }        
    
    return true;    
}

bool isTriangleSolved(int **triangle) {
    
    // quick check
    if (triangle[0][0] != SPACE)
        return false;
    
    // full check
    int num = 1;
    for (int x = 1; x < dimension; x++) {
        for (int y = 0; y <= x; y++) {
                if (triangle[x][y] != num)
                    return false;
                num++;
        }
    }    
    return true;
}

int checkTriangleStatus(Configuration configuration) {
    
    // Reconstruction of triangle
    //int ** copyOfOriginalTriangle;
    
    // hardcopy
    //copyOfOriginalTriangle = new int*[dimension];
    for (int x = 0; x < dimension; x++) {
        //copyOfOriginalTriangle[x] = new int[x+1];
        for (int y = 0; y <= x; y++) {
            copyOfOriginalTriangle[x][y] = triangle[x][y];
        }
    }
    
    bool isRecostructionDone = reconstruction(copyOfOriginalTriangle, configuration);
    
    if (!isRecostructionDone) {
        //cleanUp(copyOfOriginalTriangle);
        return INVALID_STEP;
    }
    
    if (isTriangleSolved(copyOfOriginalTriangle)) {
        //cleanUp(copyOfOriginalTriangle);
        return TRIANGLE_SOLVED;
    }
    
    //cleanUp(copyOfOriginalTriangle);
    return TRIANGLE_NOT_SOLVED;
}

bool canMoves(int moves) {
    return (moves < result);
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

Configuration * createConfiguration(Configuration * configuration, char value) {
    Configuration * newconfig;
    newconfig = new Configuration;
    newconfig->movesCount = configuration->movesCount + 1;
    newconfig->moves = new char[newconfig->movesCount];
    // tady je copy lehce rychlejsi nez to delat pres cyklus
    copy(configuration->moves, configuration->moves + configuration->movesCount, newconfig->moves);
    /*for (int i = 0; i < configuration->movesCount; i++) {
        newconfig->moves[i] = configuration->moves[i];
    }*/
    newconfig->moves[newconfig->movesCount-1] = value;
    return newconfig;
}

void mainProccesLoop() {
    //debugSwaps(triangle);
    
    stack<Configuration *> cstack;
    
    Configuration * firstConfiguration;
    firstConfiguration = new Configuration;
    firstConfiguration->movesCount = 0;
    firstConfiguration->moves = new char[0];
    cstack.push(firstConfiguration);
    int programSteps = 0;
    
    // pripravim jednu kopii trojuhelniku
    // priste uz se jen prehraje datama (usetri se alokace a dealokace pameti
    copyOfOriginalTriangle = new int*[dimension];
    for (int x = 0; x < dimension; x++) {
        copyOfOriginalTriangle[x] = new int[x+1];        
    }
    
    
    while (cstack.size() > 0) {
        programSteps++;
        // get configuration from stack
        Configuration * configuration = NULL;
        configuration = cstack.top();
        cstack.pop();
            //cout << "still ok 1 !" << endl;

        //cout << "Step " << programSteps << " stack size " << cstack.size() << " results " << results_num << " best " << result << endl;
        if ((programSteps % 1000000) == 0)
            cout << "Step " << programSteps << " stack size " << cstack.size() << " results " << results_num << " best " << result << endl;
        
        if (!canMoves(configuration->movesCount)) {
            //cout << "Wrong way" << endl;
            delete configuration->moves;
            delete configuration;
            continue;
        }
        
        // isSolved?
            //cout << "still ok 1 !" << endl;
        int triangleStatus = checkTriangleStatus(*configuration);
            //cout << "still ok 2 !" << endl;
        if (triangleStatus == TRIANGLE_SOLVED) {
            //cout << "Result!" << endl;
            saveResult(configuration->movesCount, triangle);
            delete configuration->moves;
            delete configuration;
            continue;
        } else if (triangleStatus == INVALID_STEP) {
            //cout << "Invalid step" << endl;
            delete configuration->moves;
            delete configuration;
            continue;
        }
        // create 6 other configurations and save to stack and 6 swaps
        
        if (configuration->moves[configuration->movesCount - 1] != TOP_RIGHT)
            cstack.push(createConfiguration(configuration, DOWN_LEFT));
        
        if (configuration->moves[configuration->movesCount - 1] != TOP_LEFT)
            cstack.push(createConfiguration(configuration, DOWN_RIGHT));
        
        if (configuration->moves[configuration->movesCount - 1] != RIGHT)
            cstack.push(createConfiguration(configuration, LEFT));
        
        if (configuration->moves[configuration->movesCount - 1] != LEFT)
            cstack.push(createConfiguration(configuration, RIGHT));
        
        if (configuration->moves[configuration->movesCount - 1] != DOWN_RIGHT)
            cstack.push(createConfiguration(configuration, TOP_LEFT));        
        
        if (configuration->moves[configuration->movesCount - 1] != DOWN_LEFT)
            cstack.push(createConfiguration(configuration, TOP_RIGHT));
        
            //cout << "still ok 5 !" << endl;
            
        /*
         * 
         * */
            
        // delete configuration
        // Odalokovat pamet
        //delete newconfig.moves;
            //cout << "still ok 6 !" << endl;
        delete configuration->moves;
        delete configuration;
            //cout << "still ok 7 !" << endl;
        //cout << endl;
    }
    cout << "Step " << programSteps << " stack size " << cstack.size() << " results " << results_num << " best " << result << endl;
    cout << endl << endl << "Pocet reseni: " << results_num << endl << "Nejlepsi reseni je: " << result << endl << endl;
}

void loadSampleData() {
    dimension = 5;
    maxMoves = dimension * dimension;
    result = maxMoves;
    results_num = 0;
    
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
    triangle[2][1] = 3;
    triangle[2][2] = 9;
    triangle[3][0] = 6;
    triangle[3][1] = SPACE;
    triangle[3][2] = 2;
    triangle[3][3] = 7;
    triangle[4][0] = 10;
    triangle[4][1] = 11;
    triangle[4][2] = 12;
    triangle[4][3] = 13;
    triangle[4][4] = 14;
}

void loadSampleDataSmall() {
    dimension = 3;
    maxMoves = dimension * dimension;
    result = maxMoves;
    results_num = 0;
    
    triangle = new int*[dimension];
    
    triangle[0] = new int[1];
    triangle[1] = new int[2];
    triangle[2] = new int[3];
    
    triangle[0][0] = 1;
    triangle[1][0] = 3;
    triangle[1][1] = 2;
    triangle[2][0] = SPACE;
    triangle[2][1] = 4;
    triangle[2][2] = 5;
}

void loadSampleData4() {
    dimension = 4;
    maxMoves = dimension * dimension;
    result = maxMoves;
    results_num = 0;
    
    triangle = new int*[dimension];
    
    triangle[0] = new int[1];
    triangle[1] = new int[2];
    triangle[2] = new int[3];
    triangle[3] = new int[4];
    
    triangle[0][0] = 1;
    triangle[1][0] = 3;
    triangle[1][1] = 2;
    triangle[2][0] = 8;
    triangle[2][1] = 4;
    triangle[2][2] = 5;
    triangle[3][0] = 6;
    triangle[3][1] = 7;
    triangle[3][2] = SPACE;
    triangle[3][3] = 9;
}

int main () {
    
    loadSampleData();
    
    //loadSampleData4();
    
    //loadSampleDataSmall();

    mainProccesLoop();

    cleanUp(triangle);
    
    return 0;
}
