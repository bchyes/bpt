#include<bits/stdc++.h>

using namespace std;

int main() {
    freopen("data1.in", "w", stdout);
    srand((unsigned) time(NULL));
    int maxn = 50;
    cout << maxn << endl;
    for (int i = 1; i <= maxn; i++) {
        int x = int(rand() % 3) + 1;
        if (x == 1) {
            cout << "insert ";
            int y = int(rand() % 5) + 1, z = int(rand() % 10) + 1;
            cout << y << " " << z << endl;
        }
        if (x == 2) {
            cout << "find ";
            int y = int(rand() % 5) + 1;
            cout << y << endl;
        }
        if (x == 3) {
            cout << "delete ";
            int y = int(rand() % 5) + 1, z = int(rand() % 10) + 1;
            cout << y << " " << z << endl;
        }
    }
}