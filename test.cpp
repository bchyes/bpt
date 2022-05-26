#include <bits/stdc++.h>
#include "bpt.h"
#include "string.hpp"

using namespace std;

class Node {
public:
    sjtu::bpt<sjtu::string, int> pos;

    Node() : pos("a.out", "b.out") {};

};

int main() {
    Node x;
    cout << x.pos.empty() << endl;
    x.pos.insert(sjtu::pair<sjtu::string, int>(sjtu::string("I_am_the_admin"), 1));
    cout << x.pos.count(sjtu::string("I_am_the_admin")) << endl;
    cout << x.pos.find(sjtu::string("I_am_the_admin")) << endl;
    x.pos.modify(sjtu::string("I_am_the_admin"), 2);
    cout << x.pos.find(sjtu::string("I_am_the_admin")) << endl;
    cout << x.pos.count(sjtu::string("23431")) << endl;
    cout << x.pos.empty() << endl;
    return 0;
}
