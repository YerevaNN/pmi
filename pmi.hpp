#ifndef pmi_hpp
#define pmi_hpp

#include <istream>
#include <cstddef>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <functional>

#ifdef VERBOSE
#define report(x, ...) fprintf(stderr, x, ##__VA_ARGS__); fflush(stderr);
#else
#define report(x, ...)
#endif
typedef  __uint16_t uint16;
typedef  __uint32_t uint32;
typedef  __uint64_t uint64;
typedef __uint128_t uint128;

typedef uint32 occurences;
typedef uint64 size;
typedef uint64 word;
typedef uint64 ngram;
typedef uint128 pair;

const word emptyWord = ((word)1 << 21) - 1;

const size wordMaxLength = 64;
const size defaultWindowSize = 8;

ngram hash(word = emptyWord, word = emptyWord, word = emptyWord);

template<>
struct std::hash< ::pair > {
    size_t operator() (::pair x) const;
};

class model {
private:
    bool locked;
    void push(ngram, ngram);
    template< class ForwardIterator >
    void processWindow(const ForwardIterator&, const ForwardIterator&);
    word readWord(std::istream&);
public:
    std::unordered_map< std::string, word > dictionary;
    std::vector< std::string > words;
    std::unordered_map< pair, occurences > unlockedCount;
    std::vector< std::pair< pair, occurences > > lockedCount;
public:
    model();
    void clear();
    bool isLocked() const;
    void lock();
    void unlock();
    word lookup(const std::string&);
    void dump (std::ostream&);
    void load (std::istream&);
    void train (std::istream&, size_t = defaultWindowSize);
    occurences count(ngram, ngram = emptyWord) const;
};

#endif /* pmi_hpp */
