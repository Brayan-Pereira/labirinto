#include <iostream>
#include <vector>
#include <string>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
    #define CLEAR "cls"
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#else
    #include <unistd.h>
    #include <termios.h>
    #define CLEAR "clear"
    char getch_linux() {
        char buf = 0;
        struct termios old = {0};
        tcgetattr(0, &old);
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        tcsetattr(0, TCSANOW, &old);
        read(0, &buf, 1);
        tcsetattr(0, TCSANOW, &old);
        return buf;
    }
#endif

using namespace std;

// DEFINIÇÃO DE CORES
#define RESET          "\033[0m"
#define BRIGHT_RED     "\033[91;1m" 
#define BRIGHT_GREEN   "\033[92;1m" 
#define BRIGHT_YELLOW  "\033[93;1m" 
#define BRIGHT_BLUE    "\033[94;1m" 
#define PURPLE         "\033[35;1m" // ROXO para o rastro de retorno
#define BRIGHT_CYAN    "\033[96;1m" 
#define GRAY           "\033[90m"

const int LINHAS = 30;
const int COLUNAS = 40;

bool visivel[LINHAS][COLUNAS] = {false};

struct No {
    int x, y;
    No* proximo;
};

vector<string> mapa = {
    "########################################", // 0
    "#@#       #           #               #", // 1
    "# # ##### # ######### # ############# #", // 2
    "# # #     # #         # #           # #", // 3
    "# # # ##### # ######### # ######### # #", // 4
    "#   # #     # #         # #       # # #", // 5
    "##### # ##### # ######### # ##### # # #", // 6
    "#     # #     #           # #   # # # #", // 7
    "# ####### ##### ########### # # # # # #", // 8
    "# #     #       #           # # # # # #", // 9
    "# # ### ######### ########### # # # # #", // 10
    "# # # #           #           # #   # #", // 11
    "# # # ############# ########### ##### #", // 12
    "#   #               #           #     #", // 13
    "##### ############### ########### #####", // 14
    "#     #               #               #", // 15
    "# ##### ############### ############# #", // 16
    "# #     #               #           # #", // 17
    "# # ##### ############### ######### # #", // 18
    "# # #     #               #       # # #", // 19
    "# # # ##### ############### ##### # # #", // 20
    "#   # #     #               #   # # # #", // 21
    "##### # ##### ############### # # # # #", // 22
    "#     # #     #               # # # # #", // 23
    "# ####### ##### ############### # # # #", // 24
    "# #             #               # # # #", // 25
    "# # ############# ############### # # #", // 26
    "#                 #                 #E#", // 27
    "########################################", // 28
    "                                        "  // 29
};

// Funções para manter o símbolo e trocar a cor (usando caracteres diferentes internamente)
void atualizaVisao(int pX, int pY) {
    for (int i = pY - 1; i <= pY + 1; i++) {
        for (int j = pX - 1; j <= pX + 1; j++) {
            if (i >= 0 && i < LINHAS && j >= 0 && j < COLUNAS) {
                visivel[i][j] = true;
            }
        }
    }
}

void push(No*& topo, int x, int y) {
    No* novo = new No;
    novo->x = x; novo->y = y;
    novo->proximo = topo;
    topo = novo;
}

bool pop(No*& topo, int& x, int& y) {
    if (topo == nullptr) return false;
    No* temp = topo;
    x = topo->x; y = topo->y;
    topo = topo->proximo;
    delete temp;
    return true;
}

void desenha(int pX, int pY, int passos) {
    system(CLEAR);
    cout << BRIGHT_CYAN << "========================================" << RESET << endl;
    cout << PURPLE << "   MAZE CRAWLER - UniBrasil - EQUIPE D" << RESET << endl;
    cout << BRIGHT_CYAN << "========================================" << RESET << endl;
    cout << " Passos Totais: " << BRIGHT_YELLOW << passos << RESET << " | Visao: " << BRIGHT_GREEN << "3x3 ON" << RESET << endl << endl;

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (i == pY && j == pX) cout << BRIGHT_YELLOW << "@" << RESET;
            else if (!visivel[i][j]) cout << GRAY << "?" << RESET; 
            else if (mapa[i][j] == '#') cout << BRIGHT_RED << "#" << RESET;
            else if (mapa[i][j] == 'E') cout << BRIGHT_GREEN << "E" << RESET;
            else if (mapa[i][j] == '*') cout << BRIGHT_BLUE << "*" << RESET; // Ida (Azul)
            else if (mapa[i][j] == 'R') cout << PURPLE << "*" << RESET;      // Volta (Roxo) - Mantém o símbolo '*'
            else cout << " ";
        }
        cout << endl;
    }
    cout << endl << BRIGHT_CYAN << " CONTROLES: [W,A,S,D] | [U] Undo | [Q] Sair" << RESET << endl;
}

int main() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &dwMode)) {
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif

    No* pilha = nullptr;
    int pX = 1, pY = 1;
    int passos = 0;

    atualizaVisao(pX, pY);

    while (true) {
        desenha(pX, pY, passos);
        
        char tecla;
#ifdef _WIN32
        tecla = tolower(_getch());
#else
        tecla = tolower(getch_linux());
#endif

        if (tecla == 'q') break;

        if (tecla == 'u') {
            if (pop(pilha, pX, pY)) {
                passos++; // Continua contando os passos mesmo no Undo
                mapa[pY][pX] = 'R'; // 'R' de Retorno para pintar de Roxo mantendo o '*'
            }
        } else {
            int nX = pX, nY = pY;
            if (tecla == 'w') nY--;
            else if (tecla == 's') nY++;
            else if (tecla == 'a') nX--;
            else if (tecla == 'd') nX++;
            else continue;

            if (nY >= 0 && nY < LINHAS && nX >= 0 && nX < COLUNAS && mapa[nY][nX] != '#') {
                push(pilha, pX, pY);
                mapa[pY][pX] = '*'; // Ida
                pX = nX;
                pY = nY;
                passos++; // Conta o passo de movimento
                atualizaVisao(pX, pY); 

                if (mapa[pY][pX] == 'E') {
                    desenha(pX, pY, passos);
                    cout << endl << BRIGHT_GREEN << " MISSION COMPLETE! EQUIPE D ESCAPOU EM " << passos << " PASSOS!" << RESET << endl;
                    break;
                }
            }
        }
    }

    int tx, ty;
    while (pop(pilha, tx, ty));
    return 0;
}