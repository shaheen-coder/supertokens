/*
                                bpe.hxx  
    *main* bpe header file contain trainer , encode , decode headers 
    
                                                      by @shaheen-coder  
*/
#pragma once

// stl libs
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

// custom bpe datatypes
#include "datatypes.hxx"

class BPE{
    public:
        // members 
        int vocab_size;
        std::vector<std::string> vocab;
        std::unordered_map<Pair, ui32> merge_rank;
        std::unordered_map<std::string, ui32> token_to_id;
        // methods 
        explicit BPE(int _vocab_size);

        auto train_fromfile(const std::string& _filepath, std::size_t buffer_size) -> void ;
        
        auto encode(const std::string& text) -> PreTokens ;

        auto decode(const PreTokens& ids) -> std::string;        
        
    private:
        auto list_to_utf8(const std::vector<std::string>& texts, PreTokens& tokens) -> void ;  
        
        auto train(PreTokens& tokens) -> void ; 
};
