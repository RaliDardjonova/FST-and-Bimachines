#include "../lib/construct-fst.h"
#include "../lib/bimachine.h"
#include<iostream>
#include<unordered_set>
#include<string>
#include<unordered_map>
#include<vector>

int main(int argc, char *argv[]){
    std::string regex = argv[1];
    Transducer t = constructFST(regex);

    if(testFunctionality(t)){
        Bimachine bm = FSTtoBM(t);

    /*    printf("left edges:\n");
        b.leftA.printEdges();
        printf("right edges:\n");
        b.rightA.printEdges();
        printf("func:\n");
        b.printFunc();
    */
        std::string word;
        std::cout<<"Enter a word to process"<<std::endl;
        std::cin>>word;
        bool isInDomain;
        std::string result = bm.processWord(word, isInDomain);
        if(isInDomain){
            std::cout<<"The translation is: "<<result<<std::endl;
        } else {
            std::cout<<"The word is not in the domain"<<std::endl;
        }
    }
}
