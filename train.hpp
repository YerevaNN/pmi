#ifndef train_hpp
#define train_hpp

#include <istream>

#define report(x, ...) fprintf(stderr, x, ##__VA_ARGS__); fflush(stderr);

typedef  __uint16_t uint16;
typedef  __uint32_t uint32;
typedef  __uint64_t uint64;
typedef __uint128_t uint128;

typedef uint32 occurences;
typedef uint64 word;
typedef uint64 ngram;
typedef uint128 pair;

const word emptyWord = ((word)1 << 21) - 1;

const size_t defaultWindowSize = 8;

void train(std::istream& in);

#endif /* train_hpp */
