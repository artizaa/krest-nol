#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath> 
#include <vector>
#include <iomanip>
#include <future>
#include <string>  
#include <cstring>
#include <windows.h> 
#include <excpt.h>
#include<fstream>
#include<fileapi.h>
#include <thread>
#include <queue>
#include <stack>
#include <mutex>
#include <semaphore>

//#include "KrestLibrary.h"

using namespace std;

struct Move {
    int row;
    int col;
};

struct TreeNode {
    Move move;
    vector<TreeNode*> children;
    int wins;      // ���������� ��������� �� ����

    TreeNode(int r, int c) : move{ r, c }, children{}, wins(0) {}
};

const int BOARD_SIZE = 3;
char board[BOARD_SIZE][BOARD_SIZE];

#pragma region ������� ������� + �������� ����������
void initBoard() {
    //� ������ ���� ������ ����� ������
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board[i][j] = ' ';
        }
    }
}
void printBoard() {
    //������ �����
    cout << "-------------\n";
    for (int i = 0; i < BOARD_SIZE; ++i) {
        cout << "| ";
        for (int j = 0; j < BOARD_SIZE; ++j) {
            cout << board[i][j] << " | ";
        }
        cout << "\n-------------\n";
    }
}



bool isMoveValid(int row, int col) {
    //�������� ����
    __try {
        if (cin.fail()) {
            //������� �� ����� �����
            RaiseException(STATUS_DATATYPE_MISALIGNMENT, EXCEPTION_NONCONTINUABLE, 2, nullptr);
        }
        __try {
            if (row < 0 || row > 3 || col < 0 || col > 3) {
                //����� ��� ������ ����������� ���������
                RaiseException(EXCEPTION_INT_OVERFLOW, EXCEPTION_NONCONTINUABLE, 2, nullptr);
            }
            __try
            {
                if (board[row - 1][col - 1] == 'X' || board[row - 1][col - 1] == 'O') {
                    //� ������ ��� ����� �����-�� ������
                    RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 2, nullptr);
                }
            }
            __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
                cout << "� ���� ������ ��� ����� ������, ��������� ����: ";
                cin.clear();  // ���������� ���� ������ �����
                cin.ignore(100, '\n');  // ������� ����� �����
                return false;
            }
        }
        __except (GetExceptionCode() == EXCEPTION_INT_OVERFLOW ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
            cout << "�� ����� ����� ��� ������ ����������� ���������, ��������� ����: ";
            cin.clear();  // ���������� ���� ������ �����
            cin.ignore(100, '\n');  // ������� ����� �����
            return false;
        }
    }
    //���������� �� �������������� ������ ����
    __except (GetExceptionCode() == STATUS_DATATYPE_MISALIGNMENT ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        cout << "�� ����� �� ����a, ��������� ����: ";
        cin.clear();  // ���������� ���� ������ �����
        cin.ignore(100, '\n');  // ������� ����� �����
        return false;
    }
    return true;
}

bool isIntValid(int n, int min, int max) {
    //�������� ������
    __try {
        if (cin.fail()) {
            //������� �� ����� �����
            RaiseException(STATUS_DATATYPE_MISALIGNMENT, EXCEPTION_NONCONTINUABLE, 2, nullptr);
        }
        __try {
            if (n < min || n > max) {
                //����� ��� ������ ����������� ���������
                RaiseException(EXCEPTION_INT_OVERFLOW, EXCEPTION_NONCONTINUABLE, 2, nullptr);
            }
        }
        __except (GetExceptionCode() == EXCEPTION_INT_OVERFLOW ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
            cout << "�� ����� ����� ��� ������ ����������� ���������, ��������� ����: ";
            cin.clear();  // ���������� ���� ������ �����
            cin.ignore(100, '\n');  // ������� ����� �����
            return false;
        }
    }
    //���������� �� �������������� ������ ����
    __except (GetExceptionCode() == STATUS_DATATYPE_MISALIGNMENT ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        cout << "�� ����� �� ����a, ��������� ����: ";
        cin.clear();  // ���������� ���� ������ �����
        cin.ignore(100, '\n');  // ������� ����� �����
        return false;
    }
    return true;
}

bool checkWin(char player) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        //�������� ����� � ��������
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
            (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
            return true;
        }
    }
    //�������� ����������
    if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
        (board[0][2] == player && board[1][1] == player && board[2][0] == player)) {
        return true;
    }

    return false;
}

bool isBoardFull() {
    //�������� ��� ��� ��������� �����
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}
#pragma endregion


void playerMove() {
    int row, col;
    cout << "������� ������ � ������� � ������� ������ ��������� �������: ";
    do {
        //���� ���� ������
        cin >> row >> col;
    } while (!isMoveValid(row, col));
    row--;
    col--;
    board[row][col] = 'X';
}


void generateMoves(TreeNode* parent, char player, int startRow=0, int endRow= BOARD_SIZE) {
    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == ' ') {
                TreeNode* child = new TreeNode(i, j);
                parent->children.push_back(child);

                board[i][j] = player;
                generateMoves(child, (player == 'X') ? 'O' : 'X', startRow, endRow);
                board[i][j] = ' ';
            }
        }
    }
}


#pragma region ������ ������

struct Trunk //��������� ��� �������� ������ ������
{
    Trunk* prev; //������ �� ���������� ������� ���������
    string str; //������ ��� ����������� "�����" ������

    Trunk(Trunk* prev, string str)
    {
        this->prev = prev;
        this->str = str;
    }
};
//��������������� ������� ��� ������ �������� � ������
void showTrunks(Trunk* p)
{
    if (p == nullptr) {
        return;
    }

    showTrunks(p->prev); //���������� ������� ���������� �������
    cout << p->str; //������� ������� ������
}

void printTree(TreeNode* root, Trunk* prev, bool isLeft)
{
    if (root == nullptr) {
        return;
    }

    string prev_str = "    "; // ������ �� ���������
    Trunk* trunk = new Trunk(prev, prev_str);

    for (TreeNode* child : root->children) {
        printTree(child, trunk, true); // ������� ����� �������� ����
    }

    if (!prev) {
        trunk->str = "���"; // ������ ��� ��������� ����
    }
    else {
        trunk->str = (isLeft) ? ".���" : "`���"; // ������ ��� ������ ��� ������� ���������
        prev_str = (isLeft) ? "   |" : prev_str;
    }

    showTrunks(trunk); // ����� ��������
    cout << " (" << root->move.row << ", " << root->move.col << ") [W: " << root->wins << "]" << endl; // ����� ��������

    if (prev) {
        prev->str = prev_str;
    }
    trunk->str = "   |"; // ������ ��� ������ |

    for (TreeNode* child : root->children) {
        printTree(child, trunk, false); // ���������� �������� ������� ��� ������ � ������� �����������
    }
}
#pragma endregion

int countTreeElements(TreeNode* root) {
    if (root == nullptr) {
        return 0;
    }

    int count = 1; // ��������� ������� ����

    for (TreeNode* child : root->children) {
        count += countTreeElements(child); // ��������� ����� �������� ����
    }

    return count;
}

TreeNode* findBestMove(TreeNode* root)
{
    // �������� ������ ���, ������ �� ����� ��������� � ���������
    TreeNode* bestMove = nullptr;
    int bestScore = -1;

    for (TreeNode* child : root->children) {
        int score = static_cast<int>(child->wins);

        if (score > bestScore) {
            bestScore = score;
            bestMove = child;
        }
    }
    return bestMove;
}


void tryComputerMove(TreeNode* bestMove, TreeNode& root)
{
    //������  �������� �������� ������
    __try {
        board[bestMove->move.row][bestMove->move.col] = 'O';
    }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        //cout << "������������� ���������� ��� ������ ������" << endl;
        bestMove = root.children.front();
        board[bestMove->move.row][bestMove->move.col] = 'O';
    }
}

void simulateGame(TreeNode* node, char player) {
    // ��������� ���� �� �����, ������� ��������� ����
    char currentPlayer = player;

    while (true) {
        int emptyCells = 0;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board[i][j] == ' ') {
                    emptyCells++; //������ ������
                }
            }
        }

        if (emptyCells == 0) {
            break; // �����
            //����� �� ���������
        }

        int randomRow, randomCol; //��������� �������� ����
        do {
            randomRow = rand() % 3;
            randomCol = rand() % 3;
        } while (board[randomRow][randomCol] != ' ');

        board[randomRow][randomCol] = currentPlayer;
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X'; //������ ������ ��� ���������

        if (checkWin(player)) {
            node->wins++; // ��������� ���� ��������� ��� �������� ����
            break;
        }
    }
}

void monteCarlo(TreeNode* node, char player, int simulations) {
    for (int i = 0; i < simulations; ++i) {
        // �������� ������� ��������� �����
        char originalBoard[BOARD_SIZE][BOARD_SIZE];
        memcpy(originalBoard, board, sizeof(board));

        // ������ ��� � ������������ � �����
        board[node->move.row][node->move.col] = player;

        // ���������� ���� � �������������� ����� �����
        simulateGame(node, player);

        // ��������������� �����
        memcpy(board, originalBoard, sizeof(board));
    }
}


queue<TreeNode*> missingNodesQueue; //������� �����
binary_semaphore treeSemaphore(0); //������� ��� �������������� �������
binary_semaphore terminateSemaphore(0); //������� ��� ������

void traverseLeftMostPath(TreeNode* root) {
    TreeNode* current = root;
    while (!current->children.empty()) {
        current = current->children.back();
    }
    // � ���� ������ current - "������� �����" ����
    missingNodesQueue.push(current); //��������� ���� � �������
    treeSemaphore.release(); //����������� �������
}

void processMissingNodes() {
    while (true) {
        treeSemaphore.acquire(); //��������� �������

        if (!missingNodesQueue.empty()) {
            TreeNode* missingNode = missingNodesQueue.front(); //��������� ��������� �� ����������� ����
            missingNodesQueue.pop(); //������� �� �������

            // ������������ ����������� ����
            monteCarlo(missingNode, 'O', 100);
        }

        // ��������� ������� ����������
        if (terminateSemaphore.try_acquire_for(chrono::milliseconds(0))) {
            break;  // ����� �� �����, ���� ������� ������� ����������
        }
    }
}

void computerMove() {
    TreeNode root(-1, -1); // �������� ���� ��� ���������

    generateMoves(&root, 'O', 0, 3);

    thread leftMostPathThread(traverseLeftMostPath, &root); //����� ��� ������ ����� �����
    thread processMissingNodesThread(processMissingNodes);    // ����� ��� ��������� ����������� �����

    // ���������� ���������� ������ "������ ������" ����
    leftMostPathThread.join();
    // ������������� ������� ���������� ��� ������ processMissingNodes
    terminateSemaphore.release();
    // ���������� ���������� ��������� ����������� �����
    processMissingNodesThread.join();

    // ������� future ��� ���������� ����� �����������
    future<TreeNode*> bestMoveFuture = async(findBestMove, &root);
    future<int> countElementsFuture = async(countTreeElements, &root);

    // �������� ���������� �� future
    TreeNode* bestMove = bestMoveFuture.get();
    int countElements = countElementsFuture.get();

    // ��������� ����� � ������ �����
    tryComputerMove(bestMove, root);

    // ����� ���������� � ������
    cout << "������ ������� �� " << countElements << " ���������" << endl;
    //����������� ��� ������
    //if (countTreeElements(&root) <=10) {
    //    printTree(&root, nullptr, false);
    //}

    // ������� ������, ���������� ��� ������
    for (TreeNode* child : root.children) {
        delete child;
    }
}

#pragma region �����
void saveGameToFile(ofstream& outFile) {

    if (outFile.is_open()) {
        // ���������� ��������� �����
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                outFile << board[i][j];
            }
            outFile << endl;
        }
    }
    else {
        cout << "������ ��� �������� ����� ��� ����������." << endl;
    }
}

void loadGameFromFile(ifstream& inFile) {

    // ����������� � ����� �����
    inFile.seekg(0, ios_base::end);

    // �������� ��������� ������
    for (int i = BOARD_SIZE - 1; i >= 0; --i) {
        inFile.seekg(-5, ios_base::cur); //����������� �� ������ ������

        string line;
        getline(inFile, line); //����������

        // ���������� ������ � �����
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board[i][j] = line[j];
        }
        inFile.seekg(-5, ios_base::cur); //������� � ������ ���� ������, ����� ����� ������� �� ������� ����
    }
    inFile.close();
}
bool isLastGameIncomplete(string fileName) {
    ifstream inFile(fileName);

    if (inFile.is_open()) {
        inFile.seekg(-8, ios_base::end);
        // �������� ��������� ������
        string lastLine;
        getline(inFile, lastLine);
        inFile.close();
        // ���� ��������� ������ �������� ����� ��������� ����, ���� ���������
        return lastLine.find("End game") == string::npos;
    }
    else {
        HANDLE hFile = CreateFileA("game_history.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            cout << "�� ������� ������� ����" << endl;
        }
        return false;
    }
}

#pragma endregion

typedef bool(CALLBACK* LPFNISINTVALID)(int, int, int); //��������� �������

int main() {    
    setlocale(LC_ALL, "Russian");

    string filename = "game_history.txt";

    HINSTANCE hDLL = LoadLibrary(L"KrestLibrary.dll"); //��������� ����������
    LPFNISINTVALID isIntValidFunc = reinterpret_cast<LPFNISINTVALID>(GetProcAddress(hDLL, "isIntValid")); //�������� �������


    if (isLastGameIncomplete(filename)) {
        cout << "��������� ���� �� ���������. �������� ��������:" << endl;
        cout << "1. ������ ����� ����" << endl;
        cout << "2. ���������� ��������� ����" << endl;

        int choice; //�������� �����
        do
        {
            cin >> choice;
        } while (!isIntValid(choice, 1, 2));

        if (choice == 2) {
            //����������� ���������� ����
            ifstream infile(filename);
            loadGameFromFile(infile);
        }
        else
        {
            cout << "�� ������ ����� ����" << endl;
            initBoard(); //������� ������ �����
        }
    }
    else
    {
        cout << "����� ���������� � ����� ����!" << endl;
        initBoard(); //������� ������ �����
    }

    ofstream outfile(filename);
 
    printBoard(); //������ �����
    saveGameToFile(outfile); //��������� � ���� (���� ������ �����)

    while (true) {
        playerMove(); //��� ������
        printBoard(); //������ �����
        //saveGameToFile(outfile); //���������� ���� � ����

        if (checkWin('X')) {
            cout << "You win!\n"; //���� ������ ���������
            outfile << "End game"; //������ � ���� �� ��������� ����
            break;
        }
        else if (isBoardFull()) {
            cout << "It's a draw!\n"; //���� ������ ��������� ������
            outfile << "End game";  //������ � ���� �� ��������� ����
            break;
        }

        computerMove(); //��� ����������
        printBoard(); //������ �����
        saveGameToFile(outfile); //���������� ���� � ����

        if (checkWin('O')) {
            cout << "Computer wins!\n"; //���� ������ �������
            outfile << "End game";  //������ � ���� �� ��������� ����
            break;
        }
        //�� ��������� ����� �����, ��� ���������� �� ����� �������� � �����
    }
    FreeLibrary(hDLL);

    return 0;
}