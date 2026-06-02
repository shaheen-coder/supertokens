/*
                        bpe_utils.cxx 
            this is a utils implemention for bpe model like utf8 converter

                                                        by @shaheen-coder
*/

// custom libs
#include "bpe_utils.hxx"
#include "libs/utfcpp/utf8.h"
#include "tokenizer/datatypes.hxx"
#include "utils/utils.hxx"


#include <cstddef>
#include <fstream>
#include <iostream>
#include <vector>

auto build_pairs(Pairs& pairs,const std::vector<int>& heads, const Nodes& nodes, flat_vector& position) -> void {

    // build just node 
    for(const auto h : heads){ 
        int curr = h;
        while(curr != -1 && nodes[curr].next != -1){
            ui16 a = nodes[curr].id;
            ui16 b = nodes[nodes[curr].next].id;
            Pair pair = make_pair_key(a, b);
            auto& info = pairs[pair];
            info.count++;
            curr = nodes[curr].next;
        }
    }
    // compute size

    {
        int offset = 0;

        for(auto& [pair, info] : pairs){
            info.start = offset ;
            info.end = offset ;

            offset += info.count ;
             
        }
        position.resize(offset);

    }

    
    for(const auto h : heads){ 
        int curr = h;
        while(curr != -1 && nodes[curr].next != -1){
            ui16 a = nodes[curr].id;
            ui16 b = nodes[nodes[curr].next].id;
            Pair pair = make_pair_key(a, b);
            auto& info = pairs[pair];
            position[info.end++] = curr ; 
            curr = nodes[curr].next;
        }
    }


    
}

/*
            text file ( utf8 ) stream 
*/
auto utf8_tokens_fromfile(std::ifstream& file, PreTokens& tokens) -> void {

    file.seekg(0, std::ios::end);
    const std::size_t file_size = static_cast<std::size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    std::vector<char> buf(file_size);
    file.read(buf.data(), file_size);

    tokens.reserve(tokens.size() + file_size * 2 + 1);

    const char* it  = buf.data();
    const char* end = buf.data() + file_size;

    const char* begin = buf.data();
    std::size_t last_update = 0;


    while (it != end) {
        const ui16 cp = utf8::next(it, end); 

        if (cp == 32 || cp == 10 || cp == 9 || cp == 13) {
            const ui16 pair[2] = {0, 288};
            tokens.insert(tokens.end(), pair, pair + 2);
        } else {
            tokens.push_back(cp);
        }
        const std::size_t processed =
        static_cast<std::size_t>(it - begin);

        if (processed - last_update >= (1 << 20)) { // every 1 MB
            file_progress_bar(
            it,
            begin,
            file_size,
            tokens.size());

            last_update = processed;
        }
    }

    file_progress_bar(end, begin, file_size, tokens.size());
    std::cout << '\n';        

    tokens.push_back(0);
}
