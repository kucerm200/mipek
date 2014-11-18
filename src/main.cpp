#include "mpi.h"
#include <iostream>
#include <stack>
#include <cstring> // memcpy
#include <cstdio>
#include <sstream>
#include <fstream>
#include <unistd.h> // usleep


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

#define DEBUG 1
//#define DEBUG2 1
#define DEBUG3 1
#define LENGTH 100

#define BLACK 0
#define WHITE 1

#define WAIT_FOR_JOB 9100
#define WAIT_FOR_RESULT 9200
#define WAIT_FOR_FINISH 9300

using namespace std;

MPI_Status status;
int my_rank, p, flag;

int maxMoves;
int dimension;
int ** triangle = NULL;
int result;
int results_num;
int ** copyOfOriginalTriangle = NULL;

bool hasToken;
int token;

double t1, t2;

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

Configuration * createConfiguration(Configuration * configuration, char value) {
    Configuration * newconfig;
    newconfig = new Configuration;
    newconfig->movesCount = configuration->movesCount + 1;
    newconfig->moves = new char[newconfig->movesCount];
    // Copy old moves
    copy(configuration->moves, configuration->moves + configuration->movesCount, newconfig->moves);
    // Add new move
    newconfig->moves[newconfig->movesCount-1] = value;
    
    return newconfig;
}

void processWrite(string msg) {
    cout << my_rank << "\t: " << msg << endl << flush;
}

void processWriteDebug(string msg) {
#ifdef DEBUG
    cout << my_rank << "\t: " << msg << endl << flush;
#endif    
}

void processWriteDebug2(string msg) {
#ifdef DEBUG2
    cout << my_rank << "\t: " << msg << endl << flush;
#endif    
}

void processWriteDebug3(string msg) {
#ifdef DEBUG3
    cout << my_rank << "\t: " << msg << endl << flush;
#endif    
}

// job request 

bool comWin_send_job() { // TODO
    processWriteDebug2("Entering send_job");
    char message[LENGTH];
    MPI_Iprobe(MPI_ANY_SOURCE, WAIT_FOR_JOB, MPI_COMM_WORLD, &flag, &status);
    if (flag) {
        /* receiving message by blocking receive */
        MPI_Recv(&message, LENGTH, MPI_CHAR, MPI_ANY_SOURCE, WAIT_FOR_JOB, MPI_COMM_WORLD, &status);
        processWrite("Got JOB: " + *message);
        return true;
    }
    return false;
}

bool comWin_recv_job() { // TODO
    processWriteDebug2("Entering recv_job");
    char message[LENGTH];
    MPI_Iprobe(MPI_ANY_SOURCE, WAIT_FOR_JOB, MPI_COMM_WORLD, &flag, &status);
    if (flag) {
        /* receiving message by blocking receive */
        MPI_Recv(&message, LENGTH, MPI_CHAR, MPI_ANY_SOURCE, WAIT_FOR_JOB, MPI_COMM_WORLD, &status);
        processWrite("Got JOB: " + *message);
        return true;
    }
    return false;
}

bool comWin_recv_result() {
    processWriteDebug2("Entering recv_result");
    int message;
    MPI_Iprobe(MPI_ANY_SOURCE, WAIT_FOR_RESULT, MPI_COMM_WORLD, &flag, &status);
    if (flag) {
        /* receiving message by blocking receive */
        MPI_Recv(&message, LENGTH, MPI_INT, MPI_ANY_SOURCE, WAIT_FOR_RESULT, MPI_COMM_WORLD, &status);
        processWrite("Got RESULT: " + to_string(message));
        saveResult(message, triangle);
        return true;
    }    
    return false;
}

bool comWin_send_result() {
    processWriteDebug2("Entering send_result");
    for (int dest = 0 ; dest < p; dest++) {
        if (dest == my_rank) {
            continue;
        }
        MPI_Send(&result, 1, MPI_INT, dest, WAIT_FOR_RESULT, MPI_COMM_WORLD);
    }    
    return true;
}

bool comWin_recv_end() {
    processWriteDebug2("Entering recv_end");
    int message;
    MPI_Iprobe(MPI_ANY_SOURCE, WAIT_FOR_FINISH, MPI_COMM_WORLD, &flag, &status);
    if (flag) {
        /* receiving message by blocking receive */
        MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, WAIT_FOR_FINISH, MPI_COMM_WORLD, &status);
        processWrite("Got FINISH");
        hasToken = true;
        return true;
    }    
    return false;
}

bool comWin_send_end() {
    processWriteDebug3("Entering send_end");
    if (hasToken && token == WHITE) {
        int dest = (my_rank + 1) % p;
        int message = WHITE;
        MPI_Send(&message, 1, MPI_INT, dest, WAIT_FOR_FINISH, MPI_COMM_WORLD);
        // tady by asi melo byt na cekani jestli nekdo rekne ze jeste maka nebo ne
        hasToken = false;
        return true;
    }
    return false;
}

void comWin(int type) { // komunikacni okenko
    int source;
    int flag = 0;

    processWriteDebug("Entering communication window");
    
    switch (type) {
        case WAIT_FOR_JOB:
            while (!comWin_recv_job()) {
                comWin_recv_result();
                if (comWin_recv_end()) {
                    if (comWin_send_end()) {
                        break;
                    }
                }
            }
            break;
        case WAIT_FOR_FINISH:
            comWin_send_end();
            break;
        case WAIT_FOR_RESULT:
            comWin_send_result();
        default:
            // Code
            break;
    }    
    processWriteDebug("Exiting communication window");
    
}

void soft_sync() {
    processWriteDebug("Soft-sync invoked.");
    usleep(1000000);
}

void sync() {
    processWriteDebug("Sync invoked.");
    // synchronization
    MPI_Barrier(MPI_COMM_WORLD);
    usleep(10000);
}

void mainProccesLoop() {
    
    processWriteDebug("Entering mainProccesLoop");
    stack<Configuration *> cstack;
    int programSteps = 0;
    
    // pripravim jednu kopii trojuhelniku
    // priste uz se jen prehraje datama (usetri se alokace a dealokace pameti
    copyOfOriginalTriangle = new int*[dimension];
    for (int x = 0; x < dimension; x++) {
        copyOfOriginalTriangle[x] = new int[x+1];
    }
    
    sync();
    
    // process 0 pripravit první konfiguraci
    if (my_rank == 0) {
        processWrite("Creating firstConfiguration");
        Configuration * firstConfiguration;
        firstConfiguration = new Configuration;
        firstConfiguration->movesCount = 0;
        firstConfiguration->moves = new char[0];
        cstack.push(firstConfiguration);
    } else {
        // ostatni procesy by zde mely cekat az dostanu konfigurace
        processWrite("Waiting for first configuration");
        comWin(WAIT_FOR_JOB);
    }
    soft_sync();
    
    while (cstack.size() > 0) {
        programSteps++;
        // get configuration from stack
        Configuration * configuration = NULL;
        configuration = cstack.top();
        cstack.pop();
        
        if ((programSteps % 1000000) == 0) {
            cout << "Step " << programSteps << " stack size " << cstack.size() << " results " << results_num << " best " << result << endl;
        }
        
        if (!canMoves(configuration->movesCount)) {
            //cout << "Wrong way" << endl;
            delete configuration->moves;
            delete configuration;
            continue;
        }
        
        // isSolved?
        int triangleStatus = checkTriangleStatus(*configuration);
        if (triangleStatus == TRIANGLE_SOLVED) {
            // Triangle solved, save result and clean conf
            saveResult(configuration->movesCount, triangle);
            comWin(WAIT_FOR_RESULT);    // volani pro poslani noveho vysledku
            delete configuration->moves;
            delete configuration;
            soft_sync();
            continue;
        } else if (triangleStatus == INVALID_STEP) {
            // You step out of triangle
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
        
        // delete configuration
        // Odalokovat pamet
        delete configuration->moves;
        delete configuration;
    }
    
    soft_sync();
    
    // Show result
    //cout << "Step " << programSteps << " stack size " << cstack.size() << " results " << results_num << " best " << result << endl;
    //cout << endl << endl << "Pocet reseni: " << results_num << endl << "Nejmene pocet kroku: " << result << endl << endl;
    comWin(WAIT_FOR_FINISH);
}

void initTriangle() {
    triangle = new int*[dimension];
    
    for (int i=0; i<dimension; i++) {
        triangle[i] = new int[i+1];
    }
}

void proccesLine(string line, int lineNumber) {
    int number;
    stringstream ss;
    ss << line;
    
    // Load numbers to triangle
    for (int i=0; i<lineNumber; i++) {
        // Get number from line
        ss >> number;
        // Save number to triangle
        triangle[lineNumber-1][i] = number;
    }
}

bool loadTriangleFromFile(string filePath) {
    string line;
    ifstream file (filePath);
    
    int lineNumber = 1;
    // Check if its correctly open
    if (file.is_open()) {
        // Alloc triangle
        initTriangle();
        // Load all lines
        while (getline(file, line)) {
            // Procces line
            proccesLine(line, lineNumber ++);
        }
        // Check for dummy input
        if (lineNumber != dimension+1) {
            return false;
        }
        // Close file
        file.close();
        
        return true;
    } else {
        return false;
    }
}

void init() {
    maxMoves = dimension * dimension;
    result = maxMoves;
    results_num = 0;
    hasToken = false;
    if (my_rank == 0) {
        hasToken = true;
    }
    token = WHITE;
}

int main (int argc, char **argv) {
    // Check bad number of parameters
    if (argc < 3) {
        return 1;
    }
    
    /* start up MPI */
    MPI_Init(&argc, &argv);
    
    /* find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    /* find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    
    /* time measuring - start */
    t1 = MPI_Wtime();
    
    // Store arguments from command line
    // Format is ./a.out 4 triangle4.txt
    // Where 4 is dimension and triangle4.txt is path to file with input data
    string fileName = argv[2];
    dimension = stoi(argv[1]);
    // Init default values
    init();
    
    loadTriangleFromFile(fileName);
        
    sync();
    
    // Run main procces
    mainProccesLoop();
    
    
    // Clean up data
    cleanUp(triangle);
    
    sync();
    
    /* time measuring - stop */
    t2 = MPI_Wtime();
    
    //printf("%d\t: Elapsed time is %f.\n",my_rank,t2-t1);
    processWrite("Elapsed time is " + to_string(t2-t1));
    
    sync();
    
    /* shut down MPI */
    MPI_Finalize();
    
    return 0;
}
