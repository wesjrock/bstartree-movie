#ifndef ARVORE_B_ESTRELA_H
#define ARVORE_B_ESTRELA_H

#include<stdlib.h>
#include<stdio.h>
#define M 4

typedef struct key
{
    int id;
    long byteoffset;
} Key;

struct node{
    int n; /* n < M No. of keys in node will always less than order of B tree */
    Key keys[M-1]; /*array of keys*/
    struct node *p[M];   /* (n+1 pointers will be in use) */
};

extern struct node *root;

enum KeyStatus {
    Duplicate,
    SearchFailure,
    Success,
    InsertIt,
    LessKeys
};

/** Cria a arvore de indices a partir do arquivo de indice **/
void createTree(FILE* idx_file);

void insertBTree(Key key);
void display(struct node *root,int);
void DelNode(Key x);
void getParent(Key key, struct node* parentNode, int* parentPos);
Key* search(int id);
enum KeyStatus ins(struct node *r, Key x, Key* y, struct node** u);
int searchPos(Key x,Key *key_arr, int n);
enum KeyStatus del(struct node *r, Key x);
#endif // ARVORE_B_ESTRELA_H
