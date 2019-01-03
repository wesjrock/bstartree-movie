#include "arvore_b_estrela.h"

struct node *root = NULL;

/** Cria a arvore de indices a partir do arquivo de indice **/
void createTree(FILE* idx_file)
{
    Key key;    // Chave temporaria utilizada para injetar os dados lidos do arquivo, na arvore
    int nBytesLidos = -1; // Flag que acusará que chegamos ao fim do arquivo
    int lendoChavePrimaria = 1; // Informa se estamos lendo o ID (chave primária) ou o byte offset

    rewind(idx_file);

    while(nBytesLidos != 0) // Enquanto tem byte pra ler
    {
        // Se é um ID
        if (lendoChavePrimaria)
        {
            // Lê o id
            nBytesLidos = fread(&key.id,sizeof(int),1,idx_file);

            // Verifica se não tinhamos chegado no final do arquivo
            if (nBytesLidos == 0)
            {
                break;
            }
            // Avisa pro próximo loop que agora será o byteoffset que será lido
            lendoChavePrimaria = 0;
        }
        else // A mesma coisa pro byteoffset, só que dessa vez, a chave é inserida na árvore
        {
            nBytesLidos = fread(&key.byteoffset,sizeof(long),1,idx_file);

            if (nBytesLidos == 0)
            {
                break;
            }

            insertBTree(key); // Insere a chave com a dupla [id|byteoffset] na árvore.
            lendoChavePrimaria = 1;
        }
    }
}

void insertBTree(Key key)
{
    struct node *newnode;
    Key upKey;
    enum KeyStatus value;

    value = ins(root, key, &upKey, &newnode);

    if (value == Duplicate)
        printf("ID repetido: %d!\n", key.id);

    if (value == InsertIt)
    {
        struct node *uproot = root;
        root=(struct node*)malloc(sizeof(struct node));
        root->n = 1;
        root->keys[0] = upKey;
        root->p[0] = uproot;
        root->p[1] = newnode;
    }/*End of if */
}/*End of insert()*/


enum KeyStatus ins(struct node *ptr, Key key, Key *upKey,struct node **newnode)
{
    struct node *newPtr, *lastPtr;
    int pos, i, n,splitPos;
    Key newKey, lastKey;
    enum KeyStatus value;

    if (ptr == NULL)
    {
        *newnode = NULL;
        *upKey = key;
        return InsertIt;
    }
    n = ptr->n;
    pos = searchPos(key, ptr->keys, n);
    if (pos < n && key.id == ptr->keys[pos].id)
        return Duplicate;

    value = ins(ptr->p[pos], key, &newKey, &newPtr);

    if (value != InsertIt)
        return value;
    /*If keys in node is less than M-1 where M is order of B tree*/
    if (n < M - 1)
    {
        pos = searchPos(newKey, ptr->keys, n);
        /*Shifting the key and pointer right for inserting the new key*/
        for (i=n; i>pos; i--)
        {
            ptr->keys[i] = ptr->keys[i-1];
            ptr->p[i+1] = ptr->p[i];
        }
        /*Key is inserted at exact location*/
        ptr->keys[pos] = newKey;
        ptr->p[pos+1] = newPtr;
        ++ptr->n; /*incrementing the number of keys in node*/
        return Success;
    }/*End of if */
    /*If keys in nodes are maximum and position of node to be inserted is last*/
    if (pos == M - 1)
    {
        // Verifica se dá pra redistribuir

        // Pega o pai da página atual
        struct node* parentNode = NULL;
        int parentPos = -1;
        getParent(ptr->keys[0], parentNode, &parentPos);

        // Se a página atual tiver um pai

        lastKey = newKey;
        lastPtr = newPtr;
    }
    else /*If keys in node are maximum and position of node to be inserted is not last*/
    {
        lastKey = ptr->keys[M-2];
        lastPtr = ptr->p[M-1];
        for (i=M-2; i>pos; i--)
        {
            ptr->keys[i] = ptr->keys[i-1];
            ptr->p[i+1] = ptr->p[i];
        }
        ptr->keys[pos] = newKey;
        ptr->p[pos+1] = newPtr;
    }
    splitPos = (M - 1)/2;
       (*upKey) = ptr->keys[splitPos];

    (*newnode)=(struct node*)malloc(sizeof(struct node));/*Right node after split*/
    ptr->n = splitPos; /*No. of keys for left splitted node*/
    (*newnode)->n = M-1-splitPos;/*No. of keys for right splitted node*/
    for (i=0; i < (*newnode)->n; i++)
    {
        (*newnode)->p[i] = ptr->p[i + splitPos + 1];
        if(i < (*newnode)->n - 1)
            (*newnode)->keys[i] = ptr->keys[i + splitPos + 1];
        else
            (*newnode)->keys[i] = lastKey;
    }
    (*newnode)->p[(*newnode)->n] = lastPtr;
    return InsertIt;
}/*End of ins()*/

void display(struct node *ptr, int blanks)
{
    if (ptr)
    {
        int i;
        for(i=1;i<=blanks;i++)
            printf(" ");
        for (i=0; i < ptr->n; i++)
            printf("%d ",ptr->keys[i].id);
        printf("\n");
        for (i=0; i <= ptr->n; i++)
            display(ptr->p[i], blanks+10);
    }/*End of if*/
}/*End of display()*/

Key* search(int id)
{
    int pos, n;
    struct node *ptr = root;
    Key tempKey;
    tempKey.id = id;


    while (ptr)
    {
        n = ptr->n;

        pos = searchPos(tempKey, ptr->keys, n);
        if (pos < n && id == ptr->keys[pos].id)
        {
            return &ptr->keys[pos];
        }

        ptr = ptr->p[pos];
    }
    printf("Registro com ID=%d nao encontrado\n",id);
    return &ptr->keys[pos];
}/*End of search()*/

void getParent(Key key, struct node* parentNode, int* parentPos)
{
    int pos, n;
    struct node *ptr = root;

    while (ptr)
    {
        n = ptr->n;

        pos = searchPos(key, ptr->keys, n);

        if (pos < n && key.id == ptr->keys[pos].id)
        {
            return;
        }
        if(ptr->p[pos] != NULL)
        {
            parentNode = ptr;
            *parentPos = pos;
        }

        ptr = ptr->p[pos];
    }
}

int searchPos(Key key, Key *key_arr, int n)
{
    int pos = 0;
    while (pos < n && key.id > key_arr[pos].id)
        pos++;
    return pos;
}/*End of searchPos()*/

void DelNode(Key key)
{
    struct node *uproot;
    enum KeyStatus value;
    value = del(root,key);
    switch (value)
    {
    case SearchFailure:
        printf("Key %d is not available\n",key.id);
        break;
    case LessKeys:
        uproot = root;
        root = root->p[0];
        free(uproot);
        break;
    default:;
    }/*End of switch*/
}/*End of delnode()*/

enum KeyStatus del(struct node *ptr, Key key)
{
    int pos, i, pivot, n ,min;
    Key *key_arr;
    enum KeyStatus value;
    struct node **p,*lptr,*rptr;

    if (ptr == NULL)
        return SearchFailure;
    /*Assigns values of node*/
    n=ptr->n;
    key_arr = ptr->keys;
    p = ptr->p;
    min = (M - 1)/2;/*Minimum number of keys*/

    pos = searchPos(key, key_arr, n);
    if (p[0] == NULL)
    {
        if (pos == n || key.id < key_arr[pos].id)
            return SearchFailure;
        /*Shift keys and pointers left*/
        for (i=pos+1; i < n; i++)
        {
            key_arr[i-1] = key_arr[i];
            p[i] = p[i+1];
        }
        return  --ptr->n >= (ptr==root ? 1 : min) ? Success : LessKeys;
    }/*End of if */

    if (pos < n && key.id == key_arr[pos].id)
    {
        struct node *qp = p[pos], *qp1;
        int nkey;
        while(1)
        {
            nkey = qp->n;
            qp1 = qp->p[nkey];
            if (qp1 == NULL)
                break;
            qp = qp1;
        }/*End of while*/
        key_arr[pos] = qp->keys[nkey-1];
        qp->keys[nkey - 1] = key;
    }/*End of if */
    value = del(p[pos], key);
    if (value != LessKeys)
        return value;

    if (pos > 0 && p[pos-1]->n > min)
    {
        pivot = pos - 1; /*pivot for left and right node*/
        lptr = p[pivot];
        rptr = p[pos];
        /*Assigns values for right node*/
        rptr->p[rptr->n + 1] = rptr->p[rptr->n];
        for (i=rptr->n; i>0; i--)
        {
            rptr->keys[i] = rptr->keys[i-1];
            rptr->p[i] = rptr->p[i-1];
        }
        rptr->n++;
        rptr->keys[0] = key_arr[pivot];
        rptr->p[0] = lptr->p[lptr->n];
        key_arr[pivot] = lptr->keys[--lptr->n];
        return Success;
    }/*End of if */
    if (pos<n && p[pos+1]->n > min)
    {
        pivot = pos; /*pivot for left and right node*/
        lptr = p[pivot];
        rptr = p[pivot+1];
        /*Assigns values for left node*/
        lptr->keys[lptr->n] = key_arr[pivot];
        lptr->p[lptr->n + 1] = rptr->p[0];
        key_arr[pivot] = rptr->keys[0];
        lptr->n++;
        rptr->n--;
        for (i=0; i < rptr->n; i++)
        {
            rptr->keys[i] = rptr->keys[i+1];
            rptr->p[i] = rptr->p[i+1];
        }/*End of for*/
        rptr->p[rptr->n] = rptr->p[rptr->n + 1];
        return Success;
    }/*End of if */

    if(pos == n)
        pivot = pos-1;
    else
        pivot = pos;

    lptr = p[pivot];
    rptr = p[pivot+1];
    /*merge right node with left node*/
    lptr->keys[lptr->n] = key_arr[pivot];
    lptr->p[lptr->n + 1] = rptr->p[0];
    for (i=0; i < rptr->n; i++)
    {
        lptr->keys[lptr->n + 1 + i] = rptr->keys[i];
        lptr->p[lptr->n + 2 + i] = rptr->p[i+1];
    }
    lptr->n = lptr->n + rptr->n +1;
    free(rptr); /*Remove right node*/
    for (i=pos+1; i < n; i++)
    {
        key_arr[i-1] = key_arr[i];
        p[i] = p[i+1];
    }
    return  --ptr->n >= (ptr == root ? 1 : min) ? Success : LessKeys;
}/*End of del()*/
