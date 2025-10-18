#include "code_table.hpp"
#include <algorithm>
#include <functional>

namespace huffman {

CodeTable generate_code_table(const Node* root) {
    CodeTable table;
    
    if (!root) {
        // Empty tree - all codes remain empty (length 0)
        return table;
    }
    
    // Check if single-symbol tree
    if (std::holds_alternative<Leaf>(root->data)) {
        // Single symbol gets a 1-bit code
        const auto& leaf = std::get<Leaf>(root->data);
        table.codes[leaf.symbol] = Code({0});  // Assign bit '0'
        table.lengths[leaf.symbol] = 1;
        return table;
    }
    
    // Multi-symbol tree: traverse and build codes
    std::vector<byte> current_code;
    
    std::function<void(const Node*, std::vector<byte>&)> traverse = 
        [&](const Node* node, std::vector<byte>& code) {
            if (std::holds_alternative<Leaf>(node->data)) {
                // Leaf node: store the accumulated code
                const auto& leaf = std::get<Leaf>(node->data);
                table.codes[leaf.symbol] = Code(code);
                table.lengths[leaf.symbol] = static_cast<byte>(code.size());
            } else {
                // Branch node: recurse on children
                const auto& branch = std::get<Branch>(node->data);
                
                // Left child: append 0
                code.push_back(0);
                traverse(branch.left.get(), code);
                code.pop_back();
                
                // Right child: append 1  
                code.push_back(1);
                traverse(branch.right.get(), code);
                code.pop_back();
            }
        };
    
    traverse(root, current_code);
    return table;
}

} // namespace huffman