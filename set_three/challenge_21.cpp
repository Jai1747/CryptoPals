#include "library.h"
#include <cstdint>
#include <ios>
#include <iostream>
#include <openssl/cryptoerr_legacy.h>
#include <ostream>
#include <sys/types.h>
#include <openssl/rand.h>
#include <random>
#include <iomanip>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <map>

using namespace std;

#define n 624
#define m 397
#define w 32
#define r 31
#define UMASK (0xFFFFFFFFUL << (r))
#define LMASK (0xFFFFFFFFUL >> (w - r))
#define a 0x9908b0dfUL
#define u 11
#define d 0xFFFFFFFFUL
#define s 7
#define t 15
#define l 18
#define b 0x9d2c5680UL
#define c 0xefc60000UL
#define f 1812433253UL

vector<u_int32_t> state(n);
int mt_index;

void initialize_state(u_int32_t seed) {
    state[0] = seed;
    for (int i = 1; i < n; i++)
      state[i] = f * (state[i - 1] ^ (state[i - 1] >> (w - 2))) + i;
    mt_index = n;
}

void twist() {
  for (int i = 0; i < n; i++) {
    u_int32_t x = (state[i] & UMASK) | (state[(i + 1) % n] & LMASK);
    u_int32_t xA = x >> 1;
    if (x & 0x00000001UL) 
            xA ^= a;
    state[i] = state[(i+m)%n] ^ xA;
    }
    mt_index = 0;
}

u_int32_t temper(u_int32_t x) {
    x ^= ((x >> u) & d);
    x ^= ((x << s) & b);
    x ^= ((x << t) & c);
    x ^= (x >> l);
    return x;
}

u_int32_t random_uint32() {
  if (mt_index >= n)
    twist();
  u_int32_t x = state[mt_index++];
  return temper(x);
}




int main() {
    u_int32_t seed = 1131464071;
    mt19937 mt(seed);

    initialize_state(seed);
    vector<u_int32_t> myOutput;
    vector<u_int32_t> output;

    for (int i = 0; i < 700; ++i) 
        output.push_back(mt());
        
    for(int i=0;i<700;i++)
      myOutput.push_back(random_uint32());

    for (int i = 0; i < 700; i++) {
        cout << myOutput[i] << "\t" << output[i] << "\t" <<(myOutput[i]==output[i] ? "YES" : "NO") << endl;
    }
}
