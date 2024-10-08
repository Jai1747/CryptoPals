#include "library.h"
#include <cstdint>
#include <ios>
#include <iostream>
#include <openssl/cryptoerr_legacy.h>
#include <sys/types.h>
#include <openssl/rand.h>
#include <random>
#include <iomanip>
#include <openssl/err.h>
#include <map>

using namespace std;

string key = "YELLOW SUBMARINE";
int ctr = 0;
string nonce(8, '\0');

string convertLittleEndian(int num) {
    uint64_t x = static_cast<uint64_t>(num);
    string byte_string(8, '\0');

    for (int i = 0; i < 8; i++) {
        byte_string[i] = static_cast<char>((x >> (i * 8)) & 0xFF);  // Shift and mask
    }
    return byte_string;
}
string getKeyString(int n) {
    string answer;
    for (int i = ctr; i < ctr + n; i++) {
        string tempCtr = convertLittleEndian(i);
        string tempPlaintext = nonce;
        tempPlaintext += tempCtr;
        string tempCipher;
        aes_128_ecb_encrypt(key, tempPlaintext, tempCipher);
        answer += tempCipher;
    }
    return answer;    
}

string encrpyt(string plaintext) {
    int n = plaintext.length();
    int numBlock = n / 16 + 1;
    string keyString = getKeyString(numBlock);

    string answer;
    for (int i = 0; i < n; i++)
      answer += plaintext[i] ^ keyString[i];
    return answer;
}

int main() {
    string base64_str = "L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2syLXzhPweyyMTJULu/"
                      "6/kXX0KSvoOLSFQ==";
    string cipher = base64_decode(base64_str);
    string plaintext = encrpyt(cipher);
    cout << plaintext << endl;
}
