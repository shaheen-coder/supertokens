/*
                                utils.hxx 
                this is comman utils header like fileinfo 


                                                        by @shaheen-coder
*/

#pragma once

#include <string>
// custom libs
#include "libs/expected.hxx"
#include "tokenizer/datatypes.hxx"


/*
                File Utils - for files meta data
*/

struct FileInfo {
    double size;        
    std::string suffix; 
};

auto getReadableFileSize(uintmax_t bytes) -> FileInfo ;

auto getFileInfo(const std::string& filePath) -> tl::expected<FileInfo, std::string>; 


/*
         Testing and tui utils 

*/


void progress_bar(int current, int total, const std::string& title) ;

void file_progress_bar( const char* current, const char* begin, std::size_t total_bytes, std::size_t token_count);

