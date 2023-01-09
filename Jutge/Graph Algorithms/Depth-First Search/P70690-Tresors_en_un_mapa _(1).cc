#include <iostream>
#include <vector>
using namespace std;

typedef vector<char> Fila;
typedef vector<Fila> Matriu;


void llegir_mapa(Matriu& M, int f, int c) {
    for (int i = 0; i < f; ++i) {
        for (int j = 0; j < c; ++j) cin >> M[i][j];
    }
}

//x, y posicio que estem ara
bool dfs(Matriu& M, int f, int c, int x, int y) {
    // per veure per on estem passant:  cout << x << ' ' << y << endl; o tambe cerr << x
    if (x < 0 or x >= f or y < 0 or y >= c or M[x][y] == 'X') return false;
    if (M[x][y] == 't') return true;
    M[x][y] = 'X'; // marquem per despres no tornar a passar
    return dfs(M, x + 1, y) or dfs(M, x - 1, y) or dfs(M, x, y + 1) or dfs(M, x, y - 1);
}

int main() {
    int f, c;
    cin >> f >> c;
    Matriu M(f, Fila(c)); //mapa
    llegir_mapa(M, f, c);

    int f_ini, c_ini;  //van de 1 a n, restar 1 per començar a 0
    cin >> f_ini >> c_ini;

    if (dfs(M, f, c, f_ini - 1, c_ini - 1)) {
        cout << "yes";
    }
    else cout << "no";
    cout << endl;
}
