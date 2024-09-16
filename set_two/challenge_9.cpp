#include "library.h"
#include <cstdint>
#include <sys/types.h>

using namespace std;


int main() {
    string plaintext = "YELLOW SUBMARINE";
    int pad_length = 20;
    vector<uint8_t> answer = pkcs7(plaintext, pad_length);

    for (int i = 0; i < answer.size(); i++)
      cout << hex << setw(2) << setfill('0') << static_cast<int> (answer[i]) << " ";
}
