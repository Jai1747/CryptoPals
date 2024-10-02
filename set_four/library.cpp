#include "library.h"

vector<uint8_t> plaintextToBytes(const string &str) {
    vector<uint8_t> output;
    int n = str.length();

    for (int i = 0; i < n; i += 1) {  
        output.push_back(static_cast<uint8_t>(str[i]));
    }
    return output;
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

    EVP_CIPHER_CTX_set_padding(ctx, 0); // Disable padding if your data is already padded correctly

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

void aes_128_ecb_encrypt(const string& key, const string& plaintext, string& ciphertext) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw runtime_error("Error initializing cipher context");
    }

    // Initialize the encryption operation
    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, reinterpret_cast<const unsigned char*>(key.data()), NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("Error initializing AES-128-ECB encryption");
    }

    // Optional: Disable padding if you are handling padding manually
    EVP_CIPHER_CTX_set_padding(ctx, 0); 

    int len = 0;
    int ciphertext_len = 0;
    ciphertext.resize(plaintext.size() + AES_BLOCK_SIZE); // Allocate space for ciphertext

    // Perform encryption
    if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(&ciphertext[0]), &len,
                          reinterpret_cast<const unsigned char*>(plaintext.data()), plaintext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("Error during AES-128-ECB encryption");
    }
    ciphertext_len = len;

    // Finalize encryption
    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&ciphertext[0]) + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("Error finalizing AES-128-ECB encryption");
    }
    ciphertext_len += len;

    // Resize to actual ciphertext length
    ciphertext.resize(ciphertext_len);
    

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
}

vector<uint8_t> pkcs7(string plaintext, int pad_length) {
    vector<uint8_t> answer = plaintextToBytes(plaintext);

    int n = plaintext.size();
    int remainder = pad_length - n % pad_length;
    if (remainder == pad_length)
      return answer;

    for(int i=0; i<remainder;i++)
      answer.push_back(static_cast<uint8_t>(remainder));
    
    return answer;
}

string bytesToPlaintext(vector<uint8_t> bytes) {
  string answer = "";
  for (uint8_t byte : bytes)
    answer += byte;
  return answer;
}

int singleHexToInt(char x) {
    if (x >= 'a' && x <= 'f')  
        return x - 'a' + 10;
    if (x >= '0' && x <= '9')
        return x - '0';
    return -1;
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

string byteToHex(uint8_t byte) {
  stringstream ss;
  ss << hex << setw(2) << setfill('0') << static_cast<int>(byte) << dec;
  return ss.str();
}


string bytesToHexString(vector<uint8_t> bytes){
    int n = bytes.size();
    string answer = "";
    for (uint8_t byte : bytes)
        answer += byteToHex(byte);
    return answer;
}

std::string base64_encode(const std::string &in) {
    BIO *bio, *b64;
    BUF_MEM *buf_mem;
    std::string out;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, in.data(), in.size());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &buf_mem);

    out.assign(buf_mem->data, buf_mem->length - 1);
    BIO_free_all(bio);

    return out;
}

string xorPlaintext(string a, string b) {
    string answer;
    answer.resize(a.length());
    for (int i = 0; i < a.length(); i++) {
      answer[i] = static_cast<uint>(a[i] ^ b[i]);
    }
    return answer;
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