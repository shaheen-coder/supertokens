/*
                                bpe.cxx 
    *main* bpe file contain trainer , encode , decode implemention
    
                                                      by @shaheen-coder  
*/


//   i/o

#include <ios> 
#include <iostream>

//   ds

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// c styles 

#include <cstddef>

// algorith utils

#include <algorithm>
#include <limits>
#include <iterator>
#ifdef _WIN32
  #define NOMINMAX
  #include <winsock2.h>
  #include <windows.h>
#else
  #include <sys/select.h>
#endif

// files

#include <fstream>

// custom libs

#include "bpe.hxx"
#include "tokenizer/datatypes.hxx" // custom ds for bpe 
#include "utils/utils.hxx" // indenpented utils 

// custom utf8 3rd party 

#include "utils/bpe_utils.hxx" // bpe utils ( utf 8 )
#include "libs/utfcpp/utf8.h"
#include "libs/utfcpp/utf8/cpp17.h"



// bpe constructor with vocab size 
BPE::BPE(int _vocab_size) : vocab_size(_vocab_size) {} ;


/*
                BPE Trainer ( of file streaming )
*/


// streaming type bpe trainer

auto BPE::train_fromfile(const std::string& _filepath, const std::size_t buffer_size) -> void {
    auto file_info = getFileInfo(_filepath);
    if (!file_info) {
        std::cerr << file_info.error() << "\n";
        return ;
    }
    std::ifstream file(_filepath, std::ios::binary);
    if (!file) {
        std::cerr << "[tokenizer trainer] : cant read the file !! \n";
        return ;
    }

    PreTokens tokens;
    utf8_tokens_fromfile(file, tokens);
    if (tokens.empty()){
        std::cerr<<"[tokenizer trainer] unable get tokens from file\n";
        return ;
    }
    tokens.shrink_to_fit();
    file.close();

    train(tokens);

}


/*
                    Main Trainer
*/

auto BPE::train(PreTokens& tokens) -> void {

    // Initial vocab
    std::unordered_set<ui16> initial_vocab;
    for (const auto& s : tokens) {
        if(s != 0 ) initial_vocab.insert(s);
    }
    
    if(initial_vocab.size() > vocab_size){
        std::cerr<<"[trainer] : inital symbols are filled requried ("<<vocab_size<<") size.\n so you should increase the vocab size !!\n";
        return;
    }
    if (vocab_size > 0xFFFFu) {
        std::cerr << "[trainer] : vocab_size exceeds ui16 range (65535).\n";
        return;
    }
    // tokens to id 
    std::unordered_map<ui16, ui16> ltoken_to_id; // local tokens 
    ui32 next_id = 0;
    for (auto s : initial_vocab) ltoken_to_id[s] = next_id++;

    // id to tokens
    
    List id_to_token(next_id);
    
    for (const auto& [cp, id] : ltoken_to_id) {
        std::string temp;
        utf8::append(cp,std::back_inserter(temp));
        id_to_token[id] = std::move(temp);
    }
    
    //  Linked List Node

    
    std::vector<int> heads;

    std::size_t no_pretokens = std::count(tokens.begin(), tokens.end(), 0u);
    heads.reserve(no_pretokens);
     
    Nodes nodes;
    // calcuate the nodes exact size
    nodes.reserve(tokens.size() - no_pretokens);
    

    
    if(tokens.empty() ) return ;
    
    int head = -1;
    int prev = -1;
    for (const auto& pre : tokens) {
        if( pre == 0){
            if(head != -1 ) heads.push_back(head);
            head = -1 ;
            prev = -1;
            continue;
        }
        ui16 id = ltoken_to_id[pre];
        nodes.push_back({id, prev, -1});
        int curr = static_cast<int>(nodes.size()) - 1;
        if (prev != -1) nodes[prev].next = curr;
        if (head == -1) head = curr;
        prev = curr;
    }
    if(head != -1) heads.push_back(head);
    
    // free the tokens memory 
    tokens.clear();
    tokens.shrink_to_fit(); // free the mem

    Pairs pairs;
    flat_vector flat_nodes_pos;
    build_pairs(pairs, heads, nodes,flat_nodes_pos);

    LazyHeap heap;
    for (const auto& [p, info] : pairs) {
        if (info.count > 0) {
            heap.push({p, info.count});
        }
    }
    
    ui32 merge_rank_count = 0;

    // MAIN Merge Loop
    while (next_id < static_cast<ui32>(vocab_size) && !heap.empty()) {
        HeapEntry entry;
        bool found_valid = false;

        while (!heap.empty() && !found_valid) {
            entry = heap.top();
            heap.pop();

            auto it = pairs.find(entry.pair);
            if (it == pairs.end() || it->second.count != entry.count || it->second.count <= 0) {
                continue;
            }
            found_valid = true;
        }

        if (!found_valid) break;

        Pair best_pair = entry.pair;

        merge_rank[best_pair] = merge_rank_count++; 
        // ---- find the pair --- 
        PairInfo& best_pair_info = pairs[best_pair];
        flat_vector pos(
            flat_nodes_pos.begin() + best_pair_info.start,
            flat_nodes_pos.begin() + best_pair_info.end
        );
        best_pair_info.count = 0 ; 
        std::sort(pos.begin(), pos.end());
        pos.erase(std::unique(pos.begin(), pos.end()), pos.end());
    

        ui16 new_id = next_id++;

        // Build merged string
        {
            ui32 a = pair_hi(best_pair);
            ui32 b = pair_lw(best_pair);
            id_to_token.push_back(id_to_token[a] + id_to_token[b]);
        }

        // Merge 
        for (int left : pos) {
            if (! is_alive(nodes[left]) )  continue;

            int right = nodes[left].next;
            if (right == -1 || ! is_alive( nodes[right] )  )  continue;

            if (nodes[left].id != pair_hi(best_pair) || nodes[right].id != pair_lw(best_pair) )  continue;
        
            nodes[left].id = new_id;
            int after = nodes[right].next;
            if (after != -1) nodes[after].prev = left;
            nodes[left].next = after;
            kill(nodes[right]);

            int prev_node = nodes[left].prev;
            if (prev_node != -1) {
                Pair old_prev_pair = make_pair_key(nodes[prev_node].id, pair_hi(best_pair));
                if (auto it_old = pairs.find(old_prev_pair); it_old != pairs.end()) {
                    it_old->second.count = (std::max)(0, it_old->second.count - 1);
                }

                Pair new_prev_pair = make_pair_key(nodes[prev_node].id, new_id);
                auto& ninfo = pairs[new_prev_pair];
                if(ninfo.count == 0 ){
                    ninfo.start = static_cast<int>(flat_nodes_pos.size());
                    ninfo.end = ninfo.start;
                    
                }
                flat_nodes_pos.push_back(prev_node);
                ninfo.end++;                
                ninfo.count++;

                heap.push({new_prev_pair,ninfo.count});
            }

            if (after != -1) {
                Pair old_after_pair = make_pair_key(pair_lw(best_pair), nodes[after].id);
                if (auto it_old = pairs.find(old_after_pair); it_old != pairs.end()) {
                    it_old->second.count = (std::max)(0, it_old->second.count - 1);
                }

                Pair new_after_pair = make_pair_key(new_id, nodes[after].id);
                auto& ninfo = pairs[new_after_pair];
                if(ninfo.count == 0 ){
                    ninfo.start = static_cast<int>(flat_nodes_pos.size());
                    ninfo.end = ninfo.start;
                    
                }
                flat_nodes_pos.push_back(left);
                ninfo.end++;                
                ninfo.count++;
                heap.push({new_after_pair,ninfo.count});
            }
        }
        if (heap.size() > 3 * pairs.size()) {
            LazyHeap fresh;
            for (auto& [p, info] : pairs)
                if (info.count > 0) fresh.push({p, info.count});
            heap = std::move(fresh);
        }

        progress_bar(next_id, vocab_size, "BPE Trainer");

    }
    std::cout<<"\n";

    vocab = std::move(id_to_token);

    // token to id
    token_to_id.clear();
    token_to_id.reserve(vocab.size());
    for (ui32 i = 0; i < static_cast<ui32>(vocab.size()); ++i) token_to_id[vocab[i]] = i;
    
}



/*
                    Inference 
                        encode and decode functions 
        
*/


auto BPE::encode(const std::string& text) -> PreTokens {

    if(vocab.empty()) {
        std::cerr<<"[BPE::encode] model isnt trained yet !\n";
        return {};
    }

    PreTokens utfids;
    utf8::utf8to32(text.begin(), text.end(),std::back_inserter(utfids));

    PreTokens ids;
        
    for(auto& id : utfids ){
        if( id == 32) id = 288;      
        std::string temp;
        utf8::append(id, std::back_inserter(temp));
        auto it = token_to_id.find(temp);
        if( it != token_to_id.end()) ids.push_back(it->second);
        
    }        

    if( ids.empty() ) { std::cerr<<"[error] empty \n"; return {}; } 


    
    
    while ( true ){

        ui32 best_rank = std::numeric_limits<ui32>::max();
        std::size_t best_pos = std::string::npos;

        for(std::size_t i = 0; i + 1 < ids.size(); ++i){

            Pair pair = make_pair_key(ids[i], ids[i+1]);

            auto it = merge_rank.find(pair);
            if(it != merge_rank.end()){
                if( it->second < best_rank ) {
                    best_rank = it->second;
                    best_pos = i;
                }
            }
        }

        if(best_pos == std::string::npos) break;
        

        ui32 a = ids[best_pos];
        ui32 b = ids[best_pos+1];

        std::string merged = vocab[a] + vocab[b] ;

        ui32 mids = token_to_id.at(merged);

        ids[best_pos] = mids;
        ids.erase(ids.begin() + static_cast<std::ptrdiff_t>(best_pos + 1 ));
         
    }

    return ids;
}


auto BPE::decode(const PreTokens& ids) -> std::string {

    if (vocab.empty()) {
        std::cerr << "[BPE::decode] model isn't trained yet!\n";
        return {};
    }

    std::string out;

    for (const auto id : ids) {
        if (id < static_cast<ui32>(vocab.size())) {
            out += vocab[id];
            out += ' ';
        } else {
            // id outside vocab — emit UTF-8 replacement character U+FFFD
            out += "\xEF\xBF\xBD";
        }
    }

    return out;
}
