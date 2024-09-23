#include "library.h"
#include <cstdint>
#include <sys/types.h>

using namespace std;

string removePadding(string str) {
    int n = str.length();
    if (n % 16 != 0)
      return "FALSE";

    int last = static_cast<int>(static_cast<unsigned char>(str[n - 1]));
    if (last > 0 && last < 16) {
      for (int j = 1; j <= last; j++) {
        if (str[n - j] != str[n - 1])
          return "FALSE";
      }
      return str.erase(n - last);
    }
    return "FALSE";
}

int main() {
  string s1 = "ICE ICE BABY\x04\x04\x04\x04";
  string s2 = "ICE ICE BABY\x05\x05\x05\x05";
  string s3 = "ICE ICE BABY\x01\x02\x03\x04";

  cout << removePadding(s1) << endl;
  cout << removePadding(s2) << endl;
  cout << removePadding(s3) << endl;
}
