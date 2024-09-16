#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <sstream>


using namespace std;

const string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int singleHexToInt(char x); // converts a single hex character to decimal
string singleByteToHex(uint8_t byte); // converts single byte to hex string

vector<uint8_t> hexStringToBytes(const string &str); //converts a hex string to a byte array


string bytesToBase64(vector<uint8_t> bytes); // converts a byte array to Base64
string bytesToHexString(vector<uint8_t> bytes); // converts a byte array to hex string
string bytesToPlaintext(vector<uint8_t> bytes); //convert a byte array to a string in plain text


vector<uint8_t> xorBytes(vector<uint8_t> bytes1, vector<uint8_t> bytes2); // xor of two byte arrays
vector<uint8_t>singleByteXor(vector<uint8_t> bytes, int x); // xor of a byte array with a single integer
vector<uint8_t> repeatingXor(string plaintext, string key); // repeating xor of plaintext with key

bool isValidEnglishString(string plaintext); //checks if a string is a valid english string (roughly)