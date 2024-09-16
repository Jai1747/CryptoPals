#include <cstdint>
#include <iostream>
#include "library.h"





int main() {
    string hexStr =
      "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
    

    vector<uint8_t> bytes = hexStringToBytes(hexStr);

    for (int i = 0; i < 256; i++) {
        vector<uint8_t> xored = singleByteXor(bytes, i);
        string answer = bytesToPlaintext(xored);

       if (isValidEnglishString(answer))
            cout << answer << endl;
    }

    

}


