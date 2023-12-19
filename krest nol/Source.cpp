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
    int wins;      // Количество выигрышей из узла

    TreeNode(int r, int c) : move{ r, c }, children{}, wins(0) {}
};

const int BOARD_SIZE = 3;
char board[BOARD_SIZE][BOARD_SIZE];

#pragma region Базовые функции + проверки исключений
void initBoard() {
    //в начале игры делаем доску пустой
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board[i][j] = ' ';
        }
    }
}
void printBoard() {
    //печать доски
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
    //проверка хода
    __try {
        if (cin.fail()) {
            //введено не целое число
            RaiseException(STATUS_DATATYPE_MISALIGNMENT, EXCEPTION_NONCONTINUABLE, 2, nullptr);
        }
        __try {
            if (row < 0 || row > 3 || col < 0 || col > 3) {
                //число вне границ допустимого диапозона
                RaiseException(EXCEPTION_INT_OVERFLOW, EXCEPTION_NONCONTINUABLE, 2, nullptr);
            }
            __try
            {
                if (board[row - 1][col - 1] == 'X' || board[row - 1][col - 1] == 'O') {
                    //в ячейке уже стоит какой-то символ
                    RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 2, nullptr);
                }
            }
            __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
                cout << "В этой клетке уже стоит символ, повторите ввод: ";
                cin.clear();  // Сбрасываем флаг ошибки ввода
                cin.ignore(100, '\n');  // Очищаем буфер ввода
                return false;
            }
        }
        __except (GetExceptionCode() == EXCEPTION_INT_OVERFLOW ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
            cout << "Вы ввели число вне границ допустимого диапозона, повторите ввод: ";
            cin.clear();  // Сбрасываем флаг ошибки ввода
            cin.ignore(100, '\n');  // Очищаем буфер ввода
            return false;
        }
    }
    //исключение на принадлежность целому типу
    __except (GetExceptionCode() == STATUS_DATATYPE_MISALIGNMENT ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        cout << "Вы ввели не числa, повторите ввод: ";
        cin.clear();  // Сбрасываем флаг ошибки ввода
        cin.ignore(100, '\n');  // Очищаем буфер ввода
        return false;
    }
    return true;
}

bool isIntValid(int n, int min, int max) {
    //проверка выбора
    __try {
        if (cin.fail()) {
            //введено не целое число
            RaiseException(STATUS_DATATYPE_MISALIGNMENT, EXCEPTION_NONCONTINUABLE, 2, nullptr);
        }
        __try {
            if (n < min || n > max) {
                //число вне границ допустимого диапозона
                RaiseException(EXCEPTION_INT_OVERFLOW, EXCEPTION_NONCONTINUABLE, 2, nullptr);
            }
        }
        __except (GetExceptionCode() == EXCEPTION_INT_OVERFLOW ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
            cout << "Вы ввели число вне границ допустимого диапозона, повторите ввод: ";
            cin.clear();  // Сбрасываем флаг ошибки ввода
            cin.ignore(100, '\n');  // Очищаем буфер ввода
            return false;
        }
    }
    //исключение на принадлежность целому типу
    __except (GetExceptionCode() == STATUS_DATATYPE_MISALIGNMENT ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        cout << "Вы ввели не числa, повторите ввод: ";
        cin.clear();  // Сбрасываем флаг ошибки ввода
        cin.ignore(100, '\n');  // Очищаем буфер ввода
        return false;
    }
    return true;
}

bool checkWin(char player) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        //проверка строк и столбцов
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
            (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
            return true;
        }
    }
    //проверка диагоналей
    if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
        (board[0][2] == player && board[1][1] == player && board[2][0] == player)) {
        return true;
    }

    return false;
}

bool isBoardFull() {
    //проверка что нет доступных ходов
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
    cout << "Введите строку и столбец в которые хотите поставить крестик: ";
    do {
        //ввод хода игрока
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


#pragma region Печать дерева

struct Trunk //структура для красивой печати дерева
{
    Trunk* prev; //ссылка на предыдущий уровень структуры
    string str; //строка для отображения "ветки" дерева

    Trunk(Trunk* prev, string str)
    {
        this->prev = prev;
        this->str = str;
    }
};
//вспомогательная функция для вывода отступов в консол
void showTrunks(Trunk* p)
{
    if (p == nullptr) {
        return;
    }

    showTrunks(p->prev); //рекурсивно выводим предыдущие отступы
    cout << p->str; //выводим текущий отступ
}

void printTree(TreeNode* root, Trunk* prev, bool isLeft)
{
    if (root == nullptr) {
        return;
    }

    string prev_str = "    "; // Отступ по умолчанию
    Trunk* trunk = new Trunk(prev, prev_str);

    for (TreeNode* child : root->children) {
        printTree(child, trunk, true); // Выводим детей текущего узла
    }

    if (!prev) {
        trunk->str = "———"; // Отступ для корневого узла
    }
    else {
        trunk->str = (isLeft) ? ".———" : "`———"; // Отступ для левого или правого поддерева
        prev_str = (isLeft) ? "   |" : prev_str;
    }

    showTrunks(trunk); // Вывод отступов
    cout << " (" << root->move.row << ", " << root->move.col << ") [W: " << root->wins << "]" << endl; // Вывод значения

    if (prev) {
        prev->str = prev_str;
    }
    trunk->str = "   |"; // Отступ для вывода |

    for (TreeNode* child : root->children) {
        printTree(child, trunk, false); // Рекурсивно вызываем функцию для левого и правого поддеревьев
    }
}
#pragma endregion

int countTreeElements(TreeNode* root) {
    if (root == nullptr) {
        return 0;
    }

    int count = 1; // Учитываем текущий узел

    for (TreeNode* child : root->children) {
        count += countTreeElements(child); // Учитываем детей текущего узла
    }

    return count;
}

TreeNode* findBestMove(TreeNode* root)
{
    // Выбираем лучший ход, исходя из счета выигрышей и посещений
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
    //работа  нулевыми ссылками дерева
    __try {
        board[bestMove->move.row][bestMove->move.col] = 'O';
    }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        //cout << "Зафиксировано исключение при работе дерева" << endl;
        bestMove = root.children.front();
        board[bestMove->move.row][bestMove->move.col] = 'O';
    }
}

void simulateGame(TreeNode* node, char player) {
    // Симуляция игры до конца, выбирая случайные ходы
    char currentPlayer = player;

    while (true) {
        int emptyCells = 0;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board[i][j] == ' ') {
                    emptyCells++; //пустые клетки
                }
            }
        }

        if (emptyCells == 0) {
            break; // Ничья
            //выход из симуляции
        }

        int randomRow, randomCol; //рандомные значения хода
        do {
            randomRow = rand() % 3;
            randomCol = rand() % 3;
        } while (board[randomRow][randomCol] != ' ');

        board[randomRow][randomCol] = currentPlayer;
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X'; //меняем игрока для симуляции

        if (checkWin(player)) {
            node->wins++; // Обновляем счет выигрышей для текущего узла
            break;
        }
    }
}

void monteCarlo(TreeNode* node, char player, int simulations) {
    for (int i = 0; i < simulations; ++i) {
        // Копируем текущее состояние доски
        char originalBoard[BOARD_SIZE][BOARD_SIZE];
        memcpy(originalBoard, board, sizeof(board));

        // Делаем ход в соответствии с узлом
        board[node->move.row][node->move.col] = player;

        // Симулируем игру с использованием копии доски
        simulateGame(node, player);

        // Восстанавливаем доску
        memcpy(board, originalBoard, sizeof(board));
    }
}


queue<TreeNode*> missingNodesQueue; //очередь узлов
binary_semaphore treeSemaphore(0); //семафор для взаимодействия потоков
binary_semaphore terminateSemaphore(0); //семафор для выхода

void traverseLeftMostPath(TreeNode* root) {
    TreeNode* current = root;
    while (!current->children.empty()) {
        current = current->children.back();
    }
    // В этот момент current - "крайний левый" узел
    missingNodesQueue.push(current); //добавляем узел в очередь
    treeSemaphore.release(); //освобождаем семафор
}

void processMissingNodes() {
    while (true) {
        treeSemaphore.acquire(); //блокируем семафор

        if (!missingNodesQueue.empty()) {
            TreeNode* missingNode = missingNodesQueue.front(); //извлекаем указатель на пропущенный узел
            missingNodesQueue.pop(); //удаляем из очереди

            // Обрабатываем пропущенный узел
            monteCarlo(missingNode, 'O', 100);
        }

        // Проверяем семафор завершения
        if (terminateSemaphore.try_acquire_for(chrono::milliseconds(0))) {
            break;  // Выход из цикла, если получен семафор завершения
        }
    }
}

void computerMove() {
    TreeNode root(-1, -1); // Корневой узел без координат

    generateMoves(&root, 'O', 0, 3);

    thread leftMostPathThread(traverseLeftMostPath, &root); //поток для обхода левых узлов
    thread processMissingNodesThread(processMissingNodes);    // Поток для обработки пропущенных узлов

    // Дожидаемся завершения обхода "крайне левого" пути
    leftMostPathThread.join();
    // Устанавливаем семафор завершения для потока processMissingNodes
    terminateSemaphore.release();
    // Дожидаемся завершения обработки пропущенных узлов
    processMissingNodesThread.join();

    // Создаем future для выполнения задач параллельно
    future<TreeNode*> bestMoveFuture = async(findBestMove, &root);
    future<int> countElementsFuture = async(countTreeElements, &root);

    // Получаем результаты из future
    TreeNode* bestMove = bestMoveFuture.get();
    int countElements = countElementsFuture.get();

    // Обновляем доску с лучшим ходом
    tryComputerMove(bestMove, root);

    // Вывод информации о дереве
    cout << "Дерево состоит из " << countElements << " элементов" << endl;
    //возможности при печати
    //if (countTreeElements(&root) <=10) {
    //    printTree(&root, nullptr, false);
    //}

    // Очистка памяти, выделенной для дерева
    for (TreeNode* child : root.children) {
        delete child;
    }
}

#pragma region Файлы
void saveGameToFile(ofstream& outFile) {

    if (outFile.is_open()) {
        // Сохранение состояния доски
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                outFile << board[i][j];
            }
            outFile << endl;
        }
    }
    else {
        cout << "Ошибка при открытии файла для сохранения." << endl;
    }
}

void loadGameFromFile(ifstream& inFile) {

    // Перемещение к концу файла
    inFile.seekg(0, ios_base::end);

    // Загрузка последней строки
    for (int i = BOARD_SIZE - 1; i >= 0; --i) {
        inFile.seekg(-5, ios_base::cur); //перемещение на начало строки

        string line;
        getline(inFile, line); //считывание

        // Заполнение строки в доске
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board[i][j] = line[j];
        }
        inFile.seekg(-5, ios_base::cur); //переход к началу этой строки, чтобы затем перейти на строчку выше
    }
    inFile.close();
}
bool isLastGameIncomplete(string fileName) {
    ifstream inFile(fileName);

    if (inFile.is_open()) {
        inFile.seekg(-8, ios_base::end);
        // Проверка последней строки
        string lastLine;
        getline(inFile, lastLine);
        inFile.close();
        // Если последняя строка содержит фразу окончания игры, игра завершена
        return lastLine.find("End game") == string::npos;
    }
    else {
        HANDLE hFile = CreateFileA("game_history.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            cout << "Не удалось создать файл" << endl;
        }
        return false;
    }
}

#pragma endregion

typedef bool(CALLBACK* LPFNISINTVALID)(int, int, int); //псевдоним функции

int main() {    
    setlocale(LC_ALL, "Russian");

    string filename = "game_history.txt";

    HINSTANCE hDLL = LoadLibrary(L"KrestLibrary.dll"); //загрузить библиотеку
    LPFNISINTVALID isIntValidFunc = reinterpret_cast<LPFNISINTVALID>(GetProcAddress(hDLL, "isIntValid")); //загружаю функцию


    if (isLastGameIncomplete(filename)) {
        cout << "Последняя игра не завершена. Выберите действие:" << endl;
        cout << "1. Начать новую игру" << endl;
        cout << "2. Продолжить последнюю игру" << endl;

        int choice; //проверка ввода
        do
        {
            cin >> choice;
        } while (!isIntValid(choice, 1, 2));

        if (choice == 2) {
            //продолжение предыдущей игры
            ifstream infile(filename);
            loadGameFromFile(infile);
        }
        else
        {
            cout << "Вы начали новую игру" << endl;
            initBoard(); //создаем пустую доску
        }
    }
    else
    {
        cout << "Добро пожаловать в новую игру!" << endl;
        initBoard(); //создаем пустую доску
    }

    ofstream outfile(filename);
 
    printBoard(); //рисуем доску
    saveGameToFile(outfile); //сохраняем в файл (даже пустую доску)

    while (true) {
        playerMove(); //ход игрока
        printBoard(); //печать доски
        //saveGameToFile(outfile); //сохранение игры в файл

        if (checkWin('X')) {
            cout << "You win!\n"; //если победа крестиков
            outfile << "End game"; //запись в файл об окончании игры
            break;
        }
        else if (isBoardFull()) {
            cout << "It's a draw!\n"; //если некуда поставить символ
            outfile << "End game";  //запись в файл об окончании игры
            break;
        }

        computerMove(); //ход компьютера
        printBoard(); //печать доски
        saveGameToFile(outfile); //сохранение игры в файл

        if (checkWin('O')) {
            cout << "Computer wins!\n"; //если победа ноликов
            outfile << "End game";  //запись в файл об окончании игры
            break;
        }
        //тк последним ходит игрок, ход компьютера не может привести к ничье
    }
    FreeLibrary(hDLL);

    return 0;
}