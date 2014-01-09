/*******************************************************************************
 *  CLASS NAME:	FoliationRP2
 *  AUTHOR:		Balazs Strenner
 *				Copyright 2013 UW-Madison. All rights reserved.
 *  DATE:		1/8/13
 *
 *	DESCRIPTION: A measured foliation on RP^2
 
    THE IDEA:   For any measured foliation on any surface there exists a simple closed curve tranverse to the foliation.
                (Take a rectangle decomposition of the foliation where the top and bottom sides of the rectangles are leaves, 
                the other two are transverse. Start from an arbitrary point, exit on the top of the rectangle, 
                and enter an other rectangle from the top of the bottom. Exit on the opposite side, and continue until 
                get back to a rectangle already visited from the same direction. This gives a transverse closed curve, 
                and buy cutting/pasting operations at the intersections we get simple curve.)

                We consider foliations of the RP^2 where there is a >>one-sided<< transverse simple closed curve. 
                (Mosty likely these are all the foliations.) Cutting along such a curve we get a foliation of the disk 
                which is transverse to the boundary. So measured foliations on RP^2 are simply represented by foliations
                on the disk with antipodal points identified. 
                In practice, the FoliationRP2 class will be the child on FoliationDisk.
 
    SEPARATING POINTS:
 
                For a foliation on a disk, we called division points the points on the boundary circle where 
                the separatrices intersect the boundary circle. If we identify the antipodal points, separatrices will 
                intersect the curve in RP^2 represented by the boundary circle infinitely many times. We call these 
                >>separating points<<. The division points are therefore the first separation points on the separatrices.
                The (i,k)'th separation point - A(i,k) - is the kth intersection of the ith separatrix with the boundary circle. 
                So A(i,1) = m_DivPoints[i], A(i,2) = m_DivPoints[i].Antipodal(), 
                A(i,3) = FollowTheLeaf(m_DivPoints[i].Antipodal()), and so on.
                (To be more precise, a point and its antipodal are the same points in RP^2, but for technical reasons,
                we count these points twice, once as one point on the boundary circle (Circle in short), and second,
                as the antipodal point.)
 
    SEPARATRIX SEGMENTS:
 
                In the ArcsAroundDivPoints.h documentation we have already talked about a motivation for that object: finding
                good separatrix segments. Here are some ideas again, with a bit more detials.
 
                There is leaf connecting the left endpoint of the i'th arc between the division points, m_DivPoints[i], 
                and the right endpoint of the m_Pair[i]'th interval, m_DivPoints[m_Pair[i]+1]], which passes though a 
                singularity. We refer to it either as s(i,-1) or s(m_Pair[i]+1], 1). Actually we imagine this leaf a little shifted 
                away from the singularity, towards our intervals. It hits the circle in points close to, 
                but on the right hand side of m_DivPoints[i] and on the left hand side of m_DivPoints[m_Pair[i]+1]]. 
                So we say it hits the circle at _DivPoints[i]+ and  m_DivPoints[m_Pair[i]+1]]-.
 
                Now we consider segments of such leaves, starting from the singularity (or, since we shifted the leaf away,
                from somewhere very close to the singularity). Such a leaf segment (or >>separatrix segment<<) exist the Disk
                through some point m_DivPoints[i]+/-, comes back at the antipodal point, and it hits the 
                boundary circle again and again. The endpoint of such a leaf segment is one of the separating points A(i,k)+/-.
                Denote this segment by L(i,k).
 
    GOOD SEPARATRIX SEGMENTS:
                
                The separatrix segment L(i,k) is called good, if A(i,k) can be connected to one of the division points 
                on the Circle without intersecting L(i,k). Good separatrix segments serve as building blocks of
                simple closed curves.
 
                Here is the idea of the algorithm for finding good separatrix segments. We start with L(i,1) which does not
                yet intersect the Circle, so it is a good segment. Then we start increasing 1 (the << Depth >> of the segment)
                calculate the new ArcAroundDivPoints object, and check if it is good. If yes, we save it, if not,
                we just skip it. We don't need to store separatrix segments that are not good.

    INTERVAL INTERSECTION COUNT:
 
                L(i,k) intersects the Circle k-1 times (i.e. Depth - 1 times). To calculate the matrix that brings the 
                parameters of the foliation from the perspective of a found transverse simple closed curve to the original parameters,
                we need to keed track of how many times each Interval intersects the separatrix segment. The first intersection
                is a division point, so it is on the boundary. Since the Interval of the first intersection is not
                well-defined, we omit that intersection from m_IntervalIntersectionCount. 
 
                So sum(m_IntervalIntersectionCount[i]) = k-2 for L(i,k).
 
                However, the SeparatrixSegmentData struct contains which separatrix it is part of (m_Separatrix), 
                which is the same as the index of the division point which is the first intersection.
 
 
 
 
 
    POSSIBLE IMPROVEMENTS:
                1. There is a lot to change for finding more complicated simple closed curves. But since 
                    the technicalities with the implementation seem very difficult, and the algorithm
                    might be really slow and not even productive, it might not be worth it.
                    (My only idea is to choose collections of good separatrix segments (possibly more from the same)
                    and check it that forms a transverse simple closed curve. There are just too many
                    combinations.)
 *				
 *
 ******************************************************************************/

#ifndef FOLIATIONRP2_H
#define FOLIATIONRP2_H

#include "FoliationDisk.h"
#include "ArcsAroundDivPoints.h"
#include <list>
#include <vector>
#include <algorithm>
#include "WeighedTree.h"
#include "AlmostPFMatrix.h"




const floating_point_type ERROR = 0.0000001;


class ExceptionFoundSaddleConnection {};
class ExceptionNoObjectFound {};





class FoliationRP2 : public FoliationDisk {
public:
	FoliationRP2(const WeighedTree& wt);
    FoliationRP2(const std::vector<floating_point_type>& Lengths, const std::vector<int>& Pair);
    void PrintGoodShiftedSeparatrixSegmentsVerbose(int Depth);
    void PrintGoodShiftedSeparatrixSegmentsConcise(int Depth);
    void PrintGoodCurves(int Depth);
    void PrintPseudoAnosovs(int Depth);

	
private:

//-------------------//
// SeparatrixSegment //
//-------------------//
    
    struct SeparatrixSegment{
        int m_Separatrix;       // which separatrix the segment belongs to
        int m_Depth;            // the depth of the segment
        CirclePoint m_Endpoint;
        ArcsAroundDivPoints m_ArcsAroundDivPoints;  // the ArcsAroundDivPoints data induced by the segment
        std::vector<int> m_IntervalIntersectionCount;           // counts how many times each Interval intersects the segment
        LeftOrRight m_Side;

        SeparatrixSegment(const std::vector<CirclePoint>& DivPoints);
        
        inline bool EndsOnFrontSideQ() const{ return m_Depth % 2 == 1; }
        inline bool ReversesOrientationQ() const{ return m_Depth % 4 == 0 || m_Depth % 4 == 3; }
        inline LeftOrRight BeginShiftWhenEndpointShiftedTo(LeftOrRight Side) const;
        inline LeftOrRight EndpointShiftWhenBeginShiftedTo(LeftOrRight Side) const { return BeginShiftWhenEndpointShiftedTo(Side); }
    };
    
//---------------------------------------//
// SeparatrixSegment (RELATED FUNCTIONS) //
//---------------------------------------//
    
    void ShiftToSide(SeparatrixSegment& s, LeftOrRight Side);
    void AddToGoodSegmentsIfGood(const SeparatrixSegment& s);
    void Lengthen(SeparatrixSegment& s);
    SeparatrixSegment& GetFirstIntersection(int SeparatrixIndex, const Arc& arc, LeftOrRight ReturnSide);
    SeparatrixSegment& GetGoodSeparatrixSegment(int SeparatrixIndex, int Depth, LeftOrRight Side);
    SeparatrixSegment& GetGoodSeparatrixSegment(SeparatrixSegment& s, LeftOrRight EndpointShiftedTo);
    friend std::ostream& operator<<(std::ostream& Out, const SeparatrixSegment& data);

    
    
//--------------//
// RotationData //
//--------------//
    
    struct RotationData{
        bool m_Reflection;
        int m_Rotation;
    };
    

    
//---------------------//
// BigTransitionMatrix //
//---------------------//
    
    
    class BigTransitionMatrix{
    public:
        BigTransitionMatrix(int Size) : m_Matrix(Size, std::vector<int>(Size, 0)), m_Size(Size) {}
        
        inline int GetEntry(int x, int y) const { return m_Matrix[x][y]; }
        inline void UpdateRow(int Row, const std::vector<int>& AmountOfChange, int HowManyTimes);
        inline void UpdateTransitionMatrix(const SeparatrixSegment& s, int SmallIntervalIndex);
        inline void UpdateTransitionMatrix(const SeparatrixSegment& s, const std::vector<int>& AmountOfChange);
        inline void Rotate();
        inline void Reflect();
    private:
        std::vector<std::vector<int>> m_Matrix;
        int m_Size;
    };
  
    
//----------------//
// TransitionData //
//----------------//
    
    
    struct TransitionData{
        FoliationDisk m_NewFoliationDisk;
        BigTransitionMatrix m_TransitionMatrix;
        
        inline void Rotate();
        inline void Reflect();
        void Rotate(RotationData r);
    };
    AlmostPFMatrix GetSmallMatrix(const TransitionData& td);

    
    
//-------------------//
// GoodOneSidedCurve //
//-------------------//
    
    
    struct GoodOneSidedCurve{
        SeparatrixSegment m_SegmentShiftedToLeft;
        SeparatrixSegment m_SegmentShiftedToRight;
        Arc m_ConnectingArc;

        GoodOneSidedCurve(const SeparatrixSegment& SegmentShiftedToLeft,
                          const SeparatrixSegment& SegmentShiftedToRight,
                          const Arc& ConnectingArc);
    };

    
//----------------------------------------//
// GoodOneSidedCurve  (RELATED FUNCTIONS) //
//----------------------------------------//
    
    bool AreDepthsGoodForOneSidedCurve(int Depth1, int Depth2);
    Arc GetClosingArcIfGoodOneSidedCurve(const SeparatrixSegment& Segment1, const SeparatrixSegment& Segment2);
    GoodOneSidedCurve GetGoodOneSidedCurve(const SeparatrixSegment& SegmentShiftedToLeft, const SeparatrixSegment& SegmentShiftedToRight);
    TransitionData CutAlongCurve(const GoodOneSidedCurve& GoodCurve);
    void Save(const GoodOneSidedCurve& GoodCurve);
    friend bool operator==(const GoodOneSidedCurve& c1, const GoodOneSidedCurve& c2);
    friend std::ostream& operator<<(std::ostream& Out, const GoodOneSidedCurve& data);
    static bool Compare(const GoodOneSidedCurve& c1, const GoodOneSidedCurve& c2);
    
    
    

    
// MEMBER VARIABLES
    
    std::vector<SeparatrixSegment> m_CurrentSepSegments; // stores information about the current separatrix segment
    std::vector<std::vector<std::list<SeparatrixSegment>>> m_GoodShiftedSeparatrixSegments; // stores good separating segments
    std::list<GoodOneSidedCurve> m_GoodOneSidedCurves;
    std::vector<char> m_FoundSaddleConnectionQ; // for each separatrix, shows if we have already found a saddle connection

// INITIALIZING
    
    void Init(); // Initializes varibles that do not belong to the parent
    
// MEMBER FUNCTIONS
    
    
    void CheckSelfSimilarity(int SeparatrixIndex1, int Depth1, LeftOrRight Side, int Depth2, RotationData r);
    void GenerateGoodShiftedSeparatrixSegments(int Depth);
    void GenerateGoodCurves(int Depth);
    void FindNewPseudoAnosovs(const GoodOneSidedCurve& GoodCurve);

    
    
   // void PrintMatrix(const std::vector<std::vector<int>>& Matrix);
};



#endif