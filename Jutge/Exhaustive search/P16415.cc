#include <iostream>
#include <vector>
using namespace std;


typedef vector<int> VI;
typedef vector<bool> VB;


int reines(int i, VI& t, VB& mc, VB& md1, VB& md2, int n) {
    if (i == t.size()) return 1;
    else {
        int res = 0;
        for (int j = 0; j < t.size(); ++j) {    // cols

            int diag1 = i + j;
            int diag2 = i - j + n-1;

            if (not mc[j] and not md1[diag1] and not md2[diag2]) {
                mc[j] = md1[diag1] =  md2[diag2] = true;
                t[i] = j;
                res += reines(i+1, t, mc, md1, md2, n);
                mc[j] = md1[diag1] =  md2[diag2] = false;
            }
        }
        return res;
    }
}


int main() {
    int n;
    cin >> n;
    VI t(n); // vector solucio
    VB mc(n, false);
    VB md1(2*n - 1, false);
    VB md2 = md1;
    cout << reines(0, t, mc, md1, md2, n) << endl;
}
