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

string key;
string iv;
vector<string> strings;

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

string encrypt() {
    random_device rd; 
    mt19937 generator(rd());
    uniform_int_distribution<int> distribution(0, 9);

    string str = strings[distribution(generator)];

    string plaintext = bytesToPlaintext(pkcs7(str, 16));
    string ciphertext = aes_128_cbc_encrypt(key, iv, plaintext);

    return ciphertext;
}

void printStringInHex(const string& input) {
    for (unsigned char c : input) {
        cout << hex << setw(2) << setfill('0') << (int)c << " ";
    }
    cout << dec << endl;
}

string removePadding(string str) {
    int n = str.length();
    if (n % 16 != 0)
      return "FALSE";

    int last = static_cast<int>(static_cast<unsigned char>(str[n - 1]));
    if (last > 0 && last < 16) {
      for (int j = 1; j <= last; j++) {
        if (str[n - j] != str[n - 1])
          return "FALSE";
      }
      return str.erase(n - last);
    }
    return "FALSE";
}

bool checkPadding(string ciphertext) {
    string plaintext = aes_128_cbc_decrypt(key, iv, ciphertext);

    if (removePadding(plaintext) == "FALSE")
      return false;

    return true;
}


int main() {
    key = generateRandomString(16);
    iv = generateRandomString(16);

    ifstream file("input.txt"); // Open the file
    if (!file) {
            cerr << "Unable to open file";
            return 1;
        }

    string line;
    while (getline(file, line)) { 
        strings.push_back(base64_decode(line));
    }

    file.close();

    string cipher = encrypt();
    cipher[0] = 'b';
    cout << checkPadding(cipher);
    // for (int i = 0; i < 256; i++) {
    //   string temp = cipher;
    //   temp[cipher.length() - 1] = static_cast<uint8_t>(i);
    //   cout << i << " " << checkPadding(temp) << endl;
    // }
}
