#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/bio.h>
#include <openssl/aes.h>
#include <cstring>
#include <string>
#include <fstream>
#include <bitset>
#include <algorithm>

using namespace std;

int singleHexToInt(char x);

vector<uint8_t> plaintextToBytes(const string &str);
string bytesToPlaintext(vector<uint8_t> bytes);

vector<uint8_t> hexStringToBytes(const string &str);
string bytesToHexString(vector<uint8_t> bytes);

string base64_decode(const string &encoded);
string base64_encode(const std::string &in);

string xorPlaintext(string a, string b);

vector<uint8_t> pkcs7(string plaintext, int pad_length);

void aes_128_ecb_encrypt(const string& key, const string& plaintext, string& ciphertext);
void aes_128_ecb_decrypt(const string &key, const string &ciphertext,
                         string &plaintext);

string aes_128_cbc_encrypt(string key, string iv, string plaintext);
string aes_128_cbc_decrypt(string key, string iv, string ciphertext);


