#include <stdio.h>
#include <vector>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <numeric>
#include <random>
#include <cmath>

/*

edges[0] = {{2, 1}, {3, 2}, {3, 3}};
edges[1] = {{2, 0}, {4, 2}, {3, 4}};
edges[2] = {{3, 0}, {4, 1}, {5, 3}, {1, 4}, {6, 5}};
edges[3] = {{3, 0}, {5, 2}, {7, 5}};
edges[4] = {{3, 1}, {1, 2}, {8, 5}};
edges[5] = {{6, 2}, {7, 3}, {8, 4}, {9, 6}};
edges[6] = {{9, 5}};

*/

double total_weight = 0.0;
double total_weight_2 = 0.0;
std::mutex m;
std::default_random_engine generator;

template <typename T>
class Heap
{
private:
    std::vector<T> nodes;

    // Returns index of parent of node at index `i`
    inline int parent(int i)
    {
        return (i - 1) / 2;
    }

    // Returns index of left child of node at index `i`
    inline int left(int i)
    {
        return 2 * i + 1;
    }

    // Returns index of right child of node at index `i`
    inline int right(int i)
    {
        return 2 * i + 2;
    }

    // Swaps nodes at indices `a` and `b`
    inline void swap(int a, int b)
    {
        T temp = nodes[a];
        nodes[a] = nodes[b];
        nodes[b] = temp;
    }

public:
    T pop()
    {
        // Handle empty heap
        if (nodes.size() <= 0)
        {
            throw std::runtime_error("Cannot pop from empty heap");
        }

        // Pop root and replace with last element in heap
        T root = nodes[0];
        nodes[0] = nodes.back();
        nodes.pop_back();

        // Maintain heap invariant
        int i = 0, l = left(i), r = right(i);
        while ((l < nodes.size() && nodes[l] < nodes[i]) ||
               (r < nodes.size() && nodes[r] < nodes[i]))
        {
            int smallest = (r < nodes.size() && nodes[r] < nodes[l]) ? r : l;
            swap(i, smallest);
            i = smallest, l = left(i), r = right(i);
        }

        return root;
    }

    void insert(T node)
    {
        // Insert node
        nodes.push_back(node);

        // Maintain heap invariant
        int i = nodes.size() - 1;
        while (i && node < nodes[parent(i)])
        {
            swap(i, parent(i));
            i = parent(i);
        }
    }

    int size()
    {
        return nodes.size();
    }
};

struct Edge
{
    int to;        // Name of outgoing vertex
    double weight; // Weight of edge

    // Compare based on `weight`
    bool operator<(const Edge &e)
    {
        return this->weight < e.weight;
    }
};

struct Vertex
{
    double w, x, y, z;
    double dist;
};

class Graph
{
public:
    int n;
    std::vector<std::vector<Edge>> edges;

    Graph(int n, int dim)
    {
        this->n = n;
        edges = std::vector<std::vector<Edge>>(n);
        std::uniform_real_distribution<double> unif(0.0, 1.0);

        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < i; ++j)
            {
                double weight = unif(generator);
                // if (weight > 0.3)
                // {
                //     continue;
                // }
                edges[i].push_back({j, weight});
                edges[j].push_back({i, weight});
            }
        }
    }

    double mst_weight()
    {
        return this->n;
    }
};

double prims(Graph g)
{
    std::unordered_set<int> visited;
    Heap<Edge> h;
    h.insert({0, 0.0});
    std::vector<double> dist(g.n, INT_MAX);
    dist[0] = 0.0;

    while (h.size() > 0)
    {
        Edge v = h.pop();
        visited.insert(v.to);
        for (Edge &w : g.edges[v.to])
        {
            if (visited.find(w.to) == visited.end() && dist[w.to] > w.weight)
            {
                dist[w.to] = w.weight;
                h.insert({w.to, w.weight});
            }
        }
    }

    auto x = std::max_element(dist.begin(), dist.end());
    printf("%f\n", *x);

    return std::accumulate(dist.begin(), dist.end(), 0.0);
}

Graph prune(Graph g)
{
    std::vector<std::vector<Edge>> edges(g.n);
    for (int i = 0; i < g.n; ++i)
    {
        for (Edge e : g.edges[i])
        {
            if (e.weight <= 0.001255)
            {
                edges[i].push_back(e);
            }
        }
    }
    g.edges = edges;
    return g;
}

void simulate(int n, int dim)
{
    Graph g(n, dim);
    Graph g1 = prune(g);
    m.lock();
    total_weight += prims(g);
    total_weight_2 += prims(g1);
    m.unlock();
}

int main(int argc, char *argv[])
{
    // Check for valid command line arguments
    if (argc != 5)
    {
        printf("Invalid arguments\n");
        return 1;
    }

    // Parse command line arguments
    int flag = atoi(argv[1]), n = atoi(argv[2]), trials = atoi(argv[3]), dim = atoi(argv[4]);

    // Set generator seed
    generator.seed(time(NULL));

    std::vector<std::thread> threads;
    for (int i = 0; i < trials; ++i)
    {
        threads.push_back(std::thread(simulate, n, dim));
    }
    for (std::thread &t : threads)
    {
        t.join();
    }

    // Print results
    printf("%f %d %d %d\n", total_weight / trials, n, trials, dim);
    printf("%f %d %d %d\n", total_weight_2 / trials, n, trials, dim);
    return 0;
}