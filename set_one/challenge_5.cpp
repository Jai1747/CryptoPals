#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>
#include "library.h"


string readFileToString(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return "";
    }

    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    string content(size, '\0');
    if (file.read(&content[0], size)) {
        return content;
    } else {
        cerr << "Error reading file: " << filename << endl;
        return "";
    }
}

int main() {
  string str = "Burning 'em, if you ain't quick and nimble\nI go crazy when I "
               "hear a cymbal";
  
  string key = "ICE";

  string ans = "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f";
  string myAns =  bytesToHexString(repeatingXor(str, key));

  cout << myAns << endl;
  cout << (ans == myAns ? "Match" : "No Match") << endl;

}


