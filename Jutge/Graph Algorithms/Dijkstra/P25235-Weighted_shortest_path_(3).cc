#include <iostream>
#include <vector>
#include <queue>
using namespace std;


const int INF = 1e9;

typedef pair<int, int> P;
typedef vector<P> VP;
typedef vector<VP> VVP;
typedef vector<int> VI;


void dijkstra(const VVP& G, int n, int ini, int fi) {

    priority_queue<P> Q;
    VI dist(n, INF);
    VI steps(n);
    Q.push(P(0, ini));
    dist[ini] = 0;
    steps[ini] = 0;

    while (not Q.empty()) {
        P p = Q.top(); Q.pop();
        int d = -p.first;
        int v = p.second;

        if (d == dist[v]) {
            if (v == fi) {
                cout << "cost " << d << ", " << steps[fi] << " step(s)" << endl;
                return;
            }
            for (P arc : G[v]) {
                int d2 = d + arc.first;
                int y = arc.second;
                if (d2 < dist[y]) {
                    dist[y] = d2;
                    steps[y] = steps[v] + 1;
                    Q.push(P(-d2, y));
                }
                else if (d2 == dist[y]) {  // hi ha un altre cami amb mateixa dist
                // si es fa amb menys steps el nou cami, actu steps per arribar al vertex
                    if (steps[y] > steps[v] + 1) steps[y] = steps[v] + 1;
                }
            }
        }
    }
    cout << "no path from " << ini << " to " << fi << endl;
}


int main() {
    int n, m;
    while (cin >> n >> m) {
        VVP G(n);
        while(m--) {
            int x, y, c;
            cin >> x >> y >> c;
            G[x].push_back(P(c, y));
        }
        int ini, fi;
        cin >> ini >> fi;
        dijkstra(G, n, ini, fi);
    }
}
