#include "library.h"
#include <bitset>
#include <cstdint>
#include <ios>
#include <iostream>
#include <openssl/cryptoerr_legacy.h>
#include <ostream>
#include <stdexcept>
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


struct MT19937 {
  vector<u_int32_t> state;
  int mt_index;

  MT19937() : state(n), mt_index(n) {}
};

void initialize_state(u_int32_t seed, MT19937& obj) {
    obj.state[0] = seed;
    for (int i = 1; i < n; i++)
      obj.state[i] = f * (obj.state[i - 1] ^ (obj.state[i - 1] >> (w - 2))) + i;
    obj.mt_index = n;
}

void twist(MT19937& obj) {
  for (int i = 0; i < n; i++) {
    u_int32_t x = (obj.state[i] & UMASK) | (obj.state[(i + 1) % n] & LMASK);
    u_int32_t xA = x >> 1;
    if (x & 0x00000001UL) 
            xA ^= a;
    obj.state[i] = obj.state[(i+m)%n] ^ xA;
    }
    obj.mt_index = 0;
}

u_int32_t temper(u_int32_t x) {
    x ^= ((x >> u) & d);
    x ^= ((x << s) & b);
    x ^= ((x << t) & c);
    x ^= (x >> l);
    return x;
}

u_int32_t random_uint32(MT19937& obj) {
  if (obj.mt_index >= n)
    twist(obj);
  u_int32_t x = obj.state[obj.mt_index++];
  return temper(x);
}


u_int32_t inv_right(u_int32_t x, int num_shift) {
    bitset<32> bits(x);
    bitset<32> ans(x);

    for (int i = 31-num_shift; i>=0; i--)
      ans[i] = ans[i+num_shift] ^ bits[i];

    uint32_t answer = static_cast<uint32_t>(ans.to_ulong());
    return answer;
}

u_int32_t inv_left(u_int32_t x, int num_shift, u_int32_t magic_number) {
    bitset<32> bits(x);
    bitset<32> magic_bits(magic_number);
    bitset<32> ans(x);

    for (int i = num_shift; i < 32; i++)
      ans[i] = bits[i] ^ (ans[i - num_shift] & magic_bits[i]);
    
    uint32_t answer = static_cast<uint32_t>(ans.to_ulong());
    return answer;
}

u_int32_t untemper(u_int32_t x) {
    x = inv_right(x, l);
    x = inv_left(x, t, c);
    x = inv_left(x, s, b);
    x = inv_right(x, u);
    return x;
}


int main() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<u_int32_t> dist(0, 0xFFFFFFFF);
    u_int32_t seed = dist(gen);

    MT19937 obj;
    initialize_state(seed, obj);
    vector<u_int32_t> output;

    for (int i = 0; i < n; i++)
        output.push_back(random_uint32(obj));

    MT19937 clone_obj;
    clone_obj.mt_index = n;
    for (int i = 0; i < n; i++)
        clone_obj.state[i] = untemper(output[i]);

    for (int i = 0; i < 1000; i++) {
        u_int32_t actual = random_uint32(obj);
        u_int32_t cloned = random_uint32(clone_obj);
        cout << actual << " " <<  cloned << (actual==cloned ? " Yes" : " No") << endl;
    }

   
}

    
