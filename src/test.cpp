#include <iostream>
#include <map.hpp>

using namespace std;

int main() {
    int opt, num;
    sjtu::map<int, int> m;
    while (cin >> opt >> num) {
        if (opt == 1)
            m.insert({num, 0});
        else
            m.erase(m.find(num));
        cout << m.size() << endl;
    }
}