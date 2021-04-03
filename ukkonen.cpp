#include "ukkonen.hpp"

namespace NUkkonen {
    TNode& TNode::operator=(const TNode& node) {
        SuffStartPos = node.SuffStartPos;
        Start = node.Start;
        End = node.End;
        SuffixLink = node.SuffixLink;
        Parent = node.Parent;
        ChildrenLinks = node.ChildrenLinks;
        return *this;
    }

    TNode::TNode(const TNode& node) {
        *this = node;       
    }

    int TNode::Len() {
        return End - Start;
    }

    TNode*& TNode::GetNext(char c) {
        if (!ChildrenLinks.count(c)) {
            ChildrenLinks.insert(std::make_pair(c, nullptr));
        }
		return ChildrenLinks[c];
    }

    TState& TState::operator=(const TState& state) {
        CurrNode = state.CurrNode;
        CurrShift = state.CurrShift;
        return *this;
    }

    TState::TState(const TState& state) {
        *this = state;
    }

    void TSuffixTree::RecursiveDelete(TNode* node) {
        if (node == nullptr) {
            return;
        }
        for (auto c: node->ChildrenLinks) {
            RecursiveDelete(c.second);
        }
        delete node;
    }

    TSuffixTree::~TSuffixTree() {
        RecursiveDelete(Root);
    }

    TState TSuffixTree::Go(TState state, int l, int r) {
        while (l < r) {
            if (state.CurrShift == state.CurrNode->Len()) {
                state = TState(state.CurrNode->GetNext(Str[l]), 0);
                if (state.CurrNode == nullptr)  {
                    return state;
                }
            } else {
                if (Str[state.CurrNode->Start + state.CurrShift] != Str[l]) {
                    return TState(nullptr, -1);
                }
                if (r - l < state.CurrNode->Len() - state.CurrShift) {
                    return TState(state.CurrNode, state.CurrShift + r - l);
                }
                l += state.CurrNode->Len() - state.CurrShift;
                state.CurrShift = state.CurrNode->Len();
            }
        }
        return state;        
    }

    TNode* TSuffixTree::Split(TState state) {
        if (state.CurrShift == state.CurrNode->Len()) {
            return state.CurrNode;
        }
        if (state.CurrShift == 0) {
            return state.CurrNode->Parent;
        }
        TNode* nodeInState = state.CurrNode;
        TNode* newNode = new TNode(nodeInState->Start, nodeInState->Start + state.CurrShift, nodeInState->Parent);
        nodeInState->Parent->GetNext(Str[nodeInState->Start]) = newNode;
        newNode->GetNext(Str[nodeInState->Start + state.CurrShift]) = state.CurrNode;
        state.CurrNode->Parent = newNode;
        state.CurrNode->Start += state.CurrShift;
        return newNode;
    }

    TNode* TSuffixTree::GetSuffixLink(TNode* node) {
    	if (node->SuffixLink != nullptr) {
            return node->SuffixLink;
        }
	    if (node->Parent == nullptr) {
            return Root;
        }
	    TNode* to = GetSuffixLink(node->Parent);
        TState suffixState = TState(to, to->Len());

        TState placeToSplit;
        if (node->Parent == Root) { // если предок корень, то нужно откусить 1 символ
            placeToSplit = Go(suffixState, node->Start + 1, node->End);
        } else {
            placeToSplit = Go(suffixState, node->Start, node->End);
        }
	    return node->SuffixLink = Split(placeToSplit);    
    }

    void TSuffixTree::Extend(int pos) {
        while(true) {
            TState newState = Go(STATE, pos, pos+1);
            if (newState.CurrNode != nullptr) {
                STATE = newState;
                return;
            }

            TNode* mid = Split(STATE);
            TNode* newNode = new TNode(pos, LEN, mid);
            mid->GetNext(Str[pos]) = newNode;

            STATE.CurrNode = GetSuffixLink(mid);
            STATE.CurrShift = STATE.CurrNode->Len();
            if (mid == Root) {
                break;
            }
        }
    }

    void TSuffixTree::Build() {
	    for (int i = 0; i < LEN; ++i) {
		    Extend(i);
        }
    }

    TState& TSuffixTree::State() {
        return STATE;
    }

    void TSuffixTree::State(const TState& state) {
        STATE = state;
    }

    TNode* TSuffixTree::GetRoot() {
        return Root;
    }

    void TSuffixTree::GoSuffixLink(int& matchStat) {
        if (STATE.CurrNode != Root) {
            TNode* parent = STATE.CurrNode->Parent;
            if (parent != Root) {
                TNode* suffixNode = parent->SuffixLink;
                int l = STATE.CurrNode->Start;
                int r = l + STATE.CurrShift;
                STATE.CurrNode = suffixNode;
                STATE.CurrShift = STATE.CurrNode->Len();
                STATE = Go(STATE, l, r);
                matchStat--;
            } else {
                STATE.CurrNode = Root;
                STATE.CurrShift = 0;
                matchStat = 0;
            }
        }
    }

    int TSuffixTree::Compare(int& l, int r, const std::string& text, int matchStat) {
        int matchCount = matchStat;
        while (l < r) {
            if (STATE.CurrShift == STATE.CurrNode->Len()) {
                if (!STATE.CurrNode->ChildrenLinks.count(text[l])) {
                    break;
                }
                STATE = TState(STATE.CurrNode->GetNext(text[l]), 0);
            } else {
                int oldL = l;
                int oldShift = STATE.CurrShift;
                for (int i = 0; i < STATE.CurrNode->Len() - oldShift; ++i) {
                    if (text[i + oldL] != Str[i + STATE.CurrNode->Start + oldShift]) {
                        return matchCount;
                    }
                    matchCount++;
                    STATE.CurrShift++;
                    l++;
                }
            }
        }
        return matchCount;
    }

    std::vector<int> Find(const std::string& text, const std::string& pattern) {
        std::vector<int> entery;
        if (!text.length() || !pattern.length() || pattern.length() > text.length()) {
            return entery;
        }
        int matchStat = 0;
        TSuffixTree t(pattern);
        t.Build();

        int r = pattern.length();
        int l = 0;
        int startPos = 0;
        t.State(TState(t.GetRoot(), 0));
        while (l - matchStat <= text.length() - pattern.length()) {
            matchStat = t.Compare(l, r, text, matchStat);
            if (matchStat == pattern.length()) {
                entery.push_back(startPos + 1);
            }
            t.GoSuffixLink(matchStat);
            r++;
            startPos++;
            if (matchStat == 0) {
                l = startPos;
            }
        }
        return entery;
    }
}

/*
Тут моя программа падала, как Стас Барецкий
abacbba
aababcbabcbabbabcbbabcabacbbabaaccbaaa

еще меньше случай
abacbba
abacbbab

=======
abacabc
bcbacbbbabcbbcaaabcbcababacabccaabcbabcabcbababcbcbabcbacbbababcbacbabcbabc


случай поменьше
abacabc
bcbacbbbabcbbcaaabcbcaba

еще меньше
abacabc
abcbcaba

=========
abacabc
babacabc

===============

abcbb
cbcbbcbcbcbbcbcbc

случай меньше
abcbb
cbcbbcbcbcbb

еще меньше
abcbb
cbcbbcbcbb

самый маленький
abcbb
bcbcbb
*/
