#include "library.h"
#include <cstdint>
#include <ios>
#include <sys/types.h>
#include <openssl/rand.h>
#include <iomanip>
#include <openssl/err.h>

using namespace std;

string xorPlaintext(string a, string b) {
    string answer;
    answer.resize(a.length());
    for (int i = 0; i < a.length(); i++) {
      answer[i] = static_cast<uint>(a[i] ^ b[i]);
    }
    return answer;
}

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

string aes_128_cbc_encrypt(string key, string iv, string plaintext) {
    string ciphertext = "";
    string new_plaintext = bytesToPlaintext(pkcs7(plaintext, 16));

    for (int i = 0; i < new_plaintext.length(); i += 16) {
        string temp_ciphertext;
        string temp_plaintext = xorPlaintext(iv, new_plaintext.substr(i, 16));
        
        aes_128_ecb_encrypt(key, temp_plaintext, temp_ciphertext);
        
        iv = temp_ciphertext;
        ciphertext += temp_ciphertext;
    }
    return ciphertext;
}

string aes_128_cbc_decrypt(string key, string iv, string ciphertext) {
    string plaintext = "";

    for (int i = 0; i < ciphertext.length(); i += iv.size()) {
        string temp_plaintext = "";
        string temp_ciphertext = ciphertext.substr(i, iv.size());

        aes_128_ecb_decrypt(key, temp_ciphertext, temp_plaintext);
        plaintext += xorPlaintext(iv, temp_plaintext);
        iv = temp_ciphertext;
    }
    return plaintext;
}

int main() {
    string key = "YELLOW SUBMARINE";
    string filename = "input_10.txt";
    string base64Ciphertext = readFileToString(filename); // Replace with actual Base64 ciphertext
    string ciphertext = base64_decode(base64Ciphertext); // Decode Base64 to binary
    string iv = string(16, '\0');

    string plaintext = aes_128_cbc_decrypt(key, iv, ciphertext);
    cout << plaintext;
}
