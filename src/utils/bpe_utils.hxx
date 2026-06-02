/*
                        bpe_utils.hxx
            this is a utils headers for bpe model like utf8 converter

                                                        by @shaheen-coder
*/

#pragma once

// custom uitls
#include "tokenizer/datatypes.hxx"



auto build_pairs(Pairs& pairs,const std::vector<int>& heads, const Nodes& nodes,flat_vector& position) -> void ;

auto utf8_tokens_fromfile(std::ifstream& file, PreTokens& tokens) -> void ;

