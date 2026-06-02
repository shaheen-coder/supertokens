/*
                                    utils.cxx
                    this is implemention of utils 

                                                    by @shaheen-coder

*/
#include <string>
#include <filesystem>
#include <iostream>
// custom libs
#include "utils.hxx"
#include "libs/expected.hxx"


namespace fs = std::filesystem;

// Convert bytes to human-readable format
auto getReadableFileSize(uintmax_t bytes) -> FileInfo {
    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0; 
    double size = static_cast<double>(bytes);

    while (size >= 1024 && i < 4) {
        size /= 1024;
        ++i;
    }

    return {size, suffixes[i]};
}

// Get file info
auto getFileInfo(const std::string& filePath) -> tl::expected<FileInfo, std::string> {
    if (!fs::exists(filePath)) {
        return tl::unexpected("File does not exist!");
    }

    if (!fs::is_regular_file(filePath)) {
        return tl::unexpected("Not a regular file!");
    }

    uintmax_t fileSize = fs::file_size(filePath);
    return getReadableFileSize(fileSize);
}

void progress_log(const std::string& msg) {
    std::cout << '\r' << std::string(120, ' ') << '\r'; 
    std::cout << "[LOG] " << msg << '\n';
}

void progress_bar(int current, int total, const std::string& title = "UTF-8 Tokenization")
{
    constexpr int bar_width = 50;

    float progress = static_cast<float>(current) / total;
    int pos = static_cast<int>(bar_width * progress);

    std::cout << '\r';

    // Title
    std::cout << title << " | ";

    // Progress bar
    std::cout << "[";

    for (int i = 0; i < bar_width; ++i)
    {
        if (i < pos)
            std::cout << '=';
        else
            std::cout << ' ';
    }

    std::cout << "] ";

    // Stats
    std::cout << std::setw(3)
              << static_cast<int>(progress * 100.0f)
              << "% "
              << "(" << current << "/" << total << ")";

    std::cout.flush();
}


void file_progress_bar(
    const char* current,
    const char* begin,
    std::size_t total_bytes,
    std::size_t token_count)
{
    constexpr int width = 50;

    const std::size_t processed =
        static_cast<std::size_t>(current - begin);

    const float progress =
        static_cast<float>(processed) / total_bytes;

    const int filled =
        static_cast<int>(progress * width);

    std::cout << '\r';

    std::cout << "Pre-Tokenizing  | [";

    for (int i = 0; i < width; ++i) {
        if (i < filled)
            std::cout << '=';
        else if (i == filled)
            std::cout << '>';
        else
            std::cout << ' ';
    }

    std::cout << "] ";

    std::cout << std::setw(3)
              << static_cast<int>(progress * 100.f)
              << "% ";

    std::cout << "("
              << processed / 1024 / 1024
              << "/"
              << total_bytes / 1024 / 1024
              << " MB) ";

    std::cout << "Tokens: "
              << token_count;

    std::cout.flush();
}
