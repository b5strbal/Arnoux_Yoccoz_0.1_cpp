/*******************************************************************************
 *  CLASS NAME:	FoliationRP2
 *  AUTHOR:		Balazs Strenner
 *				Copyright 2013 UW-Madison. All rights reserved.
 *  DATE:		1/8/13
 *
 *	DESCRIPTION:
 *				
 *
 ******************************************************************************/


#include "FoliationRP2.h"





/**************************************************************************************************************/
/*                                                                                                            */
/*                                         FoliationRP2::SeparatrixSegment                                    */
/*                                                                                                            */
/**************************************************************************************************************/


FoliationRP2::SeparatrixSegment::SeparatrixSegment(const std::vector<CirclePoint>& DivPoints) :
    m_IntervalIntersectionCount(DivPoints.size(), 0),
    m_Depth(1),
    m_ArcsAroundDivPoints(DivPoints),
    m_Side(CENTER)
{
}






LeftOrRight FoliationRP2::SeparatrixSegment::BeginShiftWhenEndpointShiftedTo(LeftOrRight Side) const{
    return (!ReversesOrientationQ() && Side == RIGHT) || (ReversesOrientationQ() && Side == LEFT) ? RIGHT : LEFT;
}





void FoliationRP2::ShiftToSide(SeparatrixSegment& s, LeftOrRight Side){
    assert(s.m_Side == CENTER);
    assert(Side != CENTER);
    
    s.m_Side = Side;
    s.m_Endpoint.Shift(s.EndpointShiftWhenBeginShiftedTo(Side));
    s.m_IntervalIntersectionCount[s.m_Side == LEFT ? DecreaseIndex(s.m_Separatrix) : s.m_Separatrix]++;
    if (s.EndsOnFrontSideQ() && s.m_Depth > 1) {
        s.m_IntervalIntersectionCount[WhichIntervalContains(s.m_Endpoint)]++;
    }
    if (s.m_Depth > 1) {
        s.m_ArcsAroundDivPoints.InsertPoint(m_DivPoints[s.m_Separatrix].ShiftedTo(Side));
    }
}






std::ostream& operator<<(std::ostream& Out, const FoliationRP2::SeparatrixSegment& s)
{    
    Out << "Separatrix Index: " << s.m_Separatrix << std::endl;
    Out << "Depth: " << s.m_Depth << std::endl;
    Out << "Endpoint: " << s.m_Endpoint << std::endl;
    Out << "Shifted to: ";
    switch (s.m_Side) {
        case LEFT:
            Out << "LEFT";
            break;
            
        case RIGHT:
            Out << "RIGHT";
            break;
        case CENTER:
            Out << "CENTER";
            break;
    }
    Out << std::endl;
    Out << "ArcsAroundDivpoints:\n" << s.m_ArcsAroundDivPoints;
    Out << "IntervalIntersectionCount: ";
    for (int i = 0; i < s.m_IntervalIntersectionCount.size(); i++) {
        Out << s.m_IntervalIntersectionCount[i] << " ";
    }
    Out << std::endl;
    return Out;
}





/**************************************************************************************************************/
/*                                                                                                            */
/*                              FoliationRP2::SeparatrixSegment (RELATED FUNCTIONS)                           */
/*                                                                                                            */
/**************************************************************************************************************/





void FoliationRP2::AddToGoodSegmentsIfGood(const SeparatrixSegment& s){
    if (s.m_ArcsAroundDivPoints.ContainsQ(s.m_Endpoint)){
        for (LeftOrRight Side = LEFT; Side < 2; Side++) {
            m_GoodShiftedSeparatrixSegments[s.m_Separatrix][Side].push_back(s);
            std::list<SeparatrixSegment>::reverse_iterator it = m_GoodShiftedSeparatrixSegments[s.m_Separatrix][Side].rbegin();
            ShiftToSide(*it, Side);
            if (!it->m_ArcsAroundDivPoints.ContainsQ(it->m_Endpoint))
                m_GoodShiftedSeparatrixSegments[s.m_Separatrix][Side].pop_back();
        }
    }
}






void FoliationRP2::Lengthen(SeparatrixSegment& s){
    if (s.m_Depth > 1) {
        s.m_IntervalIntersectionCount[WhichIntervalContains(s.m_Endpoint)]++;
        s.m_ArcsAroundDivPoints.InsertPoint(s.m_Endpoint); // inserting the new intersection point to m_ArcsAroundDivPoints
    }
    s.m_Endpoint = s.EndsOnFrontSideQ() ? s.m_Endpoint.Antipodal() : FollowTheLeaf(s.m_Endpoint);
    s.m_Depth++;
    
    // Checking if the new endpoint is a division point in which case we reached a saddle connection
    if (IsDivPoint(s.m_Endpoint)) {
        m_FoundSaddleConnectionQ[s.m_Separatrix] = 1;
    } else {
        AddToGoodSegmentsIfGood(s);
    }
}















FoliationRP2::SeparatrixSegment& FoliationRP2::GetGoodSeparatrixSegment(int SeparatrixIndex, int Depth, LeftOrRight Side){
    if (m_CurrentSepSegments[SeparatrixIndex].m_Depth >= Depth) {
        for (std::list<SeparatrixSegment>::iterator it = m_GoodShiftedSeparatrixSegments[SeparatrixIndex][Side].begin();
             it != m_GoodShiftedSeparatrixSegments[SeparatrixIndex][Side].end(); it++) {
            if(it->m_Depth == Depth)
                return *it;
        }
        throw ExceptionNoObjectFound();
    } else {
        while (m_CurrentSepSegments[SeparatrixIndex].m_Depth < Depth) {
            Lengthen(m_CurrentSepSegments[SeparatrixIndex]);
        }
        if (m_GoodShiftedSeparatrixSegments[SeparatrixIndex][Side].back().m_Depth == Depth) {
            return m_GoodShiftedSeparatrixSegments[SeparatrixIndex][Side].back();
        } else
            throw ExceptionNoObjectFound();
    }
}




FoliationRP2::SeparatrixSegment& FoliationRP2::GetGoodSeparatrixSegment(SeparatrixSegment& s, LeftOrRight EndpointShiftedTo){
    if (s.EndpointShiftWhenBeginShiftedTo(s.m_Side) == EndpointShiftedTo) {
        return s;
    }
    return GetGoodSeparatrixSegment(s.m_Separatrix, s.m_Depth, Opposite(s.m_Side));
}







FoliationRP2::SeparatrixSegment& FoliationRP2::GetFirstIntersection(int SeparatrixIndex, const Arc& arc, const LeftOrRight ReturnSide){
    for(std::list<SeparatrixSegment>::iterator it = m_GoodShiftedSeparatrixSegments[SeparatrixIndex][ReturnSide].begin();
        it != m_GoodShiftedSeparatrixSegments[SeparatrixIndex][ReturnSide].end(); it++){
        if (arc.ContainsQ(it->m_Endpoint.Centered())) {
            return *it;
        }
    }
    while (!arc.ContainsQ(m_GoodShiftedSeparatrixSegments[SeparatrixIndex][ReturnSide].back().m_Endpoint.Centered())) {
        Lengthen(m_CurrentSepSegments[SeparatrixIndex]);
        if(m_FoundSaddleConnectionQ[SeparatrixIndex]){ // if any time along the way a saddle connection is found by FindNextSeparatrixSegment,
            // the function is terminated and false is returned
            throw ExceptionFoundSaddleConnection(); // SADDLE CONNECTION
        }
    }
    return m_GoodShiftedSeparatrixSegments[SeparatrixIndex][ReturnSide].back();
}












/**************************************************************************************************************/
/*                                                                                                            */
/*                                         FoliationRP2::BigTransitionMatrix                                  */
/*                                                                                                            */
/**************************************************************************************************************/





void FoliationRP2::BigTransitionMatrix::UpdateTransitionMatrix(const SeparatrixSegment& s, int SmallIntervalIndex)
{
    std::vector<int> AmountOfChange(m_Size, 0);
    AmountOfChange[SmallIntervalIndex] = 1;
    UpdateTransitionMatrix(s, AmountOfChange);
}







void FoliationRP2::BigTransitionMatrix::UpdateTransitionMatrix(const SeparatrixSegment& s, const std::vector<int>& AmountOfChange)
{
    for (int LongIntervalIndex = 0; LongIntervalIndex < m_Size; LongIntervalIndex++) {
        UpdateRow(LongIntervalIndex, AmountOfChange, s.m_IntervalIntersectionCount[LongIntervalIndex]);
    }    
}






void FoliationRP2::BigTransitionMatrix::UpdateRow(int Row, const std::vector<int>& AmountOfChange, int HowManyTimes){
    for (int ShortIntervalIndex = 0; ShortIntervalIndex < m_Size; ShortIntervalIndex++) {
        m_Matrix[Row][ShortIntervalIndex] += AmountOfChange[ShortIntervalIndex] * HowManyTimes;
    }
}






void FoliationRP2::BigTransitionMatrix::Rotate(){
    for (int i = 0; i < m_Matrix.size(); i++) {
        std::rotate(m_Matrix[i].begin(), m_Matrix[i].end() - 1, m_Matrix[i].end());
    }
}






void FoliationRP2::BigTransitionMatrix::Reflect(){
    for (int i = 0; i < m_Matrix.size(); i++) {
        std::reverse(m_Matrix[i].begin(), m_Matrix[i].end());
    }
}









FoliationRP2::GoodOneSidedCurve::GoodOneSidedCurve(const SeparatrixSegment& SegmentShiftedToLeft,
                                                   const SeparatrixSegment& SegmentShiftedToRight, const Arc& ConnectingArc) :
    m_SegmentShiftedToLeft(SegmentShiftedToLeft),
    m_SegmentShiftedToRight(SegmentShiftedToRight),
    m_ConnectingArc(ConnectingArc)
{
}



bool operator==(const FoliationRP2::GoodOneSidedCurve& c1, const FoliationRP2::GoodOneSidedCurve& c2){
    return c1.m_SegmentShiftedToLeft.m_Separatrix == c2.m_SegmentShiftedToLeft.m_Separatrix &&
    c1.m_SegmentShiftedToRight.m_Separatrix == c2.m_SegmentShiftedToRight.m_Separatrix &&
    c1.m_SegmentShiftedToLeft.m_Depth == c2.m_SegmentShiftedToLeft.m_Depth &&
    c1.m_SegmentShiftedToRight.m_Depth == c2.m_SegmentShiftedToRight.m_Depth;
}





/**************************************************************************************************************/
/*                                                                                                            */
/*                            FoliationRP2::GoodOneSidedCurve (RELATED FUNCTIONS)                             */
/*                                                                                                            */
/**************************************************************************************************************/




FoliationRP2::TransitionData FoliationRP2::CutAlongCurve(const GoodOneSidedCurve& GoodCurve){
    std::vector<CirclePoint> DivPointsUnordered(m_NumSeparatrices);
    std::vector<SeparatrixSegment*> Segment(m_NumSeparatrices);
    
    
    floating_point_type NewCircumference = 2 * GoodCurve.m_ConnectingArc.GetLength();
    const CirclePoint& LeftEndpoint = GoodCurve.m_ConnectingArc.GetLeftEndpoint();
    for (int i = 0; i < m_NumSeparatrices; i++) {
        Segment[i] = &GetFirstIntersection(i, GoodCurve.m_ConnectingArc, RIGHT);
        if (Segment[i]->EndsOnFrontSideQ())
            DivPointsUnordered[i] = CirclePoint(DistanceBetween(LeftEndpoint, Segment[i]->m_Endpoint)/NewCircumference);
        else
            DivPointsUnordered[i] = CirclePoint(DistanceBetween(LeftEndpoint, Segment[i]->m_Endpoint)/NewCircumference + 0.5);
    }
    
    std::vector<CirclePoint> DivPoints(DivPointsUnordered);
    std::vector<floating_point_type> Lengths(m_NumSeparatrices);
    std::vector<int> Pair(m_NumSeparatrices);
    
    
    std::sort(DivPoints.begin(), DivPoints.begin() + m_NumSeparatrices);
    

    
    std::vector<int> SeparatrixIndexOfNewDivPoint(m_NumSeparatrices);
    
    int LastDivPointBeforeHalf = -1;
    
    for (int i = 0; i < m_NumSeparatrices; i++) {
        Lengths[i] = DistanceBetween(DivPoints[i], DivPoints[IncreaseIndex(i)]);
        
        SeparatrixIndexOfNewDivPoint[i] = static_cast<int>(std::find(DivPointsUnordered.begin(), DivPointsUnordered.begin() + m_NumSeparatrices, DivPoints[i]) - DivPointsUnordered.begin()) ;

        if (DivPoints[i] < 0.5) {
            LastDivPointBeforeHalf++;
        }
    }

    
    for (int i = 0; i < m_NumSeparatrices; i++) {
        int Index = SeparatrixIndexOfNewDivPoint[i];
        int IndexOfPair = Segment[Index]->ReversesOrientationQ() ? m_Pair[DecreaseIndex(Index)] : IncreaseIndex(m_Pair[Index]);
        
        for (int j = 0; j < m_NumSeparatrices; j++) {
            if (SeparatrixIndexOfNewDivPoint[j] == IndexOfPair) {
                Pair[i] = Segment[Index]->ReversesOrientationQ() == Segment[IndexOfPair]->ReversesOrientationQ() ? DecreaseIndex(j) : j;
                break;
            }
        }
    }
    
    
    
    BigTransitionMatrix Matrix(m_NumSeparatrices);
    
    
    
    for (int ShortIntervalIndex = 0; ShortIntervalIndex < m_NumSeparatrices; ShortIntervalIndex += 2) {
        if (ShortIntervalIndex != LastDivPointBeforeHalf && Pair[ShortIntervalIndex] != LastDivPointBeforeHalf) {
            const SeparatrixSegment& s1 = GetGoodSeparatrixSegment(*Segment[SeparatrixIndexOfNewDivPoint[ShortIntervalIndex + 1]], LEFT);
            const SeparatrixSegment& s2 = GetGoodSeparatrixSegment(*Segment[SeparatrixIndexOfNewDivPoint[Pair[ShortIntervalIndex]]], RIGHT);
            Matrix.UpdateTransitionMatrix(s1, ShortIntervalIndex);
            Matrix.UpdateTransitionMatrix(s2, ShortIntervalIndex);
            //PrintMatrix(Matrix);
        } else
        {
            int BadInterval = ShortIntervalIndex == LastDivPointBeforeHalf ? ShortIntervalIndex : Pair[ShortIntervalIndex];
            int GoodInterval = Pair[BadInterval];
            
            // SeparatrixSegment* LeftSegment_BadInterval = Segment[SeparatrixIndexOfNewDivPoint[BadInterval]];
            const SeparatrixSegment& RightSegment_BadInterval = GetGoodSeparatrixSegment(*Segment[SeparatrixIndexOfNewDivPoint[BadInterval + 1]],LEFT);
            const SeparatrixSegment& LeftSegment_GoodInterval = GetGoodSeparatrixSegment(*Segment[SeparatrixIndexOfNewDivPoint[GoodInterval]], RIGHT);
            
            Arc LengthenedArc(GoodCurve.m_ConnectingArc.GetLeftEndpoint(), GoodCurve.m_ConnectingArc.GetRightEndpoint().Centered());
            const SeparatrixSegment& MiddleSegment_BadInterval = GetGoodSeparatrixSegment(GetFirstIntersection(m_Pair[DecreaseIndex(SeparatrixIndexOfNewDivPoint[0])] , LengthenedArc, LEFT), LEFT);
            
            const SeparatrixSegment& FirstSegment = GetGoodSeparatrixSegment(*Segment[SeparatrixIndexOfNewDivPoint[0]], LEFT);
            
            std::vector<int> IncompleteIntervalContribution(m_NumSeparatrices, 0);
            
            for (int i = 0; i < m_NumSeparatrices; i += 2) {
                IncompleteIntervalContribution[i] = 1;
            }
            for (int i = 0; i < LastDivPointBeforeHalf; i++) {
                IncompleteIntervalContribution[i]--;
            }
            
            Matrix.UpdateTransitionMatrix(MiddleSegment_BadInterval, IncompleteIntervalContribution);
            Matrix.UpdateTransitionMatrix(FirstSegment, IncompleteIntervalContribution);
            
            Matrix.UpdateTransitionMatrix(LeftSegment_GoodInterval, ShortIntervalIndex);
            Matrix.UpdateTransitionMatrix(RightSegment_BadInterval, ShortIntervalIndex);
            //PrintMatrix(Matrix);
        }
    }
    
    
    
    TransitionData td = { FoliationDisk(Lengths, Pair), Matrix};
    return td;
}











void FoliationRP2::FindNewPseudoAnosovs(const GoodOneSidedCurve& GoodCurve){
    try {
        TransitionData td = CutAlongCurve(GoodCurve);
        for (int Round = 0; Round < 2; Round++) {
            for (int i = 0; i < m_NumSeparatrices; i++) {
                if (ArePairsEqual(td.m_NewFoliationDisk, *this)) {
                    AlmostPFMatrix Matrix(GetSmallMatrix(td));
                    // std::cout << Matrix << std::endl << std::endl;
                    
                    if (Matrix.IsPerronFrobenius()) {
                        std::vector<floating_point_type> SmallLengthVector = Matrix.GetPFEigenvector();
                        
                        std::vector<floating_point_type> NewLengths(m_NumSeparatrices);
                        for (int j = 0; j < m_NumSeparatrices/2; j++) {
                            NewLengths[2 * j] = SmallLengthVector[j];
                            NewLengths[m_Pair[2 * j]] = SmallLengthVector[j];
                        }
                        FoliationRP2 NewCandidate(NewLengths, m_Pair);
                        RotationData r = {Round == 0 ? false : true, i};
                        
                        NewCandidate.CheckSelfSimilarity(GoodCurve.m_SegmentShiftedToLeft.m_Separatrix, GoodCurve.m_SegmentShiftedToLeft.m_Depth, LEFT, GoodCurve.m_SegmentShiftedToRight.m_Depth, r);
                    }
                }
                td.Rotate();
            }
            td.Reflect();
        }
    } catch (const ExceptionFoundSaddleConnection&) {
        std::cout << "*** Could not complete search for pseudo-anosovs due to found saddle connection. ***\n";
    }
    
}






Arc FoliationRP2::GetClosingArcIfGoodOneSidedCurve(const SeparatrixSegment& Segment1, const SeparatrixSegment& Segment2){
    ArcsAroundDivPoints Intersection = Intersect(Segment1.m_ArcsAroundDivPoints, Segment2.m_ArcsAroundDivPoints);
    
    Arc Arc1(Segment1.m_Endpoint, Segment2.m_Endpoint);
    if(Intersection.ContainsArcThroughADivPointQ(Arc1)){
        return Arc1;
    }
    Arc Arc2(Segment2.m_Endpoint, Segment1.m_Endpoint);
    if (Intersection.ContainsArcThroughADivPointQ(Arc2)){
        return Arc2;
    }
    throw ExceptionNoObjectFound();
}








bool FoliationRP2::AreDepthsGoodForOneSidedCurve(int Depth1, int Depth2){
    if (Depth1 % 2 == 1 && abs(Depth1 - Depth2) % 4 == 2) {
        return true;
    }
    return false;
}







FoliationRP2::GoodOneSidedCurve FoliationRP2::GetGoodOneSidedCurve(const SeparatrixSegment& SegmentShiftedToLeft,
                                                                    const SeparatrixSegment& SegmentShiftedToRight){
    if (AreDepthsGoodForOneSidedCurve(SegmentShiftedToRight.m_Depth, SegmentShiftedToLeft.m_Depth)) {
        Arc myArc = GetClosingArcIfGoodOneSidedCurve(SegmentShiftedToRight, SegmentShiftedToLeft);
        if (myArc.GetLeftEndpoint().IsShiftedTo(LEFT)) {
            return GoodOneSidedCurve(SegmentShiftedToLeft, SegmentShiftedToRight, myArc);
        }
    }
    throw ExceptionNoObjectFound();
}








bool FoliationRP2::Compare(const GoodOneSidedCurve& c1, const GoodOneSidedCurve& c2){
    if (c1.m_ConnectingArc.GetLength() > c2.m_ConnectingArc.GetLength()) {
        return true;
    }
    return false;
}








std::ostream& operator<<(std::ostream& Out, const FoliationRP2::GoodOneSidedCurve& GoodCurve)
{    
    Out << "Separatrix segments (separatrix index, depth, endpoint): (" << GoodCurve.m_SegmentShiftedToLeft.m_Separatrix << ", "
    << GoodCurve.m_SegmentShiftedToLeft.m_Depth << ", " << GoodCurve.m_SegmentShiftedToLeft.m_Endpoint << "), (" <<
    GoodCurve.m_SegmentShiftedToRight.m_Separatrix << ", "
    << GoodCurve.m_SegmentShiftedToRight.m_Depth << ", " << GoodCurve.m_SegmentShiftedToRight.m_Endpoint << ")" << std::endl;
    Out << " Length of its double: " << 2 * GoodCurve.m_ConnectingArc.GetLength()
    << " Connecting Arc: " << GoodCurve.m_ConnectingArc.GetLeftEndpoint() << ", " << GoodCurve.m_ConnectingArc.GetRightEndpoint();
    return Out;
}








/**************************************************************************************************************/
/*                                                                                                            */
/*                                      FoliationRP2::TransitionData                                          */
/*                                                                                                            */
/**************************************************************************************************************/



void FoliationRP2::TransitionData::Rotate(){
    m_NewFoliationDisk.Rotate();
    m_TransitionMatrix.Rotate();
}




void FoliationRP2::TransitionData::Reflect(){
    m_NewFoliationDisk.Reflect();
    m_TransitionMatrix.Reflect();
}



void FoliationRP2::TransitionData::Rotate(RotationData r){
    if (r.m_Reflection == true) {
        Reflect();
    }
    for (int i = 0; i < r.m_Rotation; i++) {
        Rotate();
    }
}



/**************************************************************************************************************/
/*                                                                                                            */
/*                              FoliationRP2::TransitionData (RELATED FUNCTIONS)                              */
/*                                                                                                            */
/**************************************************************************************************************/





AlmostPFMatrix FoliationRP2::GetSmallMatrix(const TransitionData& td){
    int Size = m_NumSeparatrices/2;
    std::vector<std::vector<int>> Matrix(Size, std::vector<int>(Size));

    for (int i = 0; i < Size; i++) {
        for (int j = 0; j < Size; j++) {
            Matrix[i][j] = td.m_TransitionMatrix.GetEntry(2 * i, 2 * j) + td.m_TransitionMatrix.GetEntry(2 * i, td.m_NewFoliationDisk.GetPair(2 * j));
        }
    }
    return AlmostPFMatrix(Matrix);
}













/**************************************************************************************************************/
/*                                                                                                            */
/*                                                  FoliationRP2                                              */
/*                                                                                                            */
/**************************************************************************************************************/





FoliationRP2::FoliationRP2(const WeighedTree& wt) :
    FoliationDisk(wt)
{
    Init();
}




FoliationRP2::FoliationRP2(const std::vector<floating_point_type>& Lengths, const std::vector<int>& Pair) :
    FoliationDisk(Lengths, Pair)
{
    Init();
}







void FoliationRP2::Init(){
    m_GoodShiftedSeparatrixSegments.resize(m_NumSeparatrices);
    m_FoundSaddleConnectionQ.resize(m_NumSeparatrices, 0);
    m_CurrentSepSegments.resize(m_NumSeparatrices, m_DivPoints);

    
    for (int i = 0; i < m_NumSeparatrices; i++) {
        m_GoodShiftedSeparatrixSegments[i].resize(2);
    }
	
	for (int i = 0; i < m_NumSeparatrices; i++) { 
        m_CurrentSepSegments[i].m_Separatrix = i; 
        m_CurrentSepSegments[i].m_Endpoint = m_DivPoints[i];
        
        AddToGoodSegmentsIfGood(m_CurrentSepSegments[i]);
    }
}


















void FoliationRP2::PrintGoodShiftedSeparatrixSegmentsVerbose(int Depth){
    GenerateGoodShiftedSeparatrixSegments(Depth);
    for (int i = 0; i < m_NumSeparatrices; i++) {
        for (LeftOrRight Side = LEFT; Side < 2; Side++) {
            for(std::list<SeparatrixSegment>::iterator it = m_GoodShiftedSeparatrixSegments[i][Side].begin(); it != m_GoodShiftedSeparatrixSegments[i][Side].end() && it->m_Depth < Depth; it++)
            {
                std::cout << *it << std::endl;
            }
        }
    }
}



void FoliationRP2::PrintGoodShiftedSeparatrixSegmentsConcise(int Depth){
    using namespace std;
    GenerateGoodShiftedSeparatrixSegments(Depth);

    for (int i = 0; i < m_NumSeparatrices; i++) {
        cout << "SeparatrixIndex: " << i << "\n";
        for (LeftOrRight Side = LEFT; Side < 2; Side++) {
            cout << (Side == LEFT ? "LEFT: " : "RIGHT: ");
            for(std::list<SeparatrixSegment>::iterator it = m_GoodShiftedSeparatrixSegments[i][Side].begin(); it != m_GoodShiftedSeparatrixSegments[i][Side].end() && it->m_Depth < Depth; it++)
            {
                if (it->m_Depth % 2 == 1) {
                    std::cout << it->m_Depth << " ";
                }
            }
            cout << "\n";
        }
        cout << "\n";
    }
}




void FoliationRP2::PrintGoodCurves(int Depth){
    GenerateGoodCurves(Depth);
    for (std::list<GoodOneSidedCurve>::iterator it = m_GoodOneSidedCurves.begin(); it != m_GoodOneSidedCurves.end(); it++) {
        std::cout << *it << std::endl;
    }
}



void FoliationRP2::GenerateGoodShiftedSeparatrixSegments(int Depth){
    for (int SeparatrixIndex = 0; SeparatrixIndex < m_NumSeparatrices; SeparatrixIndex++) {
        while (!m_FoundSaddleConnectionQ[SeparatrixIndex] && m_CurrentSepSegments[SeparatrixIndex].m_Depth < Depth) {
            Lengthen(m_CurrentSepSegments[SeparatrixIndex]);
        }
    }
}



void FoliationRP2::Save(const GoodOneSidedCurve& GoodCurve){
    bool AlreadySaved = false;
    for (std::list<GoodOneSidedCurve>::iterator it = m_GoodOneSidedCurves.begin(); it != m_GoodOneSidedCurves.end(); it++) {
        if (*it == GoodCurve) {
            AlreadySaved = true;
            break;
        }
    }
    if (!AlreadySaved) {
        m_GoodOneSidedCurves.push_back(GoodCurve);
    }
}



void FoliationRP2::GenerateGoodCurves(int Depth){
    GenerateGoodShiftedSeparatrixSegments(Depth);
    for (int SeparatrixIndex = 0; SeparatrixIndex < m_NumSeparatrices; SeparatrixIndex++) {
        int IndexOfPair = SeparatrixIndexOfPair(SeparatrixIndex, RIGHT);
        for (std::list<SeparatrixSegment>::iterator it1 = m_GoodShiftedSeparatrixSegments[SeparatrixIndex][RIGHT].begin();
             it1 != m_GoodShiftedSeparatrixSegments[SeparatrixIndex][RIGHT].end(); it1++) {
            for (std::list<SeparatrixSegment>::iterator it2 = m_GoodShiftedSeparatrixSegments[IndexOfPair][LEFT].begin();
                 it2 != m_GoodShiftedSeparatrixSegments[IndexOfPair][LEFT].end(); it2++) {
                try {
                    Save(GetGoodOneSidedCurve(*it2, *it1));
                }
                catch (const ExceptionNoObjectFound&) {}
                catch (const ExceptionFoundSaddleConnection&) {}
            }
        }
    }
    m_GoodOneSidedCurves.sort(Compare);
}






void FoliationRP2::PrintPseudoAnosovs(int Depth){
    using namespace std;
    GenerateGoodCurves(Depth);
    std::list<GoodOneSidedCurve>::iterator it;
    int count = 1;
    for (it = m_GoodOneSidedCurves.begin(); it != m_GoodOneSidedCurves.end() && it->m_SegmentShiftedToLeft.m_Depth <= Depth && it->m_SegmentShiftedToRight.m_Depth <= Depth; ++it) {
        cout << "***************************************\n";
        cout << "***************************************\n";
        cout << "***************************************\n\n";
        cout << "Reference curve " << count << ":" << endl;
        cout << *it << endl << endl;
       // cout << "We found the foliations with pseudo-anosovs:" << endl << endl;
        FindNewPseudoAnosovs(*it);
        cout << endl;
        count++;
    }
}











/*
void FoliationRP2::PrintMatrix(const std::vector<std::vector<int>>& Matrix){
    for (int i = 0; i < Matrix.size(); i++) {
        for (int j = 0; j < Matrix.size(); j++) {
            std::cout << Matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

}
*/









void FoliationRP2::CheckSelfSimilarity(int SeparatrixIndex1, int Depth1, LeftOrRight Side1, int Depth2, RotationData r){
    try {
        int SeparatrixIndex2 = SeparatrixIndexOfPair(SeparatrixIndex1, Side1);
        LeftOrRight Side2 = Opposite(Side1);
        const SeparatrixSegment& s1 = GetGoodSeparatrixSegment(SeparatrixIndex1, Depth1, Side1);
        const SeparatrixSegment& s2 = GetGoodSeparatrixSegment(SeparatrixIndex2, Depth2, Side2);
        const GoodOneSidedCurve& GoodCurve = GetGoodOneSidedCurve(s1, s2);
        TransitionData td = CutAlongCurve(GoodCurve);
        td.Rotate(r);
        if (AreEqual(*this, td.m_NewFoliationDisk, ERROR)) {
            AlmostPFMatrix SmallMatrix = GetSmallMatrix(td);
            if (SmallMatrix.IsPerronFrobenius()) {
                std::cout << "\n\n****** NEW FOLIATION ****** " << std::endl << *this;
                std::cout << "Cutting curve: " << GoodCurve << std::endl;
                std::cout << "Pseudo-anosov stretch factor: " << SmallMatrix.GetPFEigenvalue() << std::endl;
                std::cout << "Corresponding Perron-Frobenius matrix: " << std::endl << GetSmallMatrix(td) << "\n\n";
            }
        }
    }
    catch (const ExceptionNoObjectFound&) {}
    catch (const ExceptionFoundSaddleConnection&) {}
}














