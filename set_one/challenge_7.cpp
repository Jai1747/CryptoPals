#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/bio.h>
#include <openssl/aes.h>
#include <cstring>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <bitset>
#include <algorithm>
#include "library.h"

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

void aes_128_ecb_decrypt(const string& key, const string& ciphertext, string& plaintext) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw runtime_error("Error initializing cipher context");
    }

    // Initialize the decryption operation
    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, reinterpret_cast<const unsigned char*>(key.data()), NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("Error initializing AES-128-ECB decryption");
    }

    //EVP_CIPHER_CTX_set_padding(ctx, 0); // Disable padding if your data is already padded correctly

    int len = 0;
    int plaintext_len = 0;
    plaintext.resize(ciphertext.size()); // Allocate space for plaintext

    // Perform decryption
    if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(&plaintext[0]), &len,
                          reinterpret_cast<const unsigned char*>(ciphertext.data()), ciphertext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("Error during AES-128-ECB decryption");
    }
    plaintext_len = len;

    // Finalize decryption
    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&plaintext[0]) + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("Error finalizing AES-128-ECB decryption");
    }
    plaintext_len += len;

    // Resize to actual plaintext length
    plaintext.resize(plaintext_len);

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
}


string base64_decode(const string& encoded) {
    BIO *bio, *b64;
    char* buffer = (char*)malloc(encoded.size());
    memset(buffer, 0, encoded.size());

    bio = BIO_new_mem_buf(encoded.data(), encoded.size());
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    // Disable newlines
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    int decodedLength = BIO_read(bio, buffer, encoded.size());
    string decoded(buffer, decodedLength);

    BIO_free_all(bio);
    free(buffer);

    return decoded;
}


int main() {
    string key = "YELLOW SUBMARINE";
    string filename = "input_7.txt";
    string base64Ciphertext = readFileToString(filename); // Replace with actual Base64 ciphertext
    string ciphertext = base64_decode(base64Ciphertext);; // Decode Base64 to binary

    string plaintext;
    aes_128_ecb_decrypt(key, ciphertext, plaintext);

    cout << "Decrypted message: " << plaintext << endl;
    
    return 0;
}
