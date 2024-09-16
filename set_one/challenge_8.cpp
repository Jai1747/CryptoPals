#include <memory>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/bio.h>
#include <openssl/aes.h>
#include <cstring>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <bitset>
#include <algorithm>
#include "library.h"

using namespace std;

int splitString(const string& str, size_t partSize) {
    map<string, int> m;
    size_t totalSize = str.size();
    
    for (size_t i = 0; i < totalSize; i += partSize) {
        string temp = str.substr(i, partSize);
        if (m.find(temp) == m.end())
          m[temp] = 0;
        else
            m[temp]++;
    }
    int answer = 0;
    for (auto x : m)
      answer += x.second - 1;
    return answer;
}


int main() {
    string filename = "input_8.txt";
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return 1;
    }
    vector<string> strings;
    string line;
    
    while (getline(file, line)) {
        strings.push_back(bytesToPlaintext(hexStringToBytes(line)));
    }

    file.close();

    for (int i = 0; i < strings.size(); i++) {
      cout << i << " " << splitString(strings[i], 16) << endl;
    }
    return 0;
}
