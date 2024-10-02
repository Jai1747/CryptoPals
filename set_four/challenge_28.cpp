#include "library.h"
#include <cstdint>
#include <ios>
#include <iostream>
#include <openssl/cryptoerr_legacy.h>
#include <ostream>
#include <sys/types.h>
#include <openssl/rand.h>
#include <random>
#include <iomanip>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <map>

using namespace std;

u_int32_t leftrotate(u_int32_t value, int n) {
  return (value << n) | (value >> (32-n)) & 0xFFFFFFFF;
}

vector<u_int8_t> thirtyTwo(u_int32_t val) {
  vector<u_int8_t> answer;
  
  answer.push_back(static_cast<u_int8_t>(val >> 24));
  answer.push_back(static_cast<u_int8_t>(val >> 16));
  answer.push_back(static_cast<u_int8_t>(val >> 8));
  answer.push_back(static_cast<u_int8_t>(val));

  return answer;
}
string bytesToHexString(uint32_t value) {
    stringstream ss;
    ss << hex << setw(8) << setfill('0') << value;
    return ss.str();
}

string sha1(string message) {
    // intialize variables
    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xEFCDAB89;
    uint32_t h2 = 0x98BADCFE;
    uint32_t h3 = 0x10325476;
    uint32_t h4 = 0xC3D2E1F0;
    
    int ml = message.length();
    uint64_t ml_bits = ml * 8;

    // pre-processing
    string preprocess = message;

    uint8_t zero = 0;
    uint8_t one = 128;

    preprocess += static_cast<unsigned char>(one);
    while ((preprocess.length() * 8) % 512 != 448) {
        preprocess += static_cast<unsigned char>(zero);
    }
    for (int i = 7; i>=0; i--)
      preprocess += static_cast<unsigned char>((ml_bits >> (i * 8)) & 0xFF);

    // partition the message in successive 512-bit chunks
    int numBlocks = preprocess.length() * 8 / 512;
    for (int k = 0; k < numBlocks; k++) {
      string temp = preprocess.substr(k * 64, 64);
      vector<uint32_t> w(80);

      for (int j = 0; j < 16; j++) {
        uint32_t l1 = static_cast<uint32_t>(static_cast<unsigned char>(temp[j * 4]));
        uint32_t l2 = static_cast<uint32_t>(static_cast<unsigned char>(temp[j * 4 + 1]));
        uint32_t l3 = static_cast<uint32_t>(static_cast<unsigned char>(temp[j * 4 + 2]));
        uint32_t l4 = static_cast<uint32_t>(static_cast<unsigned char>(temp[j * 4 + 3]));
        w[j] = ((l1 << 24) | (l2 << 16) | (l3 << 8) | (l4));

      }
      // extend the sixteen 32-bit words into eighty 32-bit words:
      for (int i = 16; i < 80; i++)
        w[i] = leftrotate((w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]), 1);

      // initialize hash value for this chunk:
      uint32_t a = h0;
      uint32_t b = h1;
      uint32_t c = h2;
      uint32_t d = h3;
      uint32_t e = h4;

      // main loop
      for (int i = 0; i < 80; i++) {
        uint32_t f;
        uint32_t k;

        if (i<=19) {
          f = (b & c) | (~b & d);
          k = 0x5A827999;
        } else if (20<=i && i<=39) {
          f = b ^ c ^ d;
          k = 0x6ED9EBA1;
        } else if (40 <= i && i <= 59) {
          f = (b & c) | (b & d) | (c & d);
          k = 0x8F1BBCDC;
        } else if (60 <= i && i <= 79) {
          f =b ^ c ^ d;
          k = 0xCA62C1D6;
        }
        uint32_t temp = leftrotate(a, 5) + f + e + k + w[i];
        e = d;
        d = c;
        c = leftrotate(b, 30);
        b = a;
        a = temp;
      }

      // Add this chunk's hash to result so far:
      h0 += a;
      h1 += b;
      h2 += c;
      h3 += d;
      h4 += e;
    }
    
    string hash;
    hash += bytesToHexString(h0);
    hash += bytesToHexString(h1);
    hash += bytesToHexString(h2);
    hash += bytesToHexString(h3);
    hash += bytesToHexString(h4);
        
  return hash;
}

string shaLib(const string& data) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);
    
    ostringstream result;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        result << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    }
    
    return result.str();
}

int main() {
  string message = "abcquehhbfuybfdlhbo";
  string hash = sha1(message);
  string hashLib = shaLib(message);

  cout << "message is " << message << endl
       << "hash is    " << hashLib << endl
       << "my hash is " << hash << endl;
              
}
