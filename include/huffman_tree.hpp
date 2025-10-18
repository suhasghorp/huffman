#pragma once

#include "error.hpp"
#include "types.hpp"
#include <memory>
#include <variant>

namespace huffman {

// Leaf node contains a symbol
struct Leaf {
    byte symbol;
    
    explicit Leaf(byte s) : symbol(s) {}
};

// Branch node has left and right children  
struct Branch {
    std::unique_ptr<struct Node> left;
    std::unique_ptr<struct Node> right;
    
    Branch(std::unique_ptr<struct Node> l, std::unique_ptr<struct Node> r) 
        : left(std::move(l)), right(std::move(r)) {}
    
    // Move-only type
    Branch(const Branch&) = delete;
    Branch& operator=(const Branch&) = delete;
    Branch(Branch&&) = default;
    Branch& operator=(Branch&&) = default;
    
    ~Branch(); // Declared here, defined in .cpp
};

// Node combines frequency with either a Leaf or Branch
struct Node {
    std::uint64_t freq;
    std::variant<Leaf, Branch> data;
    
    Node(std::uint64_t f, Leaf leaf) : freq(f), data(std::move(leaf)) {}
    Node(std::uint64_t f, Branch branch) : freq(f), data(std::move(branch)) {}
    ~Node(); // Declared here, defined in .cpp
};

// Build Huffman tree from frequency table
result<std::unique_ptr<Node>> build_huffman_tree(const FreqTable& table);

} // namespace huffman