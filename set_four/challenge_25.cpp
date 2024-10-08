#include "library.h"
#include <cstdint>
#include <ios>
#include <iostream>
#include <openssl/cryptoerr_legacy.h>
#include <string>
#include <sys/types.h>
#include <openssl/rand.h>
#include <random>
#include <iomanip>
#include <openssl/err.h>
#include <map>

using namespace std;

string key;
int ctr = 0;
string nonce(8, '\0');

string generateRandomString(int length) {
    random_device rd; 
    mt19937 generator(rd()); 
    uniform_int_distribution<int> distribution(0, 255);  

    string randomString;
    for (int i = 0; i < length; ++i) {
        randomString += static_cast<char>(distribution(generator));  // Cast int to char
    }

    return randomString;
}

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

string edit(string ciphertext, int offset, string newtext) {
    string decrypted = encrpyt(ciphertext);
    int n = decrypted.length();

    if (offset + newtext.length() > ciphertext.length()) {
        cerr << "Error: Replacement goes out of bounds of the ciphertext!" << endl;
        return "";
    }

    for (size_t i = 0; i < newtext.length(); ++i) {
        decrypted[offset + i] = newtext[i];
    }
    string newCiphertext = encrpyt(decrypted);
    return newCiphertext;
}

string readFileToString(const string& filename) {
    ifstream file(filename);
    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();
    return content;
}

string getBit(string ciphertext, int pos) {
    char curBit = ciphertext[pos];
    for (int i = 0; i < 256; i++) {
      string x;
      x = static_cast<char>(i);
      string newCiphertext = edit(ciphertext, pos, x);
      if (newCiphertext[pos] == curBit) {
        cout << " Found bit " << x << endl;
        return x;
      }
    }
    return "";
}

int main() {
    string plaintext = readFileToString("output.txt");
    string key = generateRandomString(16);
    string ciphertext = encrpyt(plaintext);

    string answer;
    for (int i = 0; i < ciphertext.length(); i++) {
        answer+= getBit(ciphertext, i);
    }
    cout << answer << endl;
    cout << (plaintext==answer ?   "YES" : "NO");
}
