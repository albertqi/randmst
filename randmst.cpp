#include <stdio.h>
#include <vector>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <numeric>
#include <random>
#include <math.h>
#include <chrono>

double total_weight = 0.0;
std::mutex m;
thread_local std::mt19937 generator;

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

struct Vertex
{
    std::vector<double> coords; // Coordinates of vertex
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

inline double dist(Vertex a, Vertex b)
{
    return sqrt((a.coords[0] - b.coords[0]) * (a.coords[0] - b.coords[0]) +
                (a.coords[1] - b.coords[1]) * (a.coords[1] - b.coords[1]) +
                (a.coords[2] - b.coords[2]) * (a.coords[2] - b.coords[2]) +
                (a.coords[3] - b.coords[3]) * (a.coords[3] - b.coords[3]));
}

inline double pruning_threshold(int n, int dim)
{
    return dim ? 1.5 * pow(log2(n) / (M_PI * n), 1.0 / dim) : 1.5 * (3.0 / pow(n, 0.864));
}

class Graph
{
public:
    int n;
    std::vector<std::vector<Edge>> edges;

    Graph(int flag, int n, int dim)
    {
        this->n = n;
        edges = std::vector<std::vector<Edge>>(n);

        std::random_device random_dev;
        generator.seed(random_dev());
        std::uniform_real_distribution<double> unif(0.0, 1.0);

        std::vector<Vertex> vertices;
        if (dim)
        {
            for (int i = 0; i < n; ++i)
            {
                std::vector<double> v(4, 0.0);
                for (int j = 0; j < std::min(4, dim); ++j)
                {
                    v[j] = unif(generator);
                }
                vertices.push_back({v});
            }
        }

        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < i; ++j)
            {
                double weight = dim ? dist(vertices[i], vertices[j]) : unif(generator);
                if (!flag && weight > pruning_threshold(n, dim))
                {
                    continue;
                }
                edges[i].push_back({j, weight});
                edges[j].push_back({i, weight});
            }
        }
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

    return std::accumulate(dist.begin(), dist.end(), 0.0);
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

    std::vector<std::thread> threads;
    for (int i = 0; i < trials; ++i)
    {
        threads.push_back(std::thread([&flag, &n, &dim]()
        {
            Graph g(flag, n, dim);
            double mst_weight = prims(g);
            m.lock();
            total_weight += mst_weight;
            m.unlock();
        }));
    }
    for (std::thread &t : threads)
    {
        t.join();
    }

    // Print results
    printf("%f %d %d %d\n", total_weight / trials, n, trials, dim);
    return 0;
}