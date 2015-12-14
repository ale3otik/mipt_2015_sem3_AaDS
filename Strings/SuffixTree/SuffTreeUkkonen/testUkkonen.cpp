#include <vector>
#include <string>
#include <array>
#include <iostream>
using namespace std;
typedef long long ll;
class SuffixTree {
public:
    static const size_t INF = (size_t)(-1);
    static const size_t ALP_SIZE = 28; // the smallest to optimze time
    class Node {
    public:
        std::array<size_t,ALP_SIZE> children;
        size_t suff_link;

        size_t p_ind; // index of parent in tree::nodes
        size_t s_pos; // position of the edge-begin in source string

        bool is_leaf;
        size_t p_edge_len; // if node is not a leaf
    };
    
    std::vector<Node> nodes;
    SuffixTree(const std::string & source_str); // build tree
    void buildTree();
    size_t getEdgeLength(size_t ind) const;

    size_t end_text_ind;
private:
    bool is_tree_built_already_;
    
    class SuffixTreeBuilder{
    public:
        SuffixTreeBuilder(SuffixTree * tree, const std::string & source_str);
        void build();
    private:
        
        SuffixTree * tree_;
        const std::string str_;
        
        size_t first_empty_; // first empty action position
        size_t need_link_from_; // need update suff link
        size_t edge_offset_;
        
        void createBaseForTree_(); // create root and dummy
        size_t testFromNode(size_t ind, char next_symbol); // create leaf or empty
        size_t testFromEdge(size_t ind, char next_symbol); // create node + leaf or empty
        void addNextSymbol_(char symbol);
        size_t makeNode_(size_t child_ind,size_t offset,char next_char); // no leaf
        size_t makeLeaf_(size_t parent, char symbol);
        char getSymbolFromEdge_(size_t ind, size_t offset) const;
        size_t findRelativeOffset_(size_t p_ind, size_t start); // find edge that need to test
    } builder_;
};


/******************************SuffixTree*********************************/
SuffixTree::SuffixTree(const std::string & source_str):
end_text_ind(INF),
builder_(this,source_str),
is_tree_built_already_(false) {}

void SuffixTree::buildTree() {
    if(is_tree_built_already_) return;
    is_tree_built_already_ = true;
    
    builder_.build();
}

size_t SuffixTree::getEdgeLength(size_t ind) const {
    const Node & v = nodes[ind];
    if(v.is_leaf) {
        return end_text_ind - v.s_pos + 1;
    } else {
        return v.p_edge_len;
    }
}

/**********************SuffixTree builder**********************************/
const size_t SuffixTree::INF;

SuffixTree::SuffixTreeBuilder::SuffixTreeBuilder(SuffixTree * source_tree, const std::string & source_str):
tree_(source_tree),
str_(source_str),
first_empty_(1),
need_link_from_(1),
edge_offset_(0) {}

/*create dummy node and root of tree*/
void SuffixTree::SuffixTreeBuilder::createBaseForTree_() {
    tree_->nodes.reserve(2 * str_.size() + 4);
    tree_->nodes.resize(2);

    // create dummy node
    Node & dummy = tree_->nodes[0];
    dummy.is_leaf = false;
    dummy.p_ind = 0;
    dummy.children.fill(1);
    
    //create root of tree
    Node & root = tree_->nodes[1];
    root.is_leaf = false;
    root.p_ind = 0;
    root.s_pos = 0;
    root.p_edge_len = 1;
    root.children.fill(INF);
}

char SuffixTree::SuffixTreeBuilder::getSymbolFromEdge_(size_t ind, size_t offset) const {
    const Node & v = tree_->nodes[ind];
    return str_[v.s_pos + offset];
}

size_t SuffixTree::SuffixTreeBuilder::findRelativeOffset_(size_t p_ind, size_t s_position) {
    while(true){
        if(edge_offset_ == 0) break;
        size_t child_ind = tree_->nodes[p_ind].children[str_[s_position]];
        size_t lenght = tree_->getEdgeLength(child_ind);
        if(edge_offset_ < lenght) {
            p_ind = child_ind;
            break;
        } else {
            edge_offset_ -= lenght;
            s_position += lenght;
            p_ind = child_ind;
        }
    }
    return p_ind;
}

size_t SuffixTree::SuffixTreeBuilder::makeLeaf_(size_t parent_ind, char symb) {
    size_t new_size = tree_->nodes.size() + 1;
    tree_->nodes.resize(new_size);
    Node & leaf = tree_->nodes[new_size - 1];
    
    leaf.is_leaf = true;
    leaf.p_ind = parent_ind;
    leaf.s_pos = tree_->end_text_ind;
    tree_->nodes[parent_ind].children[symb] = new_size - 1;
    
    return new_size - 1;
}

size_t SuffixTree::SuffixTreeBuilder::makeNode_(size_t last_ind, size_t offset, char next_char) {
    char last_char = getSymbolFromEdge_(last_ind, offset);
    tree_->nodes.resize(tree_->nodes.size() + 1);
    
    size_t new_v_ind = tree_->nodes.size() - 1;
    Node & new_v = tree_->nodes[new_v_ind];
    Node & last_v = tree_->nodes[last_ind];
    Node & parent_v = tree_->nodes[last_v.p_ind];
    
    new_v.is_leaf = false;
    new_v.s_pos = last_v.s_pos;
    new_v.p_ind = last_v.p_ind;
    new_v.p_edge_len = offset;
    new_v.children.fill(INF);
    
    new_v.children[last_char] = last_ind;
    parent_v.children[str_[new_v.s_pos]] = new_v_ind;
    
    last_v.p_ind = new_v_ind;
    last_v.p_edge_len = last_v.p_edge_len - offset;
    last_v.s_pos = new_v.s_pos + offset;
    
    makeLeaf_(new_v_ind, next_char);
    return new_v_ind;
}

size_t SuffixTree::SuffixTreeBuilder::testFromEdge(size_t ind, char next_symbol) {
    char expected_char = getSymbolFromEdge_(ind, edge_offset_);
    if(expected_char == next_symbol) {
        if(edge_offset_ + 1 == tree_->getEdgeLength(ind)) {
            edge_offset_ = 0;
            if(need_link_from_ != 1) {
                tree_->nodes[need_link_from_].suff_link = ind;
                need_link_from_ = 1;
            }
        } else {
            ++edge_offset_;
        }
        first_empty_ = ind;
        return INF;
    }
    
    return makeNode_(ind, edge_offset_, next_symbol);
}

size_t SuffixTree::SuffixTreeBuilder::testFromNode(size_t ind, char next_symbol) {
    if(need_link_from_ != 1) {
        tree_->nodes[need_link_from_].suff_link = ind;
        need_link_from_ = 1;
    }
    
    if(tree_->nodes[ind].children[next_symbol] != INF) {
        ++edge_offset_;
        first_empty_ = tree_->nodes[ind].children[next_symbol];
        if(tree_->getEdgeLength(first_empty_) == 1) {
            edge_offset_ = 0;
        }
        return INF;
    }
    
    return makeLeaf_(ind, next_symbol);
}

void SuffixTree::SuffixTreeBuilder::addNextSymbol_(char next_symbol) {
    ++tree_->end_text_ind;
    size_t ind = first_empty_;
    while(true) {
        size_t new_v_ind;
        size_t link_to;
        if(edge_offset_ == 0) {
            new_v_ind = testFromNode(ind, next_symbol);
            if(new_v_ind == INF) break;
            link_to = tree_->nodes[new_v_ind].p_ind;
        } else {
            new_v_ind = testFromEdge(ind, next_symbol);
            if(new_v_ind == INF) break;
            link_to = new_v_ind;
        }
        
        const Node & new_v = tree_->nodes[new_v_ind];
        size_t next_v_ind = tree_->nodes[new_v.p_ind].suff_link;
        size_t str_position = new_v.s_pos;
        
        if(need_link_from_ != 1) {
            tree_->nodes[need_link_from_].suff_link = link_to;
        }
        need_link_from_ = link_to;
        ind = findRelativeOffset_(next_v_ind,str_position);
    }
}

void SuffixTree::SuffixTreeBuilder::build() {
    createBaseForTree_();
    for(size_t i = 0; i < str_.size(); ++i) {
        addNextSymbol_(str_[i]);
    }
}

ll getTreeEdgeLengthSumm(const SuffixTree & tree) {
    ll ans = 0;
    for(size_t i = 2; i < tree.nodes.size(); ++i) {
        ans += tree.getEdgeLength(i);
    }
    return ans;
}

int main() {
    std::ios::sync_with_stdio(false);
    string str;
    ll k;
    std::cin >> k >> str;
    for(size_t i = 0; i < str.size(); ++i) {
        str[i] -= 'a' - 2;
    }
    
    string full_str = str + str;
    
    string substr;
    substr.reserve(k);
    vector<ll> ans(str.size(),0);
    for(size_t j = 0; j < str.size(); ++j) {
        substr = full_str.substr(j,k);
        SuffixTree tree(substr);
        tree.buildTree();
        ans[j] = getTreeEdgeLengthSumm(tree);
    }
    
    for(ll i = 0; i < ans.size(); ++i) {
        std::cout << ans[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}
