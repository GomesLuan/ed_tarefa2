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

struct list_node {
    int value;
    struct list_node *next;
};

struct hash {
    struct list_node **t;
    unsigned int m;
    unsigned int n;
};

struct list_node *list_new(int v);
struct hash *hash_new(int m);
void list_insert(struct list_node **l, struct list_node *w);
void hash_insert(struct hash *h, struct list_node *w);
void rehashing(struct hash *h, struct list_node *w);

struct list_node *list_new(int v) {
    struct list_node *w;

    w = (struct list_node*) malloc(sizeof(struct list_node));
    w->value = v;
    w->next = NULL;

    return w;
}

struct hash *hash_new(int m) {
    struct hash *w;

    w = (struct hash*) malloc(sizeof(struct hash));
    w->t = (struct list_node**) malloc(m*sizeof(struct list_node*));
    for (int i=0;i<m;i++) {
        w->t[i] = NULL;
    }
    w->m = m;
    w->n = 0;

    return w;
}

void list_insert(struct list_node **l, struct list_node *w) {
    if (*l == NULL) {
        *l = w;
    }
    else {
        list_insert(&((*l)->next), w);
    }
}

void hash_insert(struct hash *h, struct list_node *w) {
    if (h->m > h->n) {
        list_insert(&(h->t[w->value % h->m]), w);
        h->n++;
    }
    else {
        struct hash *new_h = hash_new(h->m*2);
        for (int i=0;i<h->m;i++) {
            if (h->t[i] != NULL) {
                rehashing(new_h, h->t[i]);
            }
        }
        *h = *new_h;
        free(new_h);
        hash_insert(h, w);
    }
}

void rehashing(struct hash *h, struct list_node *w) {
    if (w->next != NULL) {
        rehashing(h, w->next);
    }
    struct list_node *current = list_new(w->value);
    hash_insert(h, current);
    free(w);
}

int main(int argc, char **argv) {
    struct timespec start, end;
    unsigned int t;
    unsigned int n = atoi(argv[1]);
    struct hash *h = hash_new(4);
    int i;
    srand(time(NULL));
    for (i = 0; i < n; i++) {
        hash_insert(h, list_new(rand() % 2000));
    }
    
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    t = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
    printf("%d\n", t);
    return EXIT_SUCCESS;
}