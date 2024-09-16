#include <iostream>
#include "library.h"

int main() {
    string hexStr1 = "1c0111001f010100061a024b53535009181c";
    string hexStr2 = "686974207468652062756c6c277320657965";
    string answer = "746865206b696420646f6e277420706c6179";

    vector<uint8_t> bytes1 = hexStringToBytes(hexStr1);
    vector<uint8_t> bytes2 = hexStringToBytes(hexStr2);
    vector<uint8_t> bytes3 = xorBytes(bytes1, bytes2);

    string myAnswer = bytesToHexString(bytes3);
    cout << answer << endl;
    cout << (answer == myAnswer ? "Match" : "No Match") << endl;

}


