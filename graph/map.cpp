
#include <bits/stdc++.h>
using namespace std;

vector<int> djikstra(int node, int n, vector<vector<int>> adj[])
{
    vector<int> edgeweights(n, INT_MAX);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    edgeweights[node] = 0;
    pq.push({0, node});
    while (!pq.empty())
    {
        int node1 = pq.top().second;
        int weight1 = pq.top().first;
        pq.pop();

        for (auto it : adj[node1])
        {
            int adjnode = it[0];
            int adjweight = it[1];
            int totalweight = adjweight + weight1;
            if (totalweight < edgeweights[adjnode])
            {
                pq.push({totalweight, adjnode});
                edgeweights[adjnode] = totalweight;
            }
        }
    }

    return edgeweights;
}
int main()
{
    int n = 5;
    vector<vector<int>> adjmat = {{0, 2, 5, 0, 0}, {2, 0, 0, 3, 0}, {5, 0, 0, 4, 0}, {0, 3, 4, 0, 1}, {0, 0, 0, 1, 0}};
    vector<vector<int>> adj[n];
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (adjmat[i][j])
                adj[i].push_back({j, adjmat[i][j]});
        }
    }

    int node1;
    int node2;

    cin >> node1 >> node2;
    vector<int> shortestpath = djikstra(node1, n, adj);
    cout << shortestpath[node2];
}
