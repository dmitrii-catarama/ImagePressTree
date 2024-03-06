#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef char D;

typedef struct Pixel {
    unsigned char red;  // rosu
    unsigned char green;  // verde
    unsigned char blue;  // albastru
} pixel;

typedef struct Tnode {
    pixel* colors;  // rgb culori
    struct Tnode* leftUp;  // 1
    struct Tnode* rightUp;  // 2
    struct Tnode* rightDown;  // 3
    struct Tnode* leftDown;  // 4
} Tnode, *Tquad;
//--------- structuri pentru Coada ---------
typedef struct qnode {  // structura nodului din coada
    Tquad data;
    struct qnode* next;
} Qnode;

typedef struct queue {  // structura cozii
    Qnode *front;
    Qnode *rear;
} Queue;

Qnode* createQnode(Tquad data);
Queue* createQueue();
int queEmpty(Queue* q);
Queue* equeue(Queue* q, Tquad data);
Tquad dequeue(Queue* q);
Tquad front(Queue* q);
void destroyQueue(Queue* q);

// functia de initializare a arborelui
Tquad createTree();
// functia de citire din fisier
pixel** readMatrix(FILE *input_file, unsigned int *size);
// functii pentru calcularea valorii medii a culorilor unui bloc
unsigned long long medColorRed(pixel** matrix, int size, int x, int y);
unsigned long long medColorGreen(pixel** matrix, int size, int x, int y);
unsigned long long medColorBlue(pixel** matrix, int size, int x, int y);
unsigned long long medColor(pixel** matrix, int size, int x, int y);
// functia ce determina nivelul arborelui
int getLevel(Tquad tree);
// functia ce determina cel mai mare bloc din arbore si intoarce valoarea
// size-ului
int biggetsBlock(Tquad tree, int size);
// functia ce adauga noduri in arbore si poate reintaorce nr de frunze
// din arbore
int addNodeTree(pixel** matrix, Tquad tree, int factor, int size, int x, int y);
// functia pentru dealocarea memoriei pentru matrice
void freeMatrix(pixel** matrix, int height);
// functia de dealocare a memoriei pentru arbore
void freeTree(Tquad tree);
Queue* levelTravel(Tquad tree, FILE* output);
