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

string key;

void setKey() {
    ifstream file("/usr/share/dict/words");
    vector<string> words;
    string word;

    if (!file) {
        cerr << "Error opening file." << endl;
    }
    while (file >> word) {
        words.push_back(word);
    }
    file.close();

    srand(time(nullptr));

    if (!words.empty()) {
        int randomIndex = rand() % words.size();
        cout << "Random word: " << words[randomIndex] << endl;
        key = words[randomIndex];
    } else {
        cerr << "No words found in the file." << endl;
    }
    return;
}

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

string mdPad(string message, int offset) {
    int ml = message.length() + offset;
    uint64_t ml_bits = ml * 8;
    string preprocess = message;
    uint8_t zero = 0;
    uint8_t one = 128;

    preprocess += static_cast<unsigned char>(one);
    while (((preprocess.length() + offset) * 8) % 512 != 448) {
        preprocess += static_cast<unsigned char>(zero);
    }
    for (int i = 7; i>=0; i--)
      preprocess += static_cast<unsigned char>((ml_bits >> (i * 8)) & 0xFF);
    return preprocess;
}

string sha1(string preprocess, uint32_t H0, uint32_t H1, uint32_t H2, uint32_t H3, uint32_t H4) {
    // intialize variables
    uint32_t h0 = H0;
    uint32_t h1 = H1;
    uint32_t h2 = H2;
    uint32_t h3 = H3;
    uint32_t h4 = H4;

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

string secretMAC(string message) {
    string secret_message = key + message;
    return shaLib(secret_message);
}

void printStringInHex(const string& str) {
    for (unsigned char c : str) {
        cout << hex << setw(2) << setfill('0') << static_cast<int>(c) << " ";
    }
    cout << dec << endl;
}

int main() {
    setKey();
    

    uint32_t H0 = 0x67452301;
    uint32_t H1 = 0xEFCDAB89;
    uint32_t H2 = 0x98BADCFE;
    uint32_t H3 = 0x10325476;
    uint32_t H4 = 0xC3D2E1F0;

    string message = "comment1=cooking%20MCs;userdata=foo;comment2=%20like%20a%"
                     "20pound%20of%20bacon";
    string admin = ";admin=true";
    string mac = secretMAC(message);

    vector<uint8_t> macBytes = hexStringToBytes(mac);
    if (macBytes.size() != 20) {
        cerr << "Invalid MAC size." << endl;
        return 1;
    }

    uint32_t nH0 = (static_cast<uint32_t>(macBytes[0]) << 24) | 
                   (static_cast<uint32_t>(macBytes[1]) << 16) | 
                   (static_cast<uint32_t>(macBytes[2]) << 8)  | 
                    static_cast<uint32_t>(macBytes[3]);

    uint32_t nH1 = (static_cast<uint32_t>(macBytes[4]) << 24) |
                   (static_cast<uint32_t>(macBytes[5]) << 16) |
                   (static_cast<uint32_t>(macBytes[6]) << 8) |
                   static_cast<uint32_t>(macBytes[7]);

    uint32_t nH2 = (static_cast<uint32_t>(macBytes[8]) << 24) |
                   (static_cast<uint32_t>(macBytes[9]) << 16) |
                   (static_cast<uint32_t>(macBytes[10]) << 8) |
                   static_cast<uint32_t>(macBytes[11]);

    uint32_t nH3 = (static_cast<uint32_t>(macBytes[12]) << 24) |
                   (static_cast<uint32_t>(macBytes[13]) << 16) |
                   (static_cast<uint32_t>(macBytes[14]) << 8) |
                   static_cast<uint32_t>(macBytes[15]);

    uint32_t nH4 = (static_cast<uint32_t>(macBytes[16]) << 24) | 
                   (static_cast<uint32_t>(macBytes[17]) << 16) | 
                   (static_cast<uint32_t>(macBytes[18]) << 8)  | 
                    static_cast<uint32_t>(macBytes[19]);

    for (int len = 0; len < 20; len++) {
        if (len != key.length())
            continue;

        string preprocess = mdPad(message, len);
        string forged_message = preprocess + admin;
        string actual_mac = secretMAC(forged_message);

        string temp_preprocess = mdPad(admin, 128);
        string forgery = sha1(temp_preprocess, nH0, nH1, nH2, nH3, nH4);

        if (forgery == actual_mac) {
          cout << "FORGED !!!" << endl;
          break;
        }
    }
}
