#pragma once

#include "error.hpp"
#include "types.hpp"
#include <memory>
#include <variant>

namespace huffman {

struct Node;

// Leaf node contains a symbol
struct Leaf {
    byte symbol;
    
    explicit Leaf(byte s) : symbol(s) {}
};

// Branch node has left and right children
struct Branch {
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    
    Branch(std::unique_ptr<Node> l, std::unique_ptr<Node> r) 
        : left(std::move(l)), right(std::move(r)) {}
};

// Node combines frequency with either a Leaf or Branch
struct Node {
    std::uint64_t freq;
    std::variant<Leaf, Branch> data;
    
    Node(std::uint64_t f, Leaf leaf) : freq(f), data(std::move(leaf)) {}
    Node(std::uint64_t f, Branch branch) : freq(f), data(std::move(branch)) {}
};

// Build Huffman tree from frequency table
result<std::unique_ptr<Node>> build_huffman_tree(const FreqTable& table);

} // namespace huffman