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

// Helper function for creating unexpected results
template<typename T = void>
inline auto unexpected(Error&& e) {
    return std::unexpected<Error>(std::move(e));
}

} // namespace huffman
