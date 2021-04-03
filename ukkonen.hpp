#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

namespace NUkkonen {
    struct TNode {
        int SuffStartPos;
        int Start;
        int End;
        TNode* SuffixLink = nullptr;
        TNode* Parent = nullptr;
        std::unordered_map<char, TNode*> ChildrenLinks;  

        TNode(): Start(0), End(0), Parent(nullptr) {}
        TNode(int start, int end, TNode* par): Start(start), End(end), Parent(par) {}
        TNode& operator=(const TNode& node);
        TNode(const TNode& node);
        ~TNode() = default;

        int Len();
        TNode*& GetNext(char c); 
    };

    struct TState {
        TNode* CurrNode;
        int CurrShift;

        TState(): CurrNode(nullptr) {}
        TState(TNode* node, int shift): CurrNode(node), CurrShift(shift) {}
        TState& operator=(const TState& state);
        TState(const TState& state);
        ~TState() = default;
    };

    class TSuffixTree {
    private:
        const std::string& Str;
        int LEN;
        TNode* Root;
        TState STATE;

        TState Go(TState state, int l, int r);
        TNode* Split(TState state);
        TNode* GetSuffixLink(TNode* node);
        void Extend(int pos);
        void RecursiveDelete(TNode* node);
    public:
        TSuffixTree(const std::string& str): Str(str), LEN(str.length()), Root(new TNode(0, 0, nullptr)), STATE(TState(Root, 0)) {}
        ~TSuffixTree();

        TState& State();
        void State(const TState& state); 
        TNode* GetRoot();
        void Build();
        int Compare(int& l, int r, const std::string& text, int matchStat);
        void GoSuffixLink(int& matchStat);
    };

    std::vector<int> Find(const std::string& text, const std::string& pattern);
}
