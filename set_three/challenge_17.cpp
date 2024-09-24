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

bool checkPadding(string ciphertext, string iv) {
  string plaintext = aes_128_cbc_decrypt(key, iv, ciphertext);
  
    if (removePadding(plaintext) == "FALSE")
      return false;

    return true;
}

string decrypt(string block, string cipher) {
    vector<uint8_t> answer(16);
    int len = cipher.length();

    for (int i = 1; i<=16; i++) {
        string xoredCipher = cipher;
        string xoredIv = iv;

        if (len == 16) {
           for (int k = 1; k < i; k++)
            xoredIv[16 - k] = answer[16 - k] ^ (static_cast<uint8_t>(i));
        } else {
          for (int k = 1; k < i; k++)
            xoredCipher[16- k] = answer[16 - k] ^ (static_cast<uint8_t>(i));   
        }

        for (int j = 0; j < 256; j++) {
            string temp = (len==16 ? xoredIv : xoredCipher);
            if (len == 16) {
                temp[16 - i] = static_cast<uint8_t>(j);
                if (checkPadding(cipher, temp)) {
                    answer[16 - i] = static_cast<uint8_t>(j);
                      // add check
                    break;
                }
            } else {
              temp[len - 16 - i] = static_cast<uint8_t>(j);
              if (checkPadding(temp, iv)) {
                    answer[16 - i] = static_cast<uint8_t>(j);
                      // add check
                    break;
                 }
            }
        }
        
        cout << "Decrypted " << i << " th byte from right "
             << static_cast<int>(answer[16 - i])
            << dec << " " << answer[16-i] << endl;
    }
    return bytesToPlaintext(answer);
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

    string ciphertext = encrypt();
    string answer;
    vector<string> decryptedBlocks;

    int numBlock = ciphertext.length() / 16;
    cout << "ciphertext has " << numBlock << " blocks" << endl;

    for (int i = numBlock-1; i >= 0; i--) {
      string block;

      if (i == 0)
        block = iv;
      else
        block = ciphertext.substr(16 * (i-1), 16);

      string decryptedBlock = decrypt(block, ciphertext.substr(0, 16 * (i + 1)));
      decryptedBlocks.push_back(decryptedBlock);
      break;
    }

    // for (int i = 1; i <= numBlock; i++)
    //   answer += decryptedBlocks[numBlock - i];

    // cout << "decrypted plain text is " << answer << endl;
    // cout << (answer==ciphertext? "yes" : "no" ) << endl;
}
