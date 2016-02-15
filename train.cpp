#include <iostream>
#include <string>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <deque>
#include <algorithm>
#include <stddef.h>
#include "train.hpp"

inline ngram hash(word a = emptyWord, word b = emptyWord, word c = emptyWord) {
    if (a > c) {
        std::swap(a, c);
    }
    if (a > b) {
        std::swap(a, b);
    }
    if (b > c) {
        std::swap(b, c);
    }
    return a | (b << 16) | (c << 32);
}

template <>
struct std::hash<::pair> {
    inline size_t operator () (::pair x) const {
        return (x >> 64) ^ (x & 0xffffffffffff);
    }
};

std::unordered_map< std::string, word > dictionary;
std::vector< std::string > words;
std::unordered_map< pair, occurences > count;

word lookup(const std::string& s) {
    if(s.empty()) {
        return emptyWord;
    }
    auto I = dictionary.find(s);
    if(I != dictionary.end()) {
        return I->second;
    }
    word index = words.size();
    words.push_back(s);
    return dictionary[s] = index;
}

void push(ngram x, ngram y) {
    if(x > y) {
        std::swap(x, y);
    }
    ++count[(x << 3 * 21) | y];
    return;
}

template <class T>
std::vector<ngram> getUnigrams(const T& begin, const T& end) {
    std::unordered_set<ngram> ngrams;
    for(auto I = begin; I != end; ++I) {
        ngrams.insert(hash(*I));
    }
    return std::vector<ngram> (ngrams.begin(), ngrams.end());
}

template <class T>
std::vector<ngram> getBigrams(const T& begin, const T& end) {
    std::unordered_set<ngram> ngrams;
    for(auto I = begin, J = std::next(I); J != end; ++I, ++J) {
        ngrams.insert(hash(*I, *J));
    }
    return std::vector<ngram> (ngrams.begin(), ngrams.end());
}

template <class T>
std::vector<ngram> getTrigrams(const T& begin, const T& end) {
    std::unordered_set<ngram> ngrams;
    for(auto I = begin, J = std::next(I), K = std::next(J); K != end; ++I, ++J, ++K) {
        ngrams.insert(hash(*I, *J, *K));
    }
    return std::vector<ngram> (ngrams.begin(), ngrams.end());
}

template <class T>
std::vector<ngram> getNgrams(const T& begin, const T& end) { // Time - O(WlogW)
    std::vector<ngram> ngrams;
    
    if(std::distance(begin, end) >= 1) {
        auto unigrams = getUnigrams(begin, end);
        ngrams.insert(ngrams.end(), unigrams.begin(), unigrams.end());
    }
    if(std::distance(begin, end) >= 2) {
        auto bigrams = getBigrams(begin, end);
        ngrams.insert(ngrams.end(), bigrams.begin(), bigrams.end());
    }
    if(std::distance(begin, end) >= 3) {
        auto trigrams = getTrigrams(begin, end);
        ngrams.insert(ngrams.end(), trigrams.begin(), trigrams.end());
    }
    
    return ngrams; // Return size - O(W)
}

template <class ForwardIterator>
void processWindow(const ForwardIterator& begin, const ForwardIterator& end) { // Time - O(W^2log(TW^2))
    auto ngrams = getNgrams(begin, end); // Time - O(WlogW), size - O(W)
    for(auto i : ngrams) {
        for(auto j : ngrams) {
            push(i, j);
        }
    }
    return;
}

inline word readWord(std::istream& in) {
    std::string s;
    in >> s;
    return lookup(s);
}

void train(std::istream& in, size_t windowSize = defaultWindowSize) {
    std::deque<word> window;
    
    std::string s;
    
    while(window.size() != windowSize) {
        window.push_back(readWord(in));
    }
    
    while(true) {
        processWindow(window.begin(), window.end());
        
        word add = readWord(in);
        if(add == emptyWord) {
            break;
        }
        window.push_back(add);
        window.pop_front();
    }

    return;
}