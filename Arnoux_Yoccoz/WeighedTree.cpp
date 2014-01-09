//
//  WeighedTree.cpp
//  Arnoux_Yoccoz
//
//  Created by Balazs Strenner on 2/3/13.
//  Copyright (c) 2013 Balazs Strenner. All rights reserved.
//

#include "WeighedTree.h"



WeighedTree::WeighedTree(std::vector<NumberType> Weights){
    Init(Weights);
}



WeighedTree::WeighedTree(int NumEdges){
    std::vector<NumberType> Weights;
    bool Success = false;
    while (!Success) {
        try {
            GenerateRandomWeights(Weights, NumEdges);
            Init(Weights);
            Success = true;
        } catch (const char*) {}
    }
}

WeighedTree::~WeighedTree(){
    delete m_Root;
}




void WeighedTree::Init(std::vector<NumberType> Weights){
    m_Root = new Node;
    CreateChildren(Weights.begin(), Weights.end(), m_Root);
    SetNumDescendants(m_Root);
}





void WeighedTree::GenerateRandomWeights(std::vector<NumberType>& Weights, int NumEdges){
    std::default_random_engine generator(static_cast<int>(time(NULL)));
    std::uniform_real_distribution<long double> RealDistribution(0,1);
    Weights.clear();
    Weights.push_back(RealDistribution(generator));

    int Remains = NumEdges - 1;
    while (Remains > 0) {
        std::uniform_int_distribution<int> IntDistribution(2, Remains);
        int NextSequence = Remains - 1;
        while (NextSequence == Remains - 1) {
            NextSequence = IntDistribution(generator);
        }
        for (int i = 0; i < NextSequence; i++) {
            Weights.push_back(RealDistribution(generator));
        }
        Weights.push_back(0);
        Remains -= NextSequence;
    }
}





void WeighedTree::CreateChildren(std::vector<NumberType>::iterator itBegin, std::vector<NumberType>::iterator itEnd, Node* pNode){
    std::vector<NumberType>::iterator it = itBegin;
    while (it != itEnd && *it != 0) {
        if (*it < 0) {
            throw "Weights of weighed trees must be positive.";
        }
        it++;
    }
    if (pNode->m_Parent == NULL && it - itBegin < 3) {
        throw "The root of a weighed tree must have at least 3 neighbors.";
    }

    if (it != itBegin) {
        pNode->m_NumChildren = static_cast<int>(it - itBegin);
        if (pNode->m_Parent != NULL && pNode->m_NumChildren == 1) {
            throw "Vertices of degree 2 are not allowed in weighed trees.";
        }
        
        
        pNode->m_Children = new Node[pNode->m_NumChildren];
        for (int i = 0; i < pNode->m_NumChildren; i++) {
            pNode->m_Children[i].m_Parent = pNode;
            pNode->m_Children[i].m_Weight = itBegin[i];
        }
    }
    
    if (++it < itEnd) {
        pNode = NextNode(pNode);
        if (pNode->IsRoot()) {
            throw "Too many arguments provided for the definition of a weighed tree.";
        } else
            CreateChildren(it, itEnd, pNode);
    }
}



void WeighedTree::SetNumDescendants(Node* pNode){
    int CountDescendants = 0;
    for (int i = 0; i < pNode->m_NumChildren; i++) {
        SetNumDescendants(pNode->m_Children + i);
        CountDescendants += pNode->m_Children[i].m_NumDescendants;
    }
    pNode->m_NumDescendants = CountDescendants + pNode->m_NumChildren;
}


WeighedTree::Node::~Node(){
    delete [] m_Children;
}






WeighedTree::Node* WeighedTree::NextNode(Node* pNode){
    if (!pNode->IsLastSibling()) {
        return pNode + 1;
    }
    Node* NewNode;
    
    NewNode = pNode->FirstSibling();
    for (int i = 0; i < pNode->GetNumberOfSiblings(); i++) {
        if (NewNode->m_NumChildren > 0) {
            return NewNode->m_Children;
        }
        NewNode++;
    }
    
    return m_Root;
}



