#include "library.h"
#include <cstdint>
#include <ios>
#include <openssl/cryptoerr_legacy.h>
#include <sys/types.h>
#include <openssl/rand.h>
#include <random>
#include <iomanip>
#include <openssl/err.h>
#include <map>

using namespace std;

string key;
string unknown;

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

string encrypt(string plaintext) {
    plaintext += unknown;
    string new_plaintext = bytesToPlaintext(pkcs7(plaintext, 16));

    string ciphertext;
    aes_128_ecb_encrypt(key, new_plaintext, ciphertext);
    return ciphertext;
}

void printStringInHex(const string& input) {
    for (unsigned char c : input) {
        cout << hex << setw(2) << setfill('0') << (int)c << " ";
    }
    cout << dec << endl;
}

uint8_t decode(int i, string answer) {
  int prefix_length = (16 - (answer.length() + 1)%16) % 16;
  string plaintext(prefix_length, 'A');
  string ciphertext = encrypt(plaintext);
  string check = ciphertext.substr(answer.length() + 1 + prefix_length - 16, 16);
 
  string test_plaintext(prefix_length, 'A');
  test_plaintext += answer;

  for (int j=0; j<256; j++) {
    string temp = test_plaintext;
    temp += static_cast<uint8_t>(j);
    int length = temp.length();

    string temp_cipher = encrypt(temp);
    if (check == temp_cipher.substr(length - 16,16)) {
        return static_cast<uint8_t> (j);
    }
  }
    return -1;
}

int main() {
    key = generateRandomString(16);
    string plaintext;
    string unknown_base64 = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK";
    unknown = base64_decode(unknown_base64);
   
    int unknown_length = 0;
    string answer;
    int prev_length;
    for (int i = 0; i < 16; i++) {
        plaintext.resize(i, 'A');
        string ciphertext = encrypt(plaintext);
        if (i == 0) {
          prev_length = ciphertext.length();
          continue;
        }
          
        if (ciphertext.length() != prev_length) {
            unknown_length = prev_length - i + 1;
           // cout << "length of unknown string is " << unknown_length << endl;
            break;
        }
        prev_length = ciphertext.length();
    }
    
    for (int i = 0; i < unknown_length; i++) {
        uint8_t x = decode(i, answer);
        cout << i+1 << " Byte decoded - " << x << dec << endl;
        answer += x;
     
    }
   cout << "decode string is \n" << answer << endl;
   cout << (answer == unknown ? "Match" : "No match") << endl;

}
