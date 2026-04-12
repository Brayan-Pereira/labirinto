# 🕹️ Maze Crawler - Projeto 1 (Estrutura de Dados)

Este projeto consiste num jogo de exploração de labirinto em ambiente de consola (ASCII), desenvolvido como critério avaliativo para a disciplina de **Estrutura de Dados** do curso de **Engenharia de Software - UniBrasil**.

O foco principal é a aplicação prática de uma **Pilha Dinâmica** para gerir a movimentação do jogador e a funcionalidade de *backtracking* (desfazer), garantindo a gestão eficiente de memória.

---

## 🚀 Sobre o Jogo

No **Maze Crawler**, o utilizador controla um explorador (`@`) que deve encontrar a saída (`E`) num labirinto gerado por matriz. O desafio é ampliado por um sistema de visibilidade limitada, exigindo estratégia para não se perder nos becos sem saída.

### 🛠️ Especificações Técnicas
* **Linguagem:** C++
* **Estrutura de Dados:** Pilha Dinâmica (LIFO - Last-In, First-Out)
* **Gestão de Memória:** Alocação dinâmica de nós com libertação (`delete`) no desempilhamento para evitar *memory leaks*.
* **Interface:** Consola com suporte a sequências de escape ANSI para cores vibrantes.

---

## 🧠 Lógica da Pilha Dinâmica

A estrutura fundamental do projeto é baseada em **Nós e Ponteiros**, garantindo que o uso de memória seja otimizado:

1. **Push (Empilhar):** Antes de cada movimento válido, a coordenada atual `(x, y)` é armazenada num novo nó no topo da pilha.
2. **Pop (Desempilhar):** Ao acionar o comando de "Undo", a última coordenada é removida do topo, o ponteiro é atualizado e o nó antigo é destruído através do comando `delete`.

---

## ✨ Diferenciais do Projeto (Inovação)

Para elevar a jogabilidade e atender ao critério de originalidade do edital, implementámos as seguintes funcionalidades:

### 1. Sistema de Visão (Fog of War)
O jogador possui uma **Visão Limitada 3x3** que revela apenas as células adjacentes ao explorador. As áreas não visitadas permanecem ocultas por `?` (névoa), simulando uma exploração real de caverna.

### 2. Rastro de Retrocesso Inteligente (Roxo)
Diferente de sistemas que apenas apagam o caminho, o nosso diferencia visualmente a trajetória:
* **Ida:** Marcada com um asterisco azul (`*`).
* **Volta (Undo):** Quando o jogador recua (U), o rastro permanece no mapa para indicar que o local já foi visitado, mas muda para a cor **Roxa**.
* **Lógica:** O símbolo não é alterado, apenas a sua representação cromática, facilitando a análise de rotas descartadas.

### 3. Contador de Passos Progressivo
O contador de passos reflete o **esforço total**. Movimentos de "Undo" não reduzem o contador; cada ação de desfazer soma-se ao total de movimentos, permitindo medir a eficiência do jogador em encontrar a saída.

---

## 🎮 Comandos

| Tecla | Ação |
| :--- | :--- |
| `W` | Mover para Cima |
| `S` | Mover para Baixo |
| `A` | Mover para a Esquerda |
| `D` | Mover para a Direita |
| `U` | Undo (Desfazer movimento) |
| `Q` | Sair do Jogo |

---

## 👥 Estudantes Responsáveis

* **Brayan Pereira**
* **João Gabriel**

**Curso:** Engenharia de Software - UniBrasil  
**Professor(a):** Sâmela S. Sartin  
**Data de Apresentação:** 14/04/2026
