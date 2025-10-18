#pragma once

#include <expected>
#include <string>

namespace huffman {

struct Error {
    enum class Code {
        Io,
        MagicMismatch,
        CorruptHeader,
        VersionUnsupported,
        TreeBuildFailed,
        DecodeFailed,
        InvalidArgs
    };
    
    Code code;
    std::string message;
    
    Error(Code c, std::string msg) : code(c), message(std::move(msg)) {}
};

template<typename T>
using result = std::expected<T, Error>;

} // namespace huffman