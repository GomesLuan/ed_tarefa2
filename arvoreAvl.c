#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
    // Definições para Windows
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

    int clock_gettime(int dummy, struct timespec *spec) {
        (void)dummy;  // Evita avisos de variável não utilizada

        static LARGE_INTEGER frequency;
        LARGE_INTEGER currentTime;

        if (!QueryPerformanceFrequency(&frequency))
            return -1;

        if (!QueryPerformanceCounter(&currentTime))
            return -1;

        spec->tv_sec = currentTime.QuadPart / frequency.QuadPart;
        spec->tv_nsec = (long)(((currentTime.QuadPart % frequency.QuadPart) * 1e9) / frequency.QuadPart);

        return 0;
    }

    #define CLOCK_MONOTONIC 0
#else
    // Definições para sistemas Unix
    #include <unistd.h>
    #include <errno.h>
#endif

struct tree_node
{
    struct tree_node *parent;
    int value;
    unsigned int height;
    struct tree_node *lchild;
    struct tree_node *rchild;
};

void rd(struct tree_node *node) {
    struct tree_node m = *node;
    node->parent = node->lchild;
    node->lchild = node->lchild->rchild;
    m.lchild->parent = m.parent;
    m.lchild->rchild = node;
}

void re(struct tree_node *node) {
    struct tree_node m = *node;
    node->parent = node->rchild;
    node->rchild = node->rchild->lchild;
    m.rchild->parent = m.parent;
    m.rchild->lchild = node;
}

int cdiff(struct tree_node *node) {
    int lheight, rheight;
    if (node->lchild == NULL) {
        lheight = 0;
    }
    else {
        lheight = node->lchild->height;
    }
    if (node->rchild == NULL) {
        rheight = 0;
    }
    else {
        rheight = node->rchild->height;
    }
    return rheight - lheight;
}

int balance_case(struct tree_node *node) {
    if (cdiff(node) < -1) {
        if (cdiff(node->lchild) <= 0) {
            return 1;
        }
        else {
            return 3;
        }
    }
    if (cdiff(node->rchild) >= 0) {
        return 2;
    }
    return 4;
}

struct tree_node *tree_new(int v) {
    struct tree_node *w;

    w = (struct tree_node*) malloc(sizeof(struct tree_node));
    w->parent = NULL;
    w->value = v;
    w->height = 1;
    w->lchild = NULL;
    w->rchild = NULL;

    return w;
}

void balance(struct tree_node *node) {
    node = node->parent;
    while (node != NULL) {
        if (cdiff(node) > 1 || cdiff(node) < -1) {
            int c = balance_case(node);
            if (c == 1) {
                rd(node);
            }
            else if (c == 2) {
                re(node);
            }
            else if (c == 3) {
                re(node->lchild);
                rd(node);
            }
            else if (c == 4) {
                rd(node->rchild);
                re(node);
            }
        }
        node = node->parent;
    }
}

void insert(struct tree_node **r, struct tree_node *w) {
    if ((*r) == NULL)
    {
        (*r) = w;
        balance(w);
    }
    else
    {
        w->parent = *r;
        if ((*r)->value < w->value)
            insert(&((*r)->rchild), w);
	    else
            insert(&((*r)->lchild), w);
    }
}

struct tree_node *search(struct tree_node *r, int v) {
    if (r != NULL) {
        if (r->value == v) {
            return r;
        }
        if (r->value < v) {
            return search(r->rchild, v);
        }
        return search(r->lchild, v);
    }
    return NULL;
}

void print(struct tree_node *r) {
    if (r != NULL) {
        print(r->lchild);
        printf("%d\n", r->value);
        print(r->rchild);
    }
}

int main(int argc, char **argv) {
    struct timespec start, end;
    unsigned int t;
    unsigned int n = atoi(argv[1]);
    int i;
    struct tree_node *root = NULL;
    srand(time(NULL));
    for (i = 0; i < n; i++) {
        insert(&root, tree_new(rand() % 2000));
    }
    
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    struct tree_node *v = search(root, rand() % 2000);

    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    t = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
    printf("%d\n", t);
    free(v);
    return EXIT_SUCCESS;
}