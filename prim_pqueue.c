#include "priorityq_heap.c"
 
#define SIZE 8
#define INF 999
 
void prim(int graph[SIZE][SIZE], int pred[SIZE])
{
    int key[SIZE];
    pq_ptr pq; // priority queue
    int u;     // vertex explored
    int v;     // neighbors of u
    int i;
    int w;
 
    // initializing key and predecesor arrays 
    for (i = 0; i < SIZE; i++)
    {
       key[i] = INF;
       pred[i] = -1;
    }
    key[0] = 0;
    
    // initializing priority queue
    pq = pq_new(SIZE);
    for (i = 0; i< SIZE; i++)
        pq_push(pq, i, key[i]); // i (vertex number) is the data, key[i] is the priority
 
    while(pq->n > 0)
    {
        u = pq_pop(pq);
        printf("Exploring neighbors of %d\n", u);
        for (v = 0; v< SIZE; v++)
        {
            if (graph[u][v] != INF) //if i is neighbor of u
            {
                if (pq_haskey(pq, v))
                {
                    if (graph[u][v] < key[v])
                    {
                        printf("Neighbor %d found shorter path through %d with weight %d\n", v, u, graph[u][v]);
                        pred[v] = u;
                        key[v] = graph[u][v];
                        pq_changepriority(pq, v, graph[u][v]);
                    }
                }
            }
        }
    }
    printf("Minimum spanning tree calculated.\n");
}
