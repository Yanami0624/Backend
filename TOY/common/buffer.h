#include <iostream>
#include <vector>
using namespace std;

class Buffer {
private:
    vector<char> buffer;

public:
    void append(string &s) {
        for(auto c: s) buffer.emplace_back(c);
    }
};