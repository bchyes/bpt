#include "bpt.h"
#include "string.hpp"

sjtu::bpt<sjtu::string, int,4> tree("file", "file_delete");

int main() {
    int n;
    std::cin >> n;
    for (int i = 1; i <= n; i++) {
        sjtu::string s;
        std::cin >> s;
        if (s == "insert") {
            int x;
            std::cin >> s;
            std::cin >> x;
            try { tree.insert(sjtu::pair<sjtu::string, int>(s, x)); }
            catch (...) {}
        } else if (s == "find") {
            std::cin >> s;
            try { std::cout << tree.find(s) << std::endl; }
            catch (...) { std::cout << "NULL" << std::endl; }
        } else if (s == "delete") {
            int x;
            std::cin >> s;
            //std::cin >> x;
            //try { tree.erase(sjtu::pair<sjtu::string, int>(s, x)); }
            try { tree.erase(s); }
            catch (...) {}
        } else if (s == "clean") {
            tree.clean();
        } else if (s == "lower_bound") {
            try {
                std::cin >> s;
                sjtu::string k = tree.lower_bound(s);
                int x = 1;
            }
            catch (...) { std::cout << "NULL" << std::endl; }
        }
    }
    std::cout << 1;
}
