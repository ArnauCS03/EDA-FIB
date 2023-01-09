#include <iostream>   
#include <vector>
#include <queue>
using namespace std;


const int INF = 1e9;

typedef pair<int, int> P; //cost, node
typedef vector<P> VP;     //arcs
typedef vector<VP> VVP;   //graf
typedef vector<int> VI;   //distancies


void dijkstra(const VVP& G, int n, int ini, int fi) {

    priority_queue<P> Q;
    VI dist(n, INF);    // al principi totes les distancies infinites, despres tindra la millor dist per arribar al vertex
    // VI pare(n);  // si volem recordar el cami
    // VI steps(n); // nombre passos per arribar
    // VI ways(n);  // nombre maneres arribar al vertex
    Q.push(P(0, ini));
    dist[ini] = 0;
    // steps[ini] = 0;
    // ways[ini] = 1;

    while (not Q.empty()) {
        P p = Q.top(); Q.pop();
        int d = -p.first;
        int v = p.second; //vertex

        if (d == dist[v]) { // si la distancia acumulada 'd' es la mateixa que la millor que tenim guardada per arribar al node
                            // Important perque potser te varies distancies per arribar al vertex, pero ens assegurem que es la de minima dist 'd'
                            // si no es compleix, es que hi ha una distancia mes curta per arribar el vertex i ens ho podem saltar.

            if (v == fi) {      // comporvar si hem arribat a l'ultim node, si volem dist de un node a tots no fa falta aixo
                cout << d << endl;   // minim cost per arribar de ini a fi
                return;
                /* [opcional] enrecordar cami:  
                VI cami;
                while (v != ini) { // anem a la inversa
                    cami.push_back(v);
                    v = pare[v];
                }
                cout << ini;
                esciure(cami);
                */
            }
            for (P arc : G[v]) {   // recorrer vertex adjacents
                int d2 = d + arc.first;   // ojo no es -(arc.fist) i recordar + d
                int y = arc.second; // altre vertex
                if (d2 < dist[y]) { // si des del nou vertex la dist del cami es mes curt, actu dist per arribar al vertex
                    dist[y] = d2;
                    //pare[y] = v;
                    //steps[y] = steps[v] + 1;
                    //ways[y] = ways[v];
                    Q.push(P(-d2, y));
                }
                /*extra: camí alternatiu que tb te min dist
                else if (d2 == dist[y]) {   // tambe es pot arribar amb la mateixa distancia
                     ways[y] += ways[v];  // manera alternativa d'arribar al vertex
                     if (steps[y] > steps[v] + 1) steps[y] = steps[v] + 1;   // si els passos del cami alternatiu son mes curts, actualitzarlos
                }
                */
            }
        }
    }
    cout << "no path from " << ini << " to " << fi << endl;
}


int main() {

    int n, m;
    while (cin >> n >> m) {
        VVP G(n);    // no posem nombre d'arestes perque fem push back i no sabem quantes arestes surten de cada
        while (m--) {
            int x, y, c;
            cin >> x >> y >> c;
            G[x].push_back(P(c, y));
            //G[y].push_back(P(c, x));  //si es no dirigit (unic que cal afegir al codi)
        }
        int ini, fi;
        cin >> ini >> fi;
        dijkstra(G, n, ini, fi);
    }
}
