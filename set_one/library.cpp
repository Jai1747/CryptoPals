#include "library.h"

int singleHexToInt(char x) {
    if (x >= 'a' && x <= 'f')  
        return x - 'a' + 10;
    if (x >= '0' && x <= '9')
        return x - '0';
    return -1;
}

string byteToHex(uint8_t byte) {
  stringstream ss;
  ss << hex << setw(2) << setfill('0') << static_cast<int>(byte) << dec;
  return ss.str();
}

vector<uint8_t> hexStringToBytes(const string &str) {
    vector<uint8_t> output;
    int n = str.length();
    if (n % 2 != 0) {  
        throw invalid_argument("Hex string length must be even");
    }
    for (int i = 0; i < n; i += 2) {  
        int a = singleHexToInt(str[i]) * 16 + singleHexToInt(str[i + 1]);
        output.push_back(static_cast<uint8_t>(a));
    }
    return output;
}

string bytesToBase64(vector<uint8_t> bytes) {
    string str_base64 = "";
    int n = bytes.size();

    for (int i = 0; i < n; i += 3) {
        uint8_t byte1 = bytes[i];
        uint8_t byte2 = (i + 1 < n) ? bytes[i + 1] : 0;
        uint8_t byte3 = (i + 2 < n)? bytes[i + 2] : 0;

        uint32_t combined = (byte1 << 16) | (byte2 << 8) | byte3;
       
        uint8_t b1 = (combined >> 18) & 0x3F;
        uint8_t b2 = (combined >> 12) & 0x3F;
        uint8_t b3 = (combined >> 6) & 0x3F;
        uint8_t b4 = combined & 0x3F;

        str_base64 += base64_chars[static_cast<int>(b1)];
        str_base64 += base64_chars[static_cast<int>(b2)];

        str_base64 += (i + 1 < n) ? base64_chars[static_cast<int>(b3)] : '=';
        str_base64 += (i + 2 < n) ? base64_chars[static_cast<int>(b4)] : '=';     
  }
  return str_base64;
}

string bytesToHexString(vector<uint8_t> bytes){
    int n = bytes.size();
    string answer = "";
    for (uint8_t byte : bytes)
        answer += byteToHex(byte);
    return answer;
}

vector<uint8_t> xorBytes(vector<uint8_t> bytes1, vector<uint8_t> bytes2) {
    int n = bytes1.size();
    vector<uint8_t> answer;
    for (int i = 0; i < n; i++)
        answer.push_back(bytes1[i] ^ bytes2[i]);
    
    return answer;
}

bool isValidEnglishString(string plaintext) {
    int count = 0;
    for (char ch : plaintext) {
        if (!((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || 
              ch == ' ' || ch == '\n' || ch == '.' || ch == ',' || ch == '-')) {
            count++;
        }
    }
    if (count > 5)
        return false;
    return true;
}

vector<uint8_t> singleByteXor(vector<uint8_t> bytes, int x) {
    int n = bytes.size();
    uint8_t key = static_cast<uint8_t>(x);

    vector<uint8_t> answer;
    for (int i = 0; i < n; i++)
        answer.push_back(bytes[i] ^ key);
    
    return answer;
}

string bytesToPlaintext(vector<uint8_t> bytes) {
  string answer = "";
  for (uint8_t byte : bytes)
    answer += byte;
  return answer;
}

vector<uint8_t> repeatingXor(string plaintext, string key) {
    vector<uint8_t> answer;

    int n = plaintext.size();
    int m = key.size();
    for (int i = 0; i < n; i++) 
        answer.push_back(static_cast<uint8_t>(plaintext[i] ^ key[i%m]));

    return answer;
}