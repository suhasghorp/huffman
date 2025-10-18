#include "io.hpp"
#include <fstream>
#include <filesystem>

namespace huffman {

result<std::vector<byte>> read_file_bytes(std::string_view path) {
    std::ifstream file(std::filesystem::path{path}, std::ios::binary | std::ios::ate);
    
    if (!file) {
        return std::unexpected(Error(Error::Code::Io, 
            std::string("Failed to open file for reading: ") + std::string(path)));
    }
    
    auto size = file.tellg();
    if (size < 0) {
        return std::unexpected(Error(Error::Code::Io, 
            std::string("Failed to get file size: ") + std::string(path)));
    }
    
    file.seekg(0, std::ios::beg);
    if (!file) {
        return std::unexpected(Error(Error::Code::Io, 
            std::string("Failed to seek to beginning: ") + std::string(path)));
    }
    
    std::vector<byte> buffer(static_cast<std::size_t>(size));
    
    if (size > 0) {
        file.read(reinterpret_cast<char*>(buffer.data()), size);
        if (!file || file.gcount() != size) {
            return std::unexpected(Error(Error::Code::Io, 
                std::string("Failed to read file contents: ") + std::string(path)));
        }
    }
    
    return buffer;
}

result<void> write_file_bytes(std::string_view path, std::span<const byte> data) {
    std::ofstream file(std::filesystem::path{path}, std::ios::binary);
    
    if (!file) {
        return std::unexpected(Error(Error::Code::Io, 
            std::string("Failed to open file for writing: ") + std::string(path)));
    }
    
    if (!data.empty()) {
        file.write(reinterpret_cast<const char*>(data.data()), 
                   static_cast<std::streamsize>(data.size()));
        if (!file) {
            return std::unexpected(Error(Error::Code::Io, 
                std::string("Failed to write file contents: ") + std::string(path)));
        }
    }
    
    return {};
}

} // namespace huffman