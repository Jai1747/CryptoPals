#include <iostream>
#include <cstdint>
#include <fstream>
#include "library.h"

int main() {
    string filename = "input_4.txt";
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return 1;
    }

    vector<string> strings;
    string line;
    
    while (getline(file, line)) {
        strings.push_back(line);
    }

    file.close();

    for (auto str : strings) {
        vector<uint8_t> bytes = hexStringToBytes(str);

        for (int i = 0; i < 256; i++) {
            vector<uint8_t> xored = singleByteXor(bytes, i);
            string answer = bytesToPlaintext(xored);
            if (isValidEnglishString(answer)) {
                cout << answer << " " << i << endl;
            }

                
        }
    }

}


