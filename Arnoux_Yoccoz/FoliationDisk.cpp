/*******************************************************************************
 *  CLASS NAME:	FoliationDisk
 *  AUTHOR:		Balazs Strenner
 *				Copyright 2013 UW-Madison. All rights reserved.
 *  DATE:		1/8/13
 *
 *	DESCRIPTION: Measured foliation on the disk, transverse to the boundary.
 *				
 *
 ******************************************************************************/

#include "FoliationDisk.h"



FoliationDisk::FoliationDisk(const WeighedTree& wt):
m_NumSeparatrices(2 * wt.m_Root->m_NumDescendants),
m_Lengths(m_NumSeparatrices),
m_Pair(m_NumSeparatrices)
{
    FillInLengthsAndPair(0, wt.m_Root);
    Init();
}



FoliationDisk::FoliationDisk(const std::vector<floating_point_type>& Lengths, const std::vector<int>& Pair) :
m_NumSeparatrices(static_cast<int>(Lengths.size())),
m_Lengths(Lengths),
m_Pair(Pair)
{
    AverageLengths();
    Init();
}



// Initializes variables once m_NumSeparatrices, m_Lengths and m_Pair are defined
void FoliationDisk::Init(){
    NormalizeLengths();
    m_DivPoints.resize(m_NumSeparatrices);
    m_DivPoints[0] = CirclePoint(0);
    for (int i = 0; i < m_NumSeparatrices - 1; i++) { // Initialize mDivPoints
        m_DivPoints[i + 1] = m_DivPoints[i] + m_Lengths[i];
    }
    m_TempForFollowTheLeaf.resize(m_NumSeparatrices);
    for (int i = 0; i < m_NumSeparatrices; i++) {
        m_TempForFollowTheLeaf[i] = m_DivPoints[m_Pair[i]].GetValue() + m_DivPoints[IncreaseIndex(i)].GetValue();
    }
}



void ValidatePair(const std::vector<int>& Pair){
    for (int i = 0; i < Pair.size(); i++) {
        if (Pair[i] == i) {
            throw "No arc can be paired with itself.";
        }
        if (Pair[Pair[i]] != i) {
            throw "The pair of a pair must be itself.";
        }
        if ((Pair[i] - i) % 2 == 0) {
            throw "There are crossing pairs. (The parity of indices of pairs must be different.)";
        }
    }

    // Finally checking that there are no crossings:
    
    AssertRangeIsClosedUnderPair(Pair, 1, Pair[0] - 1);
    AssertRangeIsClosedUnderPair(Pair, Pair[0] + 1, static_cast<int>(Pair.size()) - 1);
}



void AssertRangeIsClosedUnderPair(const std::vector<int>& Pair, int begin, int end){
    if (begin < end) {
        if (Pair[begin] < begin || Pair[begin] > end) {
            throw "There are crossing pairs.";
        }
        if (end - begin > 2) {
            AssertRangeIsClosedUnderPair(Pair, begin + 1, Pair[begin] - 1);
            AssertRangeIsClosedUnderPair(Pair, Pair[begin] + 1, end);
        }
    }
}




void FoliationDisk::FillInLengthsAndPair(int StartingIndex, WeighedTree::Node* pNode){
    int ChildrenStartingIndex = StartingIndex;
    for (int i = 0; i < pNode->m_NumChildren; i++) {
        int pair = ChildrenStartingIndex + 2 * pNode->m_Children[i].m_NumDescendants + 1;
        m_Lengths[ChildrenStartingIndex] = m_Lengths[pair] = pNode->m_Children[i].m_Weight;
        m_Pair[ChildrenStartingIndex] = pair;
        m_Pair[pair] = ChildrenStartingIndex;
        FillInLengthsAndPair(ChildrenStartingIndex + 1, pNode->m_Children + i);
        ChildrenStartingIndex = pair + 1;
    }
}


void FoliationDisk::AverageLengths(){
    for (int i = 0; i < m_NumSeparatrices; i++) {
        if (m_Pair[i] > i) {
            floating_point_type Average = (m_Lengths[i] + m_Lengths[m_Pair[i]])/2;
            m_Lengths[i] = m_Lengths[m_Pair[i]] = Average;
        }
    }
}




void FoliationDisk::NormalizeLengths(){
    floating_point_type TotalLength = 0;
    for (int i = 0; i < m_NumSeparatrices; i++) {
        TotalLength += m_Lengths[i];
    }
    for (int i = 0; i < m_NumSeparatrices; i++) {
        m_Lengths[i] /= TotalLength;
    }
}











// The point we get by following the leaf from t until we hit the boundary circle again.
CirclePoint FoliationDisk::FollowTheLeaf(const CirclePoint& t){
    return -t + m_TempForFollowTheLeaf[WhichIntervalContains(t)];
} 


int FoliationDisk::FindBetweenShiftedDivPoints(const CirclePoint& t, int StartIndex, int EndIndex){
    return 0;
}


int FoliationDisk::WhichIntervalContainsNew(const CirclePoint& t){
    int Index = FindBetweenShiftedDivPoints(t, 0, 2 * m_NumSeparatrices);
    if (Index % 2 == 1) {
        return Index/2;
    } else
        throw ExceptionThePointIsADivPoint();
}




// Find the index of the Interval that contains t
// If t is a division point and is therefore contained in two Intervals, it returns the index of the first one.
int FoliationDisk::WhichIntervalContains(const CirclePoint& t){
    int i = 0;
    while (i < m_NumSeparatrices - 1 && m_DivPoints[i + 1].GetValue() < t.GetValue()) {
        i++;
    }
    return i;
} // Efficient for small n's, under 20.



int FoliationDisk::WhichIntervalContains_ForManySeparatrices(const CirclePoint& t){  // broken!!
    int begin = 0, end = m_NumSeparatrices, temp;
    while (end - begin > 1) {   
        temp = (begin + end) / 2;
        m_DivPoints[temp].GetValue() > t.GetValue() ? end = temp : begin = temp;
    }
    return begin;
} // Efficient for larger n's





// Decides if a point is a division point
bool FoliationDisk::IsDivPoint(const CirclePoint& t){
    for (int i = 0; i < m_NumSeparatrices; i++) {
        if (m_DivPoints[i].GetValue() == t.GetValue()) {
            return true;
        }
    }
    return false;
}



void FoliationDisk::Rotate(){
    FoliationDisk fd = *this;
    
    for (int i = 1; i < m_NumSeparatrices; i++) {
        m_Lengths[i] = fd.m_Lengths[i - 1];
        m_Pair[i] = fd.m_Pair[i - 1] < m_NumSeparatrices - 1 ? fd.m_Pair[i - 1] + 1 : 0;
    }
    m_Lengths[0] = fd.m_Lengths[m_NumSeparatrices - 1];
    m_Pair[0] = fd.m_Pair[m_NumSeparatrices - 1] < m_NumSeparatrices - 1 ? fd.m_Pair[m_NumSeparatrices - 1] + 1 : 0;
    Init();
}


void FoliationDisk::Reflect(){
    FoliationDisk fd = *this;

    for (int i = 0; i < m_NumSeparatrices; i++) {
        m_Lengths[i] = fd.m_Lengths[m_NumSeparatrices - 1 - i];
        m_Pair[i] = m_NumSeparatrices - 1 - fd.m_Pair[m_NumSeparatrices - 1 - i];
    }
    Init();
}



int FoliationDisk::SeparatrixIndexOfPair(int SeparatrixIndex, LeftOrRight Side){
    return Side == RIGHT ? IncreaseIndex(m_Pair[SeparatrixIndex]) : m_Pair[DecreaseIndex(SeparatrixIndex)] ;
}


void FoliationDisk::CountSingularities(std::vector<int>& SingularityType, int BeginInterval, int EndInterval, bool FirstCall){
    int iterator = BeginInterval;
    int count = FirstCall ? 0 : 1;
    
    while (iterator != EndInterval + 1) {
        count++;
        if (iterator + 1 < m_Pair[iterator] - 1) {
            CountSingularities(SingularityType, iterator + 1, m_Pair[iterator] - 1, false);
        } else
            SingularityType.push_back(1);
        iterator = m_Pair[iterator] + 1;
    }
    SingularityType.push_back(count);
}



void FoliationDisk::GetSingularityType(std::vector<int>& SingularityType){
    SingularityType.clear();
    CountSingularities(SingularityType, 0, m_NumSeparatrices - 1, true);
    std::sort(SingularityType.begin(), SingularityType.end());
}








bool ArePairsEqual(const FoliationDisk& fd1, const FoliationDisk& fd2)
{
    if (fd1.m_NumSeparatrices != fd2.m_NumSeparatrices) {
        return false;
    }
    for (int i = 0; i < fd1.m_NumSeparatrices; i++) {
        if (fd1.m_Pair[i] != fd2.m_Pair[i]) {
            return false;
        }
    }
    return true;
}


bool AreLengthsEqual(const FoliationDisk& fd1, const FoliationDisk& fd2, floating_point_type AllowedError)
{
    if (fd1.m_NumSeparatrices != fd2.m_NumSeparatrices) {
        return false;
    }
    for (int i = 0; i < fd1.m_NumSeparatrices; i++) {
        if (fabs(fd1.m_Lengths[i] - fd2.m_Lengths[i]) > AllowedError) {
            return false;
        }
    }
    return true;
}



bool AreEqual(const FoliationDisk& fd1, const FoliationDisk& fd2, floating_point_type AllowedError){
    return  ArePairsEqual(fd1, fd2) && AreLengthsEqual(fd1, fd2, AllowedError);
}



bool AreEqualUpToRotation(const FoliationDisk& fd1, const FoliationDisk& fd2, floating_point_type AllowedError){
    if (fd1.m_NumSeparatrices != fd2.m_NumSeparatrices) {
        return false;
    }
    FoliationDisk fd = fd2;
    for (int i = 0; i < fd1.m_NumSeparatrices; i++) {
        if (ArePairsEqual(fd1, fd) && AreLengthsEqual(fd1, fd, AllowedError)) {
            return true;
        }
        fd.Rotate();
       // std::cout << fd;
    }
    return false;
}



bool AreEqualUpToRotationAndReflection(const FoliationDisk& fd1, const FoliationDisk& fd2, floating_point_type AllowedError){
    if (fd1.m_NumSeparatrices != fd2.m_NumSeparatrices) {
        return false;
    }
    FoliationDisk fd = fd2;
    if (AreEqualUpToRotation(fd1, fd, AllowedError)) {
        return true;
    }
    fd.Reflect();
    if (AreEqualUpToRotation(fd1, fd, AllowedError)) {
        return true;
    }
    return false;
}








std::ostream& operator<<(std::ostream& Out, FoliationDisk fd){
    using namespace std;

    Out << "Lengths: ";
    for (int i = 0; i < fd.m_NumSeparatrices; i++) {
        Out << fd.m_Lengths[i] << " ";
    }
    Out << endl << "Pairing: ";
    for (int i = 0; i < fd.m_NumSeparatrices; i++) {
        Out << fd.m_Pair[i] << " ";
    }
    Out << endl << "Singularity type: ";
    std::vector<int> SingularityType;
    fd.GetSingularityType(SingularityType);
    for (std::vector<int>::reverse_iterator it = SingularityType.rbegin(); it != SingularityType.rend(); it++) {
        Out << *it << " ";
    }
    Out << endl << endl;
    return Out;
}



