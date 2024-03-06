#include <stdio.h>
#include "header.h"

int main(int argc, char const *argv[]) {
if (argc < 2)
    exit(1);

FILE* output_file;
FILE* input_file;
int factor = 0;  // pragul impus

if (strcmp(argv[1], "-c1") == 0) {
    factor = atoi(argv[2]);  // obtinem valoarea factorului
    input_file = fopen(argv[3], "r");
    output_file = fopen(argv[4], "w");
    unsigned int height;  // inaltimea matricei, respectiv latimea
    pixel** MatrixRGB;  // matricea de pixeluri
    // citim matricea de pixeluri din fisierul de input
    MatrixRGB = readMatrix(input_file, &height);
    // formam arborele
    Tquad QuadTree = createTree();
    // determinam nr de frunze; de asemenea functia data completeaza arborele
    // cu noduri, iar cand se determina ca mean < (factor furnizat) se
    // intelege ca nodul nou format e frunza
    int NrFrunze = addNodeTree(MatrixRGB, QuadTree, factor, height, 0, 0);
    // obtinem nr de niveluri in arbore
    int level = getLevel(QuadTree) + 1;
    // obtinem cel mai mare bloc nedivizat
    int maxBlock = biggetsBlock(QuadTree, height);
    fprintf(output_file, "%d\n", level);
    fprintf(output_file, "%d\n", NrFrunze);
    fprintf(output_file, "%d\n", maxBlock);
    // eliberam memoria ocupata de matrice
    freeMatrix(MatrixRGB, height);
    freeTree(QuadTree);

} else if (strcmp(argv[1], "-c2") == 0) {
    factor = atoi(argv[2]);  // obtinem valoarea factorului
    input_file = fopen(argv[3], "r");
    output_file = fopen(argv[4], "wb");
    pixel** MatrixRGB;
    unsigned int height;  // dimensiunea imaginii
    MatrixRGB = readMatrix(input_file, &height);
    // adaugam in fisierul de output dimensiunea
    fwrite(&height, sizeof(height), 1, output_file);
    printf("%d ", height);
    // formam arborele
    Tquad QuadTree = createTree();
    addNodeTree(MatrixRGB, QuadTree, factor, height, 0, 0);
    Queue* que = levelTravel(QuadTree, output_file);
    // eliberam memoria ocupata de matrice, arbore, coada
    destroyQueue(que);
    freeMatrix(MatrixRGB, height);
    freeTree(QuadTree);

} else if (strcmp(argv[1], "-d") == 0) {
    input_file = fopen(argv[2], "r");
    output_file = fopen(argv[3], "w");
} else {
    fprintf(stderr, "ERROR\n");
    exit(3);
}
fclose(output_file);

return 0;
}

// ------------------------ functia de citire din fisier --------------
pixel** readMatrix(FILE* input_file, unsigned int *size) {
char buffer[100];  // buffer
// obtinem valoarea de pe prima linie
fgets(buffer, sizeof(buffer), input_file);
// obtinem voloarea de pe a doua linie
fgets(buffer, sizeof(buffer), input_file);
// obtinem valorile de width and height din buffer si le salvam in variabile
sscanf(buffer, "%d %d", size, size);
fgets(buffer, sizeof(buffer), input_file);
pixel** matrix;
// alocam memorie pentru linii de pointeri la pixeli
matrix = (pixel **)malloc((*size) * sizeof(pixel *));
for (unsigned int i = 0; i < *size; i++) {
    matrix[i] = malloc((*size) * sizeof(pixel));
}
// citim valorile din fisier, introducem in matrice
for (unsigned int i = 0; i < *size; i++) {
    for (unsigned int j = 0; j < *size; j++) {
        fread(&matrix[i][j], sizeof(pixel), 1, input_file);
    }
}
fclose(input_file);

return matrix;
}

// -------------------- functii de calculare a mediei -----------
unsigned long long medColorRed(pixel** matrix, int size, int x, int y) {
    unsigned long long medRed = 0;
    for (int i = x; i < x + size; i++) {
        for (int j = y; j < y + size; j++) {
            medRed += matrix[i][j].red;
        }
    }
    medRed = floor(medRed / (size * size));

    return medRed;
}

unsigned long long medColorGreen(pixel** matrix, int size, int x, int y) {
    unsigned long long medGreen = 0;
    for (int i = x; i < x + size; i++) {
        for (int j = y; j < y + size; j++) {
            medGreen += matrix[i][j].green;
        }
    }
    medGreen = floor(medGreen / (size * size));

    return medGreen;
}

unsigned long long medColorBlue(pixel** matrix, int size, int x, int y) {
    unsigned long long medBlue = 0;
    for (int i = x; i < x + size; i++) {
        for (int j = y; j < y + size; j++) {
            medBlue += matrix[i][j].blue;
        }
    }
    medBlue = floor(medBlue / (size * size));
return medBlue;
}
unsigned long long medColor(pixel** matrix, int size, int x, int y) {
    unsigned long long mean = 0;  // culoarea medie
    unsigned long long medRed = medColorRed(matrix, size, x, y);
    unsigned long long medGreen = medColorGreen(matrix, size, x, y);
    unsigned long long medBlue = medColorBlue(matrix, size, x, y);
    for (int i = x; i < x + size; i++) {
        for (int j = y; j < y + size; j++) {
            mean += ((medRed - matrix[i][j].red) * (medRed - matrix[i][j].red))
            +((medGreen - matrix[i][j].green)*(medGreen - matrix[i][j].green))
            +((medBlue - matrix[i][j].blue) * (medBlue - matrix[i][j].blue));
        }
    }
    mean = floor(mean / (3 * (size * size)));

    return mean;
}

// --------------- functia de dealocare a memoriei ---------
void freeMatrix(pixel** matrix, int height) {
    for (int i = 0; i < height; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

Tquad createTree() {
    Tquad root = malloc(sizeof(Tnode));
    root->colors = NULL;
    root->leftUp = NULL;
    root->rightUp = NULL;
    root->rightDown = NULL;
    root->leftDown = NULL;

    return root;
}

int addNodeTree(pixel** matrix, Tquad tree,
    int factor, int size, int x, int y) {
    int nrFrunze = 0;  // numarul de frunze
    int mean;  // media culorilor
    int red = 0, green = 0, blue = 0;
    red = medColorRed(matrix, size, x , y);
    green = medColorGreen(matrix, size, x , y);
    blue = medColorBlue(matrix, size, x , y);
    mean = medColor(matrix, size, x, y);
    if (mean > factor) {
        tree->leftUp = createTree();
        tree->rightUp = createTree();
        tree->rightDown = createTree();
        tree->leftDown = createTree();
    nrFrunze += addNodeTree(matrix, tree->leftUp, factor, size / 2, x, y);
    nrFrunze += addNodeTree(matrix, tree->rightUp,
        factor, size / 2, x, y + size / 2);
    nrFrunze += addNodeTree(matrix, tree->rightDown, factor,
        size / 2, x + size / 2, y + size / 2);
    nrFrunze += addNodeTree(matrix, tree->leftDown, factor,
        size / 2, x + size / 2, y);
    } else {
        // atata timp cat se intra in else, inseamna ca nodul e o frunza
        nrFrunze += 1;
        tree->colors = malloc(sizeof(pixel));
        tree->colors->red = red;
        tree->colors->green = green;
        tree->colors->blue = blue;
    }

    return nrFrunze;
}

int getLevel(Tquad tree) {
    if (tree == NULL || (tree->leftUp == NULL && tree->rightUp == NULL &&
        tree->rightDown == NULL && tree->leftDown == NULL)) {
        return 0;
    }
    int first = 0, second = 0, third = 0, fourth = 0;  // pentru fiecare copil
    first = getLevel(tree->leftUp);
    second = getLevel(tree->rightUp);
    third = getLevel(tree->rightDown);
    fourth = getLevel(tree->leftDown);
    int max = first;
    if (max < second) {
        max = second;
    }
    if (max < third) {
        max = third;
    }
    if (max < fourth) {
        max = fourth;
    }

    return max + 1;
}

int biggetsBlock(Tquad tree, int size) {
    if (tree == NULL || (tree->leftUp == NULL && tree->rightUp == NULL &&
        tree->rightDown == NULL && tree->leftDown == NULL)) {
        return size;
    }
    int sizeFirst = 0, sizeSecond = 0, sizeThird = 0, sizeFourth = 0;
    sizeFirst = biggetsBlock(tree->leftUp, size / 2);
    sizeSecond = biggetsBlock(tree->rightUp, size / 2);
    sizeThird = biggetsBlock(tree->rightDown, size / 2);
    sizeFourth = biggetsBlock(tree->leftDown, size / 2);
    int maxBlock = sizeFirst;
    if (maxBlock < sizeSecond) {
        maxBlock = sizeSecond;
    } else if (maxBlock < sizeThird) {
        maxBlock = sizeThird;
    } else if (maxBlock < sizeFourth) {
        maxBlock = sizeFourth;
    }

    return maxBlock;
}

void freeTree(Tquad tree) {
    if (tree == NULL) {  // daca nodul curent e null
        return;
    }
    // verificam daca nodul curent are copii
    if (tree->leftUp == NULL && tree->rightUp == NULL &&
        tree->rightDown == NULL && tree->leftDown == NULL) {
        // nu are copii, verificam daca are adresa pentru culori
        if (tree->colors != NULL) {
            free(tree->colors);
        }
        free(tree);
    // daca are copii, ii parcurgem si apelam functia recursiv
    } else {
        if (tree->leftUp != NULL) {
            freeTree(tree->leftUp);
        }
        if (tree->rightUp != NULL) {
            freeTree(tree->rightUp);
        }
        if (tree->rightDown != NULL) {
            freeTree(tree->rightDown);
        }
        if (tree->leftDown != NULL) {
            freeTree(tree->leftDown);
        }
        free(tree);
    }
}
// ---------------- Coada ---------------//

Qnode* createQnode(Tquad data) {  // functia de creare a unui nou nod in coada
    Qnode* newQnode = malloc(sizeof(Qnode));
    newQnode->next = NULL;
    newQnode->data = data;

    return newQnode;
}

Queue* createQueue() {  // functia de creare a cozii
    Queue* newQ = malloc(sizeof(Queue));
    newQ->front = NULL;
    newQ->rear = NULL;

    return newQ;
}

int queEmpty(Queue* q) {  // functia de verificare daca coada e vida
    if (q->front == NULL || q == NULL)
        return 1;
    return 0;
}

// functia de introducere a noului element in coada
Queue* equeue(Queue* q, Tquad data) {
    Qnode* newNode = createQnode(data);
	if (q == NULL)
        return NULL;

    if (queEmpty(q)) {
        q->front = newNode;
        q->rear = newNode;
        return q;
    }

    q->rear->next = newNode;
    q->rear = newNode;

    return q;
}

Tquad dequeue(Queue* q) {  // functia de extragere a elementului din coada
    if (queEmpty(q))
        return NULL;
    Tquad result = q->front->data;
    Qnode* aux = q->front;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(aux);

    return result;
}
// functia de afisare a primului element din coada
Tquad front(Queue* q) {
    if (!queEmpty(q)) {
        return q->front->data;
    } else
        exit(1);
}
// functia de dealocare a memoriei ocupate de coada
void destroyQueue(Queue* q) {
    while (!queEmpty(q)) {
        dequeue(q);
    }
    free(q);
}

Queue* levelTravel(Tquad tree, FILE* output) {
    if (tree == NULL)
        return NULL;
    unsigned char intern = 0;
    unsigned char frunza = 1;
    // valorile pentru rosu, verde, albastru pentru pixelii din nodul respectiv
    unsigned char valR = 0, valG = 0, valB = 0;
    Queue* q = createQueue();
    Tquad current;
    equeue(q, tree);
    while (!queEmpty(q)) {
        current = dequeue(q);
        if (current != NULL) {
            if (current->leftUp != NULL) {
                equeue(q, current->leftUp);
            }
            if (current->rightUp != NULL) {
                equeue(q, current->rightUp);
            }
            if (current->rightDown != NULL) {
                equeue(q, current->rightDown);
            }
            if (current->leftDown != NULL) {
                equeue(q, current->leftDown);
                fwrite(&intern, sizeof(intern), 1, output);
                printf("%d ", intern);
            } else {
                valR = current->colors->red;
                valG = current->colors->green;
                valB = current->colors->blue;
                fwrite(&frunza, sizeof(frunza), 1, output);
                printf("%d ", frunza);
                fwrite(&valR, sizeof(unsigned char), 1, output);
                printf("%d ", valR);
                fwrite(&valG, sizeof(unsigned char), 1, output);
                printf("%d ", valG);
                fwrite(&valB, sizeof(unsigned char), 1, output);
                printf("%d ", valB);
            }
        }
    }

return q;
}
