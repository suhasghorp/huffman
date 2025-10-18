#include "huffman_tree.hpp"
#include <queue>
#include <vector>
#include <ranges>
#include <algorithm>

namespace huffman {

// Destructor definitions (needed to break circular dependency)
Branch::~Branch() = default;
Node::~Node() = default;

result<std::unique_ptr<Node>> build_huffman_tree(const FreqTable& table) {
    // Collect all symbols with non-zero frequencies
    std::vector<std::unique_ptr<Node>> nodes;
    
    for (std::size_t i = 0; i < 256; ++i) {
        if (table[i] > 0) {
            auto leaf = std::make_unique<Node>(table[i], Leaf(static_cast<byte>(i)));
            nodes.push_back(std::move(leaf));
        }
    }
    
    // Handle edge cases
    if (nodes.empty()) {
        // No symbols in input - return null as valid empty tree
        return std::unique_ptr<Node>{};
    }
    
    if (nodes.size() == 1) {
        // Single symbol - return as-is, encoder will assign 1-bit code
        return std::move(nodes[0]);
    }
    
    // Custom comparator for min-heap (priority queue defaults to max-heap)
    auto compare = [](const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b) {
        return a->freq > b->freq;  // Reverse comparison for min-heap
    };
    
    // Use priority queue for efficient min-frequency selection
    std::priority_queue<std::unique_ptr<Node>, std::vector<std::unique_ptr<Node>>, decltype(compare)> 
        pq(compare, std::move(nodes));
    
    // Build tree by repeatedly combining two smallest-frequency nodes
    while (pq.size() > 1) {
        // Extract two nodes with smallest frequencies
        auto left = std::move(const_cast<std::unique_ptr<Node>&>(pq.top()));
        pq.pop();
        auto right = std::move(const_cast<std::unique_ptr<Node>&>(pq.top()));
        pq.pop();
        
        // Create new branch node with combined frequency
        auto combined_freq = left->freq + right->freq;
        auto branch = std::make_unique<Node>(combined_freq, Branch(std::move(left), std::move(right)));
        
        pq.push(std::move(branch));
    }
    
    // Return the root node
    return std::move(const_cast<std::unique_ptr<Node>&>(pq.top()));
}

} // namespace huffman