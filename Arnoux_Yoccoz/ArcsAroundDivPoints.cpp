/*******************************************************************************
 *  CLASS NAME:	ArcsAroundDivPoints
 *  AUTHOR:		Balazs Strenner
 *				Copyright 2013 UW-Madison. All rights reserved.
 *  DATE:		1/8/13
 *				
 *
 ******************************************************************************/


#include "ArcsAroundDivPoints.h"






// Decides of a point if contained in any of the Arcs around the division points
bool ArcsAroundDivPoints::ContainsQ(const CirclePoint& c) const{
    if (!IsEmpty()) {
        
        // We go over each Arc until we find one that contains t.
        for (int i = 0; i < m_DivPoints.size(); i++) {
                if (m_Arcs[i].ContainsQ(c)) {
                    return true;
            }
        }
    } else
    {
        return true; // For an empty object, there is the whole Circle around each point, therefore it contains everything.
    }
    return false; // In case we didn't find any, we return false.
}




// Inserts a new intersection point 
void ArcsAroundDivPoints::InsertPoint(const CirclePoint& NewIntersectionPoint){
    /*  The function works for all ArcsAroundDivPoints objects and for all NewIntersectionPoint as input, but there is
        one thing worth mentioning. Computers only have limited precision for floating point numbers, so in certain cases
        it might happen that the function doesn't do what we want.
     
        For example, suppose that in the current "this" object m_NumArcs[0] is 1, so there is one Arc around m_DivPoints[0].
        If we insert m_DivPoints[0], then this Arc will be divided in two at m_DivPoints[0], one of them will be on the left
        side, the other on the right side. But the sum of the lengths of these two new Arcs is the same as the length of the 
        original Arc.
     
        Now suppose that in the same situation we insert a point very close to m_DivPoints[0], but different from it. In this 
        case, the further part of the Arc will be cut off, and we will still have one arc around m_DivPoints[0], but this
        will be shorter than the original. So if our different point is so close that the computer can't distinguish it
        from m_DivPoints[0], we can get unexpected results.
     
        This caveat can't be handled in this function because sometimes we do need to insert points that are exactly division
        points (e.g. taking intersections of ArcsAroundDivPoints objects), so we can't just give an error message if 
        the new intersection point is too close to some division point.
     
        So when this function is called, usually it is known that NewIntersectionPoint can't be a division point unless the
        the foliation has a saddle connection, so it has to be checked there. But even if NewIntersectionPoint equals a 
        division point as a floating point number, we can't be absolutely sure that there is a saddle connection, it might
        just be the limitation of floating point precision, so instead of throwing away the foliation completely, the data 
        before the point of the suspicion of a saddle connection should be used for calculations.
     
        (Not to mention that since only finitely many different floating point numbers exist with
        a fixed precision, by taking longer and longer segments on a separatrix eventually it will always look like
        there is a saddle connection.)
    */
    if (IsEmpty()) {
        m_Arcs.resize(m_DivPoints.size(), Arc(NewIntersectionPoint));

    } else { // if the object is not empty
        for (int i = 0; i < m_DivPoints.size(); i++) {
                m_Arcs[i].CutOff(m_DivPoints[i], NewIntersectionPoint);
        }
    }
}





// Prints out the DivPoints and all the Arcs.
std::ostream& operator<<(std::ostream& Out, const ArcsAroundDivPoints& adp){
    using namespace std;
    
    Out << "Reference points: ";
    for (int i = 0; i < adp.m_DivPoints.size(); i++) {
        Out << adp.m_DivPoints[i] << " ";
    }
    Out << endl << "Intervals: " << endl;
    for (int i = 0; i < adp.m_DivPoints.size(); i++) {
        cout << i << ": ";
        if (adp.IsEmpty()) {
            cout << "whole circle";
        } else {
            cout << adp.m_Arcs[i];
        }
        cout << endl;
    }
    return Out;
}



// Checks if the DivPoints in the two objects are the same. 
bool IsSameDivPointType(const ArcsAroundDivPoints& adp1, const ArcsAroundDivPoints adp2)
{
    return adp1.m_DivPoints == adp2.m_DivPoints ? true : false;
}



// Takes the intersection of two ArcsAroundDivPoints by taking the union of intersection points in them
ArcsAroundDivPoints Intersect(const ArcsAroundDivPoints& adp1, const ArcsAroundDivPoints& adp2)
{
    
    if (!IsSameDivPointType(adp1, adp2)) {
        std::cerr << "Can't intersect ArcsAroundDivPoints of different DivPoint types!";
        exit(1);
    }
     
    if (adp1.IsEmpty()) { // if one object is empty, it does not add new intersection point, so we return the other object.
        return adp2;
    }
    if (adp2.IsEmpty()) {
        return adp1;
    }
    const std::vector<CirclePoint>& DivPoints = adp1.m_DivPoints; // giving a non-assymetrical name
    ArcsAroundDivPoints adp = adp1;
    for (int i = 0; i < adp1.m_DivPoints.size(); i++) {
        adp.m_Arcs[i].CutOff(DivPoints[i], adp2.m_Arcs[i].GetLeftEndpoint());
        adp.m_Arcs[i].CutOff(DivPoints[i], adp2.m_Arcs[i].GetRightEndpoint());
    }
    
    return adp;
}
    


bool ArcsAroundDivPoints::ContainsADivPoint(const Arc& arc) const{
    for (int i = 0; i < m_DivPoints.size(); i++) {
        if (arc.ContainsQ(m_DivPoints[i])) {
            return true;
        }
    }
    return false;
}





bool ArcsAroundDivPoints::ContainsArcQ(const Arc& arc) const{
    for (int i = 0; i < m_DivPoints.size(); i++) {
        if (arc.ContainsQ(m_Arcs[i].GetLeftEndpoint()) || arc.ContainsQ(m_Arcs[i].GetRightEndpoint())) {
            return false;
        }
    }
    for (int i = 0; i < m_DivPoints.size(); i++) {
        if (m_Arcs[i].ContainsQ(arc.GetRightEndpoint())) {
            return true;
        }
    }
    return false;
}






bool ArcsAroundDivPoints::ContainsArcThroughADivPointQ(const Arc& arc) const{
    if (ContainsArcQ(arc) && ContainsADivPoint(arc)) {
        return true;
    }
    return false;
}





