//
//  AlmostPFMatrix.cpp
//  Arnoux_Yoccoz
//
//  Created by Balazs Strenner on 2/10/13.
//  Copyright (c) 2013 Balazs Strenner. All rights reserved.
//

#include "AlmostPFMatrix.h"


AlmostPFMatrix::AlmostPFMatrix(const std::vector<std::vector<int>>& Matrix):
    m_data(Matrix)
{
    assert(Matrix.size() == Matrix[0].size()); // making sure it is a square matrix
    for (int i = 0; i < Size(); i++) {
        for (int j = 0; j < Size(); j++) {
            assert(m_data[i][j] >= 0);         // making sure entries are non-negative
        }
    }
    InitEigenData();
}




void AlmostPFMatrix::InitEigenData(){
    if (IsPerronFrobenius_Init()) {
        m_PFEigenvector.resize(Size());
        Eigen::MatrixXd EigenMatrix(Size(), Size());
        for (int i = 0; i < Size(); i++) {
            for (int j = 0 ; j < Size(); j++) {
                EigenMatrix(i, j) = m_data[i][j] ;
            }
        }
        
        Eigen::EigenSolver<Eigen::MatrixXd> es(EigenMatrix);
        
        double LargestValue = 0;
        int IndexOfLargest = 0;
        for (int i = 0; i < Size(); i++) {
            if(LargestValue < fabs(real(es.eigenvalues()[i]))){
                LargestValue = fabs(real(es.eigenvalues()[i]));
                IndexOfLargest = i;
            }
        }
        m_PFEigenvalue = LargestValue;
        
        for(int i = 0; i < Size(); i++){
            m_PFEigenvector[i] = fabs(real(es.eigenvectors().col(IndexOfLargest)[i]));
        }
    }
}











void AlmostPFMatrix::Transpose(){
    std::vector<std::vector<int>> Copy(m_data);
    
    for (int i = 0; i < Size(); i++) {
        for (int j = 0; j < Size(); j++) {
            m_data[i][j] = Copy[j][i];
        }
    }
}




bool AlmostPFMatrix::IsEveryVertexReachableFrom(int Vertex) const{
    int CountReachableVertices = 1;
    
    enum vertex_status{
        ALREADY_SEARCHED_NEIGHBORS,
        NEIGHBORS_TO_BE_SEARCHED,
        NOT_YET_REACHED
    };
    
    std::vector<vertex_status> Status(Size(), NOT_YET_REACHED);
    Status[Vertex] = NEIGHBORS_TO_BE_SEARCHED;
    std::deque<int> VerticesToSearch(1, Vertex);
    
    while (VerticesToSearch.size() > 0) {
        int CurrentVertex = VerticesToSearch.front();
        for (int i = 0; i < Size(); i++) {
            if (m_data[CurrentVertex][i] > 0 && Status[i] == NOT_YET_REACHED) {
                Status[i] = NEIGHBORS_TO_BE_SEARCHED;
                CountReachableVertices++;
                VerticesToSearch.push_back(i);
            }
        }
        Status[CurrentVertex] = ALREADY_SEARCHED_NEIGHBORS;
        VerticesToSearch.pop_front();
    }
    
    if (CountReachableVertices == Size()) {
        return true;
    }
    return false;
}


bool AlmostPFMatrix::IsReachableFromEveryVertex(int Vertex) {
    Transpose();
    bool temp = IsEveryVertexReachableFrom(Vertex);
    Transpose();
    return temp;
}




std::ostream& operator<<(std::ostream& Out, const AlmostPFMatrix& Matrix){
    for (int i = 0; i < Matrix.Size(); i++) {
        for (int j = 0; j < Matrix.Size(); j++) {
            Out << Matrix.m_data[i][j] << " ";
        }
        std::cout << std::endl;
    }
    return Out;
}







AlmostPFMatrix ArnouxYoccozMatrix(int genus){
    std::vector<std::vector<int>> Matrix(genus, std::vector<int>(genus));
    
    Matrix[0][0] = 1;
    Matrix[0][genus - 1] = 1;
    for (int i = 1; i < genus - 1; i++) {
        Matrix[i][i - 1] = 1;
    }
    for (int i = 0; i < genus - 1; i++) {
        Matrix[genus - 1][i] = 1;
    }
    return AlmostPFMatrix(Matrix);
}

