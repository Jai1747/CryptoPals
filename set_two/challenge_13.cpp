#include "library.h"
#include <cstdint>
#include <ios>
#include <iterator>
#include <openssl/cryptoerr_legacy.h>
#include <string>
#include <sys/types.h>
#include <openssl/rand.h>
#include <random>
#include <iomanip>
#include <openssl/err.h>
#include <map>
#include <vector>

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

void printStringInHex(const string& input) {
    for (unsigned char c : input) {
        cout << hex << setw(2) << setfill('0') << (int)c << " ";
    }
    cout << dec << endl;
}

vector<pair<string, string>> parse(string cookie) {
    vector<pair<string, string>> answer;
    size_t pos1 = cookie.find('=');
    size_t pos2 = cookie.find('&', pos1);
    size_t pos3 = cookie.find('=', pos2);
    size_t pos4 = cookie.find('&', pos3);
    size_t pos5 = cookie.find('=', pos4);

    answer.push_back(
        {cookie.substr(0, pos1), cookie.substr(pos1 + 1, pos2 - pos1 - 1)});
    answer.push_back(
        {cookie.substr(pos2 + 1, pos3 - pos2 - 1), cookie.substr(pos3 + 1, pos4 - pos3 - 1)});
    answer.push_back(
        {cookie.substr(pos4 + 1, pos5 - pos4 - 1), cookie.substr(pos5 + 1)});

    return answer;
}

string profile_for(string email) {
    email.erase(remove(email.begin(), email.end(), '&'), email.end());
    email.erase(remove(email.begin(), email.end(), '='), email.end());

    string answer = "email=";
    answer += email;
    answer += "&uid=10&role=user";

    return answer;
}

string encryptUser(string email) {
  string plaintext = profile_for(email);
  string new_plaintext = bytesToPlaintext(pkcs7(plaintext, 16));

  string ciphertext;
  aes_128_ecb_encrypt(key, new_plaintext, ciphertext);
  return ciphertext;
}

vector<pair<string, string>> decryptParse(string ciphertext) {
    string plaintext;
    aes_128_ecb_decrypt(key, ciphertext, plaintext);
    return parse(plaintext);
}

int main() {
   key = generateRandomString(16);
   string email(10, '\0');

   string temp1 = "admin";                  
   temp1.resize(16, '\0');
   fill(temp1.begin() + 5, temp1.end(), 10);

   string temp2 (16, 'a');
   temp2[15] = '.';
   email += temp1 + temp2 + "com";

   string cookie = profile_for(email);
   string cipher = encryptUser(email);
   string modCipher = cipher;
   string block2 = cipher.substr(16, 16);
   
   modCipher.replace(64, 16, block2);
   vector<pair<string, string>> obj = decryptParse(modCipher);

   cout << obj[2].first << " " << obj[2].second;
   
}
