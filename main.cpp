#include <iostream>
#include <vector>
#include <string>

/* * TRATAMENTO DE PORTABILIDADE (WINDOWS/LINUX)
 * Define macros e bibliotecas específicas para garantir que o controle de console
 * e a leitura de teclado funcionem em diferentes sistemas operacionais.
 */
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
    
    // Simulação da conio.h para sistemas Unix-like
    char getch_linux()
    {
        char buf = 0;
        struct termios old = {0};
        tcgetattr(0, &old);
        old.c_lflag &= ~ICANON; // Desativa modo canônico (buffer de linha)
        old.c_lflag &= ~ECHO;   // Desativa eco de caracteres no terminal
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        tcsetattr(0, TCSANOW, &old);
        read(0, &buf, 1);
        tcsetattr(0, TCSANOW, &old);
        return buf;
    }
#endif

using namespace std;

// DEFINIÇÃO DE CONSTANTES DE CORES (Sequências de Escape ANSI)
#define RESET         "\033[0m"
#define BRIGHT_RED    "\033[91;1m"
#define BRIGHT_GREEN  "\033[92;1m"
#define BRIGHT_YELLOW "\033[93;1m"
#define BRIGHT_BLUE   "\033[94;1m"
#define PURPLE        "\033[35;1m" // Cor para o rastro de retorno (Undo)
#define BRIGHT_CYAN   "\033[96;1m"
#define GRAY          "\033[90m"

// DIMENSÕES DO AMBIENTE
const int LINHAS = 30;
const int COLUNAS = 40;

// Matriz booleana para controle do Campo de Visão (Fog of War)
bool visivel[LINHAS][COLUNAS] = {false};

/* * ESTRUTURA DE DADOS: NÓ DA PILHA
 * Utilizada para implementar a Pilha Dinâmica que armazena o histórico de movimentos.
 */
struct No
{
    int x, y;
    No *proximo;
};

// REPRESENTAÇÃO DO MAPA EM MATRIZ DE STRINGS
vector<string> mapa = {
    "########################################",
    "#@#       #           #               #",
    "# # ##### # ######### # ############# #",
    "# # #     # #         # #           # #",
    "# # # ##### # ######### # ######### # #",
    "#   # #     # #         # #       # # #",
    "## ## # ##### # ######### # ##### # # #",
    "#     # #     #           # #   # # # #",
    "# ####### ##### ########### # # # # # #",
    "# #     #       #           # # # # # #",
    "# # ### ######### ########### # # # # #",
    "# # # #           #           # #   # #",
    "# # # ############# ########### ##### #",
    "#   #               #           #     #",
    "##### ############### ########### #####",
    "#     #               #               #",
    "# ##### ############### ############# #",
    "# #     #               #           # #",
    "# # ##### ############### ######### # #",
    "# # #     #               #       # # #",
    "# # # ##### ############### ##### # # #",
    "#   # #     #               #   # # # #",
    "##### # ##### ############### # # # # #",
    "#     # #     #               # # # # #",
    "# ####### ##### ############### # # # #",
    "# #             #               # # # #",
    "# # ############# ############### # # #",
    "#                 #                 #E#",
    "########################################",
    "                                        "
};

/**
 * Atualiza a matriz de visibilidade em um raio 3x3 ao redor do jogador.
 * @param pX Coordenada X atual
 * @param pY Coordenada Y atual
 */
void atualizaVisao(int pX, int pY)
{
    for (int i = pY - 1; i <= pY + 1; i++)
    {
        for (int j = pX - 1; j <= pX + 1; j++)
        {
            if (i >= 0 && i < LINHAS && j >= 0 && j < COLUNAS)
            {
                visivel[i][j] = true;
            }
        }
    }
}

/**
 * Insere uma nova coordenada no topo da pilha (LIFO).
 * Aloca memória dinamicamente para o novo nó.
 */
void push(No *&topo, int x, int y)
{
    No *novo = new No;
    novo->x = x;
    novo->y = y;
    novo->proximo = topo;
    topo = novo;
}

/**
 * Remove e retorna a coordenada do topo da pilha.
 * Realiza a liberação de memória (delete) para evitar memory leaks.
 * @return idtrue se a operação foi bem-suceda, false se a pilha estiver vazia.
 */
bool pop(No *&topo, int &x, int &y)
{
    if (topo == nullptr)
        return false;
    No *temp = topo;
    x = topo->x;
    y = topo->y;
    topo = topo->proximo;
    delete temp;
    return true;
}

/**
 * Renderiza o estado atual do jogo no terminal.
 * Aplica lógica de cores e máscara de visibilidade (Fog of War).
 */
void desenha(int pX, int pY, int passos)
{
    system(CLEAR); // Limpa o terminal para a nova renderização
    cout << BRIGHT_CYAN << "========================================" << RESET << endl;
    cout << PURPLE      << "   MAZE CRAWLER - UniBrasil - EQUIPE D" << RESET << endl;
    cout << BRIGHT_CYAN << "========================================" << RESET << endl;
    cout << " Passos Totais: " << BRIGHT_YELLOW << passos << RESET << " | Visao: " << BRIGHT_GREEN << "3x3 ON" << RESET << endl << endl;

    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
        {
            if (i == pY && j == pX)
                cout << BRIGHT_YELLOW << "@" << RESET; // Jogador
            else if (!visivel[i][j])
                cout << GRAY << "?" << RESET;          // Área não explorada
            else if (mapa[i][j] == '#')
                cout << BRIGHT_RED << "#" << RESET;    // Parede
            else if (mapa[i][j] == 'E')
                cout << BRIGHT_GREEN << "E" << RESET;  // Saída
            else if (mapa[i][j] == '*')
                cout << BRIGHT_BLUE << "*" << RESET;   // Caminho percorrido (Ida)
            else if (mapa[i][j] == 'R')
                cout << PURPLE << "*" << RESET;        // Caminho revertido (Undo)
            else
                cout << " ";                           // Espaço vazio visível
        }
        cout << endl;
    }
    cout << endl << BRIGHT_CYAN << " CONTROLES: [W,A,S,D] | [U] Undo | [Q] Sair" << RESET << endl;
}

int main()
{
    /* Habilita processamento de sequências ANSI no terminal Windows */
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &dwMode))
    {
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif

    // Inicialização do estado do jogo
    No *pilha = nullptr; // Ponteiro topo da pilha
    int pX = 1, pY = 1;  // Coordenadas iniciais
    int passos = 0;

    atualizaVisao(pX, pY);

    // Game Loop Principal
    while (true)
    {
        desenha(pX, pY, passos);

        char tecla;
#ifdef _WIN32
        tecla = tolower(_getch());
#else
        tecla = tolower(getch_linux());
#endif

        if (tecla == 'q') // Encerramento manual
            break;

        if (tecla == 'u') // Lógica de Desfazer (Undo)
        {
            int atualX = pX;
            int atualY = pY;
            if (pop(pilha, pX, pY))
            {
                if (mapa[atualY][atualX] != 'E')
                {
                    mapa[atualY][atualX] = 'R'; // Marca a casa abandonada pelo undo
                }
                if (passos > 0)
                {
                    passos--;
                }
                atualizaVisao(pX, pY);
            }
        }
        else // Lógica de Movimentação
        {
            int nX = pX, nY = pY;
            if (tecla == 'w') nY--;
            else if (tecla == 's') nY++;
            else if (tecla == 'a') nX--;
            else if (tecla == 'd') nX++;
            else continue; // Tecla inválida

            // Verificação de colisão e limites do mapa
            if (nY >= 0 && nY < LINHAS && nX >= 0 && nX < COLUNAS && mapa[nY][nX] != '#')
            {
                push(pilha, pX, pY); // Salva posição atual antes de mover
                mapa[pY][pX] = '*';  // Marca rastro de ida
                pX = nX;
                pY = nY;
                passos++;
                atualizaVisao(pX, pY);

                // Verificação de Condição de Vitória
                if (mapa[pY][pX] == 'E')
                {
                    desenha(pX, pY, passos);
                    cout << endl << BRIGHT_GREEN 
                         << " MISSION COMPLETE! EQUIPE D ESCAPOU EM " 
                         << passos << " PASSOS!" << RESET << endl;

                    cout << endl << BRIGHT_CYAN << " Desenvolvido por:" << RESET << endl;
                    cout << " - Brayan Pereira RA: 2023100832" << endl;
                    cout << " - Joao Gabriel RA: 2023100603" << endl;
                    break;
                }
            }
        }
    }

    /* * LIMPEZA FINAL DE MEMÓRIA
     * Esvazia a pilha para liberar todos os nós alocados dinamicamente antes de fechar.
     */
    int tx, ty;
    while (pop(pilha, tx, ty));
    
    return 0;
}