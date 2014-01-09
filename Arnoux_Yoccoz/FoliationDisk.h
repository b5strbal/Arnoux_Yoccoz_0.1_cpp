#ifndef FOLIATIONDISK_H
#define FOLIATIONDISK_H

/*******************************************************************************
 *  CLASS NAME:	FoliationDisk
 *  AUTHOR:		Balazs Strenner
 *				Copyright 2013 UW-Madison. All rights reserved.
 *  DATE:		1/8/13
 *
 *	DESCRIPTION: Measured foliation on the disk, transverse to the boundary.
 
    MORE DETAILS: If the number of prongs of the singularities are n_1, n_2, etc, then by the Euler-Poincare formula,
                sum(n_i - 2) = -2. In particular, the number of prongs are always even. We are interseted in creating
                measured foliations on RP^2 or on the sphere from foliations on the disk that are stable or unstable
                foliations of pseudo-anosovs, therefore we assume that there are no saddle connections. In this case
                all separatrices intersect the boundary. We call those points DIVISION POINTS (m_DivPoints). 
                Because of the above, there is always even number of them.   

                The division points divide the boundary circle (in short: Circle) into same number of intervals 
                (m_Intervals). These Intervals are naturally paired up by leaves of the foliation (m_Pair).
                The lengths of the Intervals in pairs have the same length (m_Lengths).
 
                Conversely, given a list of even number of positive numbers, and a pairing of them such that pairs
                have the same length and such that the parity of the indices of the pairs are different (e.g. the 1st
                and 3rd Intervals can't be paired up by the foliation because then the 2nd one does not have a pair), then
                up to Whitehead equivalence, there exists a unique measured foliation with that data. (Unique
                to isotopy among the ones with no saddle connections.)
 
                We assume that the boundary has length 1, so sum(m_Lenghts[i]) = 1. 
 
                We use the CirclePoint class to represent the division points. The first division point is always 0.
 
 *				
 *
 ******************************************************************************/

#include <iostream>
#include <vector>
#include "Arc.h"
#include "WeighedTree.h"

const floating_point_type PRECISION = 0.000000001;

class ExceptionThePointIsADivPoint {};


class FoliationDisk
{
public:
    FoliationDisk(const WeighedTree& wt);
    FoliationDisk(const std::vector<floating_point_type>& Lengths, const std::vector<int>& Pair);
  //  FoliationDisk(const FoliationDisk& fd);
    CirclePoint FollowTheLeaf(const CirclePoint& t); // The point we get by following the leaf from t until we hit the boundary circle again.
    int WhichIntervalContainsNew(const CirclePoint& t);
    int WhichIntervalContains(const CirclePoint& t);      // Find the index of the Interval that contains t
    int WhichIntervalContains_ForManySeparatrices(const CirclePoint& t); // another version of the previous function, faster for many separatrices
    bool IsDivPoint(const CirclePoint& t); // Decides if a point is a division point
    void Rotate();
    void Reflect();
    int GetPair(int Index) const{ return m_Pair[Index]; }
    
    friend std::ostream& operator<<(std::ostream& Out, FoliationDisk fd);
    
    
protected:
    int m_NumSeparatrices;      // number of separatrices
    std::vector<CirclePoint> m_DivPoints;   // division points: intersections of separatrices with the boundary circle
    std::vector<int> m_Pair;        // the index of the pair of the i'th Interval
    
    inline int IncreaseIndex(int SeparatrixIndex) const{ return SeparatrixIndex < m_NumSeparatrices - 1 ? SeparatrixIndex + 1 : 0; }
    inline int DecreaseIndex(int SeparatrixIndex) const{ return SeparatrixIndex > 0 ? SeparatrixIndex - 1 : m_NumSeparatrices - 1; }
    inline int ReflectIndex(int SeparatrixIndex) const{ return m_NumSeparatrices - 1 - SeparatrixIndex; }
    int SeparatrixIndexOfPair(int SeparatrixIndex, LeftOrRight Side);

private:
    std::vector<floating_point_type> m_Lengths;  // lengths of the Intervals between division points
    std::vector<floating_point_type> m_TempForFollowTheLeaf; // storing some often used data for fast calculation
    std::vector<floating_point_type> m_ShiftedDivPoints; // the points DivPoints[i] +/- PRECISION

    void Allocate();
    void Init(); // Initializes variables once everything is allocated and m_NumSeparatrices, m_Lengths and m_Pair are defined
    
    void AverageLengths();
    void NormalizeLengths();
    void FillInLengthsAndPair(int StartingIndex, WeighedTree::Node* pNode);
    void CountSingularities(std::vector<int>& SingularityType, int BeginInterval, int EndInterval, bool FirstCall);
    void GetSingularityType(std::vector<int>& SingularityType);
    int FindBetweenShiftedDivPoints(const CirclePoint& t, int StartIndex, int EndIndex);

    friend bool ArePairsEqual(const FoliationDisk& fd1, const FoliationDisk& fd2);
    friend bool AreLengthsEqual(const FoliationDisk& fd1, const FoliationDisk& fd2, floating_point_type AllowedError);
    friend bool AreEqual(const FoliationDisk& fd1, const FoliationDisk& fd2, floating_point_type AllowedError);
    friend bool AreEqualUpToRotation(const FoliationDisk& fd1, const FoliationDisk& fd2, floating_point_type AllowedError);
    friend bool AreEqualUpToRotationAndReflection(const FoliationDisk& fd1, const FoliationDisk& fd2, floating_point_type AllowedError);
};


void AssertRangeIsClosedUnderPair(const std::vector<int>& Pair, int begin, int end);
void ValidatePair(const std::vector<int>& Pair);


#endif