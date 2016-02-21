#include <iostream>
#include "train.hpp"

int main(int argc, char** argv){
    model m;
    
    

    if(argc == 1) {
        m.train(std::cin);
        m.dump(std::cout);
    }
    else {
        m.load(std::cin);
    }
    
    ngram they_could = hash(m.lookup("they"),
                            m.lookup("could"));
    ngram anywhere = hash(m.lookup("anywhere"));
	
    int they_could_count = m.count(they_could, they_could);
    int anywhere_count = m.count(anywhere, anywhere);
    int they_could__anywhere_count = m.count(they_could, anywhere);
    int they_could__anywhere_count_ = m.count(anywhere, they_could);
    
    std::cerr << they_could_count << " " << anywhere_count << " " << they_could__anywhere_count << " " << they_could__anywhere_count_ << std::endl;
    
    return 0;
}
