/*
                                datatypes.hxx 
                this is a comman or bpe's models custom stl data types 

                                                            by @shaheen-coder
*/

#pragma once


#include <limits>
#include <queue>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <string>
/*
                    Comman DataType 
*/

using List = std::vector<std::string>;

using ui32 = std::uint32_t;

using ui16 = std::uint16_t;

using ui8 = std::uint8_t;

/*
                    BPE ( data types )

*/

using PreTokens = std::vector<ui16>;


// pairs

using Pair = ui32;

inline auto make_pair_key(ui16 a, ui16 b) -> Pair {
    return (ui32(a) << 16 ) | b;
}

inline auto pair_hi(Pair p) -> ui16 { return  static_cast<ui16>(p >> 16); }
inline auto pair_lw(Pair p) -> ui16 { return static_cast<ui16>(p & 0xFFFFU); }

using flat_vector = std::vector<int> ;


struct PairInfo{
    int count = 0;
    int start ;
    int end;     
};


using Pairs = std::unordered_map<Pair,PairInfo>;

//          lazy heap nodes 
struct Node {
    ui16 id;
    int prev ; int next ;
};
constexpr ui16 DEAD = (std::numeric_limits<ui16>::max)();
inline bool is_alive(const Node& n) { return n.id != DEAD; }
inline void kill(Node& n) { n.id = DEAD; }

using Nodes = std::vector<Node>;


//  lazy Heap

struct HeapEntry{
    
    Pair pair;
    int count;

    bool operator<(const HeapEntry& other) const {
        return count < other.count ; 
    }
};

using LazyHeap = std::priority_queue<HeapEntry>;
