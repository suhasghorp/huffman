#include "huffman_tree.hpp"
#include <queue>
#include <vector>
#include <ranges>
#include <algorithm>

namespace huffman {

// Custom deleter implementation
void NodeDeleter::operator()(Node* ptr) const {
    delete ptr;
}

result<NodePtr> build_huffman_tree(const FreqTable& table) {
    // Collect all symbols with non-zero frequencies
    std::vector<NodePtr> nodes;
    
    for (std::size_t i = 0; i < 256; ++i) {
        if (table[i] > 0) {
            auto leaf = make_node_ptr(table[i], Leaf(static_cast<byte>(i)));
            nodes.push_back(std::move(leaf));
        }
    }
    
    // Handle edge cases
    if (nodes.empty()) {
        // No symbols in input - return null as valid empty tree
        return NodePtr{};
    }
    
    if (nodes.size() == 1) {
        // Single symbol - return as-is, encoder will assign 1-bit code
        return std::move(nodes[0]);
    }
    
    // Custom comparator for min-heap (priority queue defaults to max-heap)
    auto compare = [](const NodePtr& a, const NodePtr& b) {
        return a->freq > b->freq;  // Reverse comparison for min-heap
    };
    
    // Use priority queue for efficient min-frequency selection
    std::priority_queue<NodePtr, std::vector<NodePtr>, decltype(compare)> 
        pq(compare, std::move(nodes));
    
    // Build tree by repeatedly combining two smallest-frequency nodes
    while (pq.size() > 1) {
        // Extract two nodes with smallest frequencies
        auto left = std::move(const_cast<NodePtr&>(pq.top()));
        pq.pop();
        auto right = std::move(const_cast<NodePtr&>(pq.top()));
        pq.pop();
        
        // Create new branch node with combined frequency
        auto combined_freq = left->freq + right->freq;
        auto branch = make_node_ptr(combined_freq, Branch(std::move(left), std::move(right)));
        
        pq.push(std::move(branch));
    }
    
    // Return the root node
    return std::move(const_cast<NodePtr&>(pq.top()));
}

} // namespace huffman