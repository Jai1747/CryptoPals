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
string prefix = "comment1=cooking%20MCs;userdata=";
string suffix = ";comment2=%20like%20a%20pound%20of%20bacon";

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

string encrypt(string str) {
  string plaintext = prefix + str + suffix;

  string result = ""; // Use a string stream to build the output

    for (char c : plaintext) {
        if (c == ';') {
            result +="%3B"; // Replace ';' with '%3B'
        } else if (c == '=') {
            result += "%3D"; // Replace '=' with '%3D'
        } else {
            result += c; // Keep other characters unchanged
        }
    }
    string padded_plaintext = bytesToPlaintext(pkcs7(result, 16));
    return aes_128_cbc_encrypt(key, iv, padded_plaintext);
}

bool detect(string ciphertext) {
  string plaintext = aes_128_cbc_decrypt(key, iv, ciphertext);
  string test = ";admin=true;";

  plaintext = removePadding(plaintext);
  size_t pos = plaintext.find(test);
    if (pos != string::npos) {
        return true;
    }
    return false;
}

void printStringInHex(const string& input) {
    for (unsigned char c : input) {
        cout << hex << setw(2) << setfill('0') << (int)c << " ";
    }
    cout << dec << endl;
}



int main() {
  key = generateRandomString(16);
  iv = generateRandomString(16);

  string input = "xxxxxxxxxx1admin2true3";

  string cipher = encrypt(input);

  int pos1 = 48;
  int pos2 = 54;
  int pos3 = 59;

  string modCipher = cipher;
  modCipher[pos1-16] ^= (';' ^ '1');
  modCipher[pos2-16] ^= ('=' ^ '2');
  modCipher[pos3-16] ^= (';' ^ '3');

  cout << detect(modCipher);
}
