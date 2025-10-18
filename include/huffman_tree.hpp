#pragma once

#include "error.hpp"
#include "types.hpp"
#include <memory>
#include <variant>

namespace huffman {

// Forward declaration
struct Node;

// Custom deleter for Node - declared here, defined in .cpp
struct NodeDeleter {
    void operator()(Node* ptr) const;
};

// Unique pointer type for Node with custom deleter
using NodePtr = std::unique_ptr<Node, NodeDeleter>;

// Leaf node contains a symbol
struct Leaf {
    byte symbol;
    
    explicit Leaf(byte s) : symbol(s) {}
};

// Branch node has left and right children  
struct Branch {
    NodePtr left;
    NodePtr right;
    
    Branch(NodePtr l, NodePtr r) 
        : left(std::move(l)), right(std::move(r)) {}
    
    // Move-only type
    Branch(const Branch&) = delete;
    Branch& operator=(const Branch&) = delete;
    Branch(Branch&&) = default;
    Branch& operator=(Branch&&) = default;
};

// Node combines frequency with either a Leaf or Branch
struct Node {
    std::uint64_t freq;
    std::variant<Leaf, Branch> data;
    
    Node(std::uint64_t f, Leaf leaf) : freq(f), data(std::move(leaf)) {}
    Node(std::uint64_t f, Branch branch) : freq(f), data(std::move(branch)) {}
};

// Helper function to create NodePtr
template<typename... Args>
NodePtr make_node_ptr(Args&&... args) {
    return NodePtr(new Node(std::forward<Args>(args)...), NodeDeleter{});
}

// Build Huffman tree from frequency table
result<NodePtr> build_huffman_tree(const FreqTable& table);

} // namespace huffman