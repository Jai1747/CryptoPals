#include <iostream>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <bitset>
#include "library.h"

string removeNewlines(const string& str) {
    string result = str;
    result.erase(remove(result.begin(), result.end(), '\n'), result.end());
    result.erase(remove(result.begin(), result.end(), '\r'), result.end());
    return result;
}

string readFileToString(const string& filename) {
    ifstream file(filename);
    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();
    string cleanContent = removeNewlines(content);
    return cleanContent;
}

int editDistanceSingleByte(uint8_t a, uint8_t b) {
    int answer = 0;
    for (int i = 7; i >= 0; i--) {
      if (((a >> i) & 1) == ((b >> i) & 1))
        answer++;
    }
    return 8 - answer;
}

int editDistance(vector<uint8_t> x, vector<uint8_t> y) {
    int n = x.size();
    int answer = 0;

    for (int i = 0; i < n; i++) {
        answer += editDistanceSingleByte(x[i], y[i]);
    }
    return answer;
}

int base64CharToValue(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';  // 0 to 25
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a' + 26;  // 26 to 51
    } else if (c >= '0' && c <= '9') {
        return c - '0' + 52;  // 52 to 61
    } else if (c == '+') {
        return 62;
    } else if (c == '/') {
        return 63;
    } else {
        return -1;  // Invalid Base64 character
    }
}

string uintToBinaryString(uint8_t value) {
    bitset<6> binary(value);
    return binary.to_string();     
}

vector<uint8_t> base64ToBytes(string str) {
    vector<uint8_t> answer;
    int n = str.length();
    int count = 0;

    for (int i = str.size() - 1; i >= 0 && str[i] == '='; --i) 
        count++;

    string newStr = str.substr(0, str.size() - count);
    string bitString = "";

    for(int i =0 ;i< newStr.size(); i++)
        bitString += uintToBinaryString(base64CharToValue(newStr[i]));

    bitString.erase(bitString.size() - 2 * count);
    
    for (size_t i = 0; i < bitString.length(); i += 8) {
        string byteString = bitString.substr(i, 8);
        uint8_t byteValue = static_cast<uint8_t>(bitset<8>(byteString).to_ulong());
        answer.push_back(byteValue);
        
    }
    return answer;
}

float special(string plaintext) {
    int count = 0;
    for (char ch : plaintext) {
      if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == ' ' ||
          ch == '\n' || ch == '.' || ch == ',' || ch == '-' || ch == '\t')
        count++;
    }
   return 1-float(count)/plaintext.size();
}


int main() {
    string filename = "input_6.txt";
    string base64Str = readFileToString(filename);
    vector<uint8_t> bytes = base64ToBytes(base64Str);
    vector<pair<int, float>> scoreVec;

    // for (int keySize = 2; keySize <= 40; keySize++) {
    //     vector<uint8_t> v1(bytes.begin(), bytes.begin() + keySize);
    //     vector<uint8_t> v2(bytes.begin() + keySize, bytes.begin() + 2*keySize);
    //     vector<uint8_t> v3(bytes.begin() + 2*keySize,bytes.begin() + 3 * keySize);
    //     vector<uint8_t> v4(bytes.begin() + 3 * keySize,bytes.begin() + 4 * keySize);

    //     float x = editDistance(v1, v2) + editDistance(v1, v3) +
    //               editDistance(v1, v4) + editDistance(v2, v3) +
    //               editDistance(v2, v4) + editDistance(v2, v4);
        
    //     scoreVec.push_back({keySize,x/(6*keySize)});
    // }

    // sort(scoreVec.begin(), scoreVec.end(),
    //           [](const pair<int, float>& a, const pair<int, float>& b) {
    //               return a.second < b.second;
    //           });
    // for (const auto& pair : scoreVec) {
    //     cout <<  pair.first << ":   " << fixed << setprecision(2) <<
    //     pair.second << endl;
    // }
    // key length = 29

    string key = "";
    vector<vector<uint8_t>> block(29, vector<uint8_t>(0));

    for (int i = 0; i < bytes.size(); i++) {
      block[i%29].push_back(bytes[i]);
    }

    for (int i = 0; i < 29; i++) {
        vector<pair<uint8_t, float>> scoreMap;
      
        for (int j = 0; j < 127; j++) {
            vector<uint8_t> xored = singleByteXor(block[i], j);
            string answer = bytesToPlaintext(xored);
            scoreMap.push_back({j, special(answer)});
        }

        auto minElement = min_element(
            scoreMap.begin(), scoreMap.end(),
            [](const pair<int, float> &a, const pair<int, float> &b) {
              return a.second < b.second;
            });
        key += minElement->first;
    }

    cout << key << endl;
    cout << bytesToPlaintext(repeatingXor(bytesToPlaintext(bytes), key));
}


