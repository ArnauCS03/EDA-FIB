#include <iostream>
#include <vector>
#include <queue>
using namespace std;


typedef vector<char> Fila_c;
typedef vector<Fila_c> Matriu_c;
typedef vector<int> Fila_i;
typedef vector<Fila_i> Matriu_i;

const vector<int> inc_i = { 1, -1, 0,  0 };
const vector<int> inc_j = { 0,  0, 1, -1 };

int n, m;


int bfs(Matriu_c& mapa, int x, int y) {
	queue<pair<int, int> > Q;
	Matriu_i dist(n, Fila_i(m, -1));      		// inici totes dist a -1 perque no estan descobertes
												// despres conte dist des de l'origen
	Q.push(make_pair(x, y));					// posem a la cua la posicio inical
	dist[x][y] = 0;								// la posicio inicial es troba a distancia 0
	
	while (not Q.empty()) {
		pair<int, int> pos_actual = Q.front();	// agafem una posicio del principi cua
		Q.pop();		
		int i = pos_actual.first;
		int j = pos_actual.second;
		
		for (int d = 0; d < 4; ++d) {						 // mirar les 4 direccions possibles
			int i2 = i + inc_i[d];							 // noves pos a un dels costats que mirarem
			int j2 = j + inc_j[d];
			
			if (i2 >= 0 and i2 < n and j2 >= 0 and j2 < m) {  // estem dins els limits del mapa
				if (dist[i2][j2] == -1) {					  // si no hem passat per aqui
					dist[i2][j2] = dist[i][j] + 1;			  // augentem la distancia recorreguda
					
					if (mapa[i2][j2] == 't') return dist[i2][j2]; // dist fins el primer t que troba
					if (mapa[i2][j2] == '.') Q.push(make_pair(i2, j2));  // nova pos a tractar
				}
			}
		}
	}
	return -1;
}


int dist_min(Matriu_c& mapa, int f_ini, int c_ini) {
	return bfs(mapa, f_ini, c_ini);
}


int main() {
	cin >> n >> m;
	Matriu_c mapa(n, Fila_c(m));
	
	for (int i = 0; i < n; ++i) 
		for (int j = 0; j < m; ++j) cin >> mapa[i][j];
	
	int f_ini, c_ini;
	cin >> f_ini >> c_ini; 
	
	int distancia = dist_min(mapa, f_ini-1, c_ini-1);
	
	if (distancia == -1) cout << "no es pot arribar a cap tresor";
	else cout << "distancia minima: " << distancia;
	cout << endl;
}
