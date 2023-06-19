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
    int value;
    struct tree_node *lchild;
    struct tree_node *rchild;
};

struct tree_node *tree_new(int v) {
    struct tree_node *w;

    w = (struct tree_node*) malloc(sizeof(struct tree_node));
    w->value = v;
    w->lchild = NULL;
    w->rchild = NULL;

    return w;
}

void insert(struct tree_node **r, struct tree_node *w) {
    if ((*r) == NULL)
    {
        (*r) = w;
    }
    else
    {
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