//
//  WeighedTree.h
//  Arnoux_Yoccoz
//
//  Created by Balazs Strenner on 2/3/13.
//  Copyright (c) 2013 Balazs Strenner. All rights reserved.
//

#ifndef __Arnoux_Yoccoz__WeighedTree__
#define __Arnoux_Yoccoz__WeighedTree__

#include <iostream>
#include <vector>
#include <random>

typedef long double NumberType;
class WeighedTree{
public:
    WeighedTree(std::vector<NumberType> Weights);
    WeighedTree(int NumEdges);      // A random weighed tree with NumEdges edges
    ~WeighedTree();
    
    struct Node{
        NumberType m_Weight = 0;
        Node* m_Parent = NULL;
        int m_NumChildren = 0;
        Node* m_Children = NULL;
        int m_NumDescendants = 0;
        
        ~Node();
        
        inline Node* FirstChild() { return m_NumChildren == 0 ? NULL : m_Children; }
        inline Node* LastChild() { return m_NumChildren == 0 ? NULL : m_Children + (m_NumChildren - 1); }
        inline Node* FirstSibling() { return IsRoot() ? this : m_Parent->FirstChild(); };
        //Node* Root();
        
        inline bool IsRoot() { return m_Parent == NULL ? true : false; }
        bool IsLastSibling() { return IsRoot() || m_Parent->LastChild() == this ? true : false; }
        int GetNumberOfSiblings(){ return IsRoot() ? 1 : m_Parent->m_NumChildren; }
    };

    Node* m_Root;
private:
    void Init(std::vector<NumberType> Weights);
    void GenerateRandomWeights(std::vector<NumberType>& Weights, int NumEdges);
    Node* NextNode(Node*);
    void CreateChildren(std::vector<NumberType>::iterator itBegin, std::vector<NumberType>::iterator itEnd, Node* pNode);
    void SetNumDescendants(Node* pNode);
    
    WeighedTree(const WeighedTree&);
    WeighedTree& operator=(const WeighedTree&);
};













#endif /* defined(__Arnoux_Yoccoz__WeighedTree__) */
