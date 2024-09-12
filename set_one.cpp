#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;

const string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int getVal(char x) {
    if (x >= 'a' && x <= 'f')  
        return x - 'a' + 10;
    if (x >= '0' && x <= '9')  
        return x - '0';
    throw invalid_argument("Invalid hex character");  
}

vector<uint8_t> hexStringToBytes(const string &str) {
    vector<uint8_t> output;
    int n = str.length();
    if (n % 2 != 0) {  
        throw invalid_argument("Hex string length must be even");
    }
    for (int i = 0; i < n; i += 2) {  
        int a = getVal(str[i]) * 16 + getVal(str[i + 1]);
        output.push_back(static_cast<uint8_t>(a));
    }
    return output;
}

string convert(vector<uint8_t> bytes) {
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

int main() {
    string hexStr = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120"
                  "706f69736f6e6f7573206d757368726f6f6d";
    string answer =
        "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";
    
    vector<uint8_t> bytes = hexStringToBytes(hexStr);
    string str_base64 = convert(bytes);

    cout << str_base64 << endl;
    cout << answer << endl;
    cout << (answer == str_base64 ? "Match" : "No Match") << endl;

}


