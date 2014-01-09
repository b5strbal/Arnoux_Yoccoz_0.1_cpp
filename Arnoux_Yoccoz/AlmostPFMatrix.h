//
//  AlmostPFMatrix.h
//  Arnoux_Yoccoz
//
//  Created by Balazs Strenner on 2/10/13.
//  Copyright (c) 2013 Balazs Strenner. All rights reserved.
//

#ifndef __Arnoux_Yoccoz__AlmostPFMatrix__
#define __Arnoux_Yoccoz__AlmostPFMatrix__

#include <iostream>
#include <deque>
#include <vector>
#include "Eigen/Eigenvalues"
#include "CirclePoint.h"

class AlmostPFMatrix{
public:
    AlmostPFMatrix(const std::vector<std::vector<int>>& Matrix);
    // AlmostPFMatrix(int Size) : m_data(Size, std::vector<int>(Size, 0)), m_PFEigenvector(0), m_PFEigenvalue(0) {}
   // void SetEntry(int i, int j, int Value) { m_data[i][j] = Value; }
   // int GetEntry(int i, int j) const { return m_data[i][j]; }
    bool IsPerronFrobenius() const { return m_PFEigenvector.size() != 0; }
    int Size() const { return static_cast<int>(m_data.size()); }
    std::vector<floating_point_type> GetPFEigenvector() const { return m_PFEigenvector; }
    floating_point_type GetPFEigenvalue() const { return m_PFEigenvalue; }
    
    friend std::ostream& operator<<(std::ostream& Out, const AlmostPFMatrix& Matrix);
    
private:
    std::vector<std::vector<int>> m_data;
    std::vector<floating_point_type> m_PFEigenvector;
    floating_point_type m_PFEigenvalue;
    
    void Transpose();
    void InitEigenData();
    bool IsPerronFrobenius_Init() {return IsEveryVertexReachableFrom(0) && IsReachableFromEveryVertex(0); }
    bool IsReachableFromEveryVertex(int Vertex);
    bool IsEveryVertexReachableFrom(int Vertex) const;
};

AlmostPFMatrix ArnouxYoccozMatrix(int genus);


#endif /* defined(__Arnoux_Yoccoz__AlmostPFMatrix__) */
