#include <iostream>
#include <string>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <deque>
#include <algorithm>
#include <stddef.h>
#include "pmi.hpp"
#include "train.hpp"

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
std::vector<ngram> getNgrams(const T& begin, const T& end) {
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
    
    return ngrams;
}

word model::lookup(const std::string& s) {
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

void model::push(ngram x, ngram y) {
    if(x > y) {
        std::swap(x, y);
    }
    ++unlockedCount[(pair(x) << (3 * 21)) | pair(y)];
    return;
}

template <class ForwardIterator>
void model::processWindow(const ForwardIterator& begin, const ForwardIterator& end) {
    auto ngrams = getNgrams(begin, end);
    for(auto i : ngrams) {
        for(auto j : ngrams) {
            push(i, j);
        }
    }
    return;
}

word model::readWord(std::istream& in) {
    std::string s;
    in >> s;
    return lookup(s);
}

void model::train(std::istream& in, size_t windowSize) {
    clear();
    
    std::deque<word> window;
    
    std::string s;
    
    while(window.size() != windowSize) {
        window.push_back(readWord(in));
    }
    
    size counter = 0;
    
    while(true) {
        processWindow(window.begin(), window.end());
        
        word add = readWord(in);
        if(add == emptyWord) {
            break;
        }
        window.push_back(add);
        window.pop_front();
        
        ++counter;
        if(counter % 731 == 0)
            report("\r%lld\t\t", counter);
    }
    std::cerr << std::endl;

    return;
}