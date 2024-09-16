#include <iostream>
#include "library.h"

int main() {
    string hexStr = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    string answer = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";

    vector<uint8_t> bytes = hexStringToBytes(hexStr);
    string myAnswer = bytesToBase64(bytes);

    cout << answer << endl;
    cout << (answer == myAnswer ? "Match" : "No Match") << endl;

}


