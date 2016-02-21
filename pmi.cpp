#include <algorithm>
#include "pmi.hpp"

ngram hash(word a, word b, word c) {
    if (a > c) {
        std::swap(a, c);
    }
    if (a > b) {
        std::swap(a, b);
    }
    if (b > c) {
        std::swap(b, c);
    }

    return a | (b << 21) | (c << 42);
}

size_t std::hash< ::pair >::operator()(::pair x) const {
    ngram higher = (x << 0) >> 64;
    ngram lower = (x << 64) >> 64;
    return higher ^ (lower << 32) ^ (lower >> 32);
}

model::model() {
    clear();
}

void model::clear() {
    locked = false;
    dictionary.clear();
    words.clear();
    unlockedCount.clear();
    lockedCount.clear();
}

bool model::isLocked() const {
    return locked;
}

occurences model::count(ngram x, ngram y) const {
    if(x > y) {
        std::swap(x, y);
    }
    if(locked) {
        auto key = std::make_pair((pair(x) << (3 * 21)) | pair(y), occurences(0));
        auto I = std::lower_bound(lockedCount.begin(), lockedCount.end(), key);
        if(I == lockedCount.end() ||
           I->first != key.first) {
            report("%d %d %llx %llx %llx %llx\n", I == lockedCount.end() , I->first != key.first,
                   uint64(I->first >> 64),
                   uint64((I->first << 64) >> 64),
                   uint64(key.first >> 64),
                   uint64((key.first << 64) >> 64)
                   );
            return 0;
        }
        return I->second;
    } else {
        return unlockedCount.at((pair(x) << (3 * 21)) | pair(y));
    }
}

void model::lock() {
    if(locked) {
        return;
    }
    report("Locking...");
    lockedCount.clear();
    while(!unlockedCount.empty()) {
        lockedCount.push_back(*unlockedCount.begin());
        unlockedCount.erase(unlockedCount.begin());
    }
    report("Indexing...");
    std::sort(lockedCount.begin(), lockedCount.end());
    locked = true;
    report("Done\n");
    return;
}

void model::unlock() {
    if(!locked) {
        return;
    }
    report("Unlocking...");
    while(!lockedCount.empty()) {
        unlockedCount.insert(lockedCount.back());
        lockedCount.pop_back();
    }
    locked = false;
    report("Done\n");
    return;
}

void model::dump(std::ostream& out){
    lock();
    
    report("Dumping...");
    report("\b\b\b Dictionary...");
    size dictionarySize = words.size();
    out.write((const char*)&dictionarySize, sizeof(dictionarySize));
    for(auto w : words) {
        std::string s = w;
        s.resize(wordMaxLength, 0);
        out.write(s.c_str(), wordMaxLength);
    }
    report("\b\b\b Matrix...");
    size matrixSize = lockedCount.size();
    out.write((const char *)&matrixSize, sizeof(matrixSize));
    for(auto i : lockedCount) {
        pair key = i.first;
        out.write((const char*)&key, sizeof(key));
        occurences val = i.second;
        out.write((const char*)&val, sizeof(val));
    }
    report("Done\n");
    return;
}

void model::load(std::istream& in) {
    clear();
    lock();
    
    report("Loading...");
    
    size dictionarySize;
    in.read((char*)&dictionarySize, sizeof(dictionarySize));
    for(word i = 0; i < dictionarySize; ++i) {
        char s[wordMaxLength];
        in.read(s, wordMaxLength);
        std::string w(s);
        dictionary[w] = i;
        words.push_back(w);
    }
    size matrixSize;
    in.read((char*)&matrixSize, sizeof(matrixSize));
    for(size i = 0; i < matrixSize; ++i) {
        pair key;
        in.read((char*)&key, sizeof(key));
        occurences val;
        in.read((char*)&val, sizeof(val));
        lockedCount.push_back(std::make_pair(key, val));
    }
    report("Done\n");
    return;
}