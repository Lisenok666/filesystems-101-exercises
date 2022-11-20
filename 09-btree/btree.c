#include <solution.h>
#include <stdlib.h>
#include <stdio.h>

struct BNode{
    int leaf, count, t  ;
    int * values;
    struct BNode ** links;
};

struct btree
{
    struct BNode* root;
    int  t;
};

static struct BNode* node_alloc(unsigned int t, int leaf)
{
    struct BNode *temp = (struct BNode *)malloc(sizeof(struct BNode));
    temp->t = t;
    temp->links = (struct BNode **)malloc((2 *t +1) * sizeof(struct BNode *));
    temp->count = 0;
    temp->leaf = leaf;
    temp->values = (int *)malloc((2*t ) * sizeof(int));
    printf("Succsess struct node_alloc \n");
    return temp;
}

struct btree* btree_alloc(unsigned int L)
{
    struct btree *newTree = (struct btree *)malloc(sizeof(struct btree));
    newTree->root = node_alloc(L, 1);
    newTree->t = L;
    printf("Succsess struct btree* btree_alloc \n");
	return newTree;
}

void node_free(struct BNode *t)
{
    int L = 2*t->t;
    int i = 0;
    while(i < L)
    {
        if(t->links[i] != NULL)
            free(t->links[i]);
    }
    free(t->links);
    free(t->values);
    free(t);
}


void btree_free(struct btree *t)
{
	node_free(t->root);
	free(t);
}

void btree_split_child(struct BNode *x, int i, struct BNode *y)
{
    int t = x->t;
    struct BNode *z = node_alloc(t, 0);
    z->leaf = y->leaf;
    z->count = t - 1;
    for(int j = 0; j < t- 1; j++)
        (z->values)[j]  =  (y->values)[j + t];
    if(!y->leaf)
    {
        for(int j = 0; j < t- 1; j++)
            (z->links)[j] = (y->links)[j + t];
    }
    y->count = t - 1;
    for(int j = x->count + 1; j >= i + 1; j--)
        (x->links)[j + 1]  =  (y->links)[j + t];
    (x->links)[i + 1] = z;
    for(int j = x->count - 1; j >= i; j--)
        (x->values)[j + 1]  =  (x->values)[j];
    (x->values)[i]  =  (y->values)[t - 1];
    x->count++;
    return;

}

void btree_insert_nonfull(struct BNode *x, int k){
    int i =  x->count;
    int t = x->t;
    if(x->leaf)
    {
        while(i >= 0 && k < (x->values)[i]){
            (x->values)[i + 1] = (x->values)[i];
            i--;}
        (x->values)[i + 1] = k;
        x->count++;
        return;
    }
    while(i >= 0 && k < (x->values)[i]){
        i--;
    }
    i++;
    struct BNode* temp = (x->links)[i];
    if(temp->count == 2*t - 1)
    {
        btree_split_child(x, i, temp);
        if(k > (x->values)[i])
            i++;
    }
    btree_insert_nonfull(temp, k);
}


void btree_insert(struct btree *t, int k)
{
    struct BNode *r = t->root;
    if(r->count == 2*r->t - 1){
        struct BNode *s = node_alloc(r->t, 0);
        t->root = s;
        s->links[0] = r;
        btree_split_child(s, 0, r);
        btree_insert_nonfull(s, k);
        return;
    }
    btree_insert_nonfull(r, k);
    return;

}

void btree_delete(struct btree *t, int x)
{
	(void) t;
	(void) x;
}

bool node_contain(struct BNode *r, int k)
{
    int i = 0;
    while(i <= r->count && k > (r->values)[i])
        i++;
    if(i < r->count &&  k == (r->values)[i])
        return true;
    if(r->leaf)
        return false;
    else
        return node_contain((r->links)[i], k);
}

bool btree_contains(struct btree *t, int k)
{
    struct BNode *r = t->root;
	int i = 0;
	while(i <= r->count && k > (r->values)[i])
	    i++;
	if(i < r->count &&  k == (r->values)[i])
	    return true;
	if(r->leaf)
	    return false;
	else
        return node_contain((r->links)[i], k);
}

struct btree_iter
{
};

struct btree_iter* btree_iter_start(struct btree *t)
{
	(void) t;

	return NULL;
}

void btree_iter_end(struct btree_iter *i)
{
	(void) i;
}

bool btree_iter_next(struct btree_iter *i, int *x)
{
	(void) i;
	(void) x;

	return false;
}
