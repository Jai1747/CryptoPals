#include "library.h"
#include <cstdint>
#include <ios>
#include <sys/types.h>
#include <openssl/rand.h>
#include <iomanip>
#include <openssl/err.h>

using namespace std;

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



int main() {
    string key = "YELLOW SUBMARINE";
    string filename = "input_10.txt";
    string base64Ciphertext = readFileToString(filename); // Replace with actual Base64 ciphertext
    string ciphertext = base64_decode(base64Ciphertext); // Decode Base64 to binary
    string iv = string(16, '\0');

    string plaintext = aes_128_cbc_decrypt(key, iv, ciphertext);
    cout << plaintext;
}
