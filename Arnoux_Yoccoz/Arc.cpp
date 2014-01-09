/*******************************************************************************
 *  CLASS NAME:	Arc
 *  AUTHOR:		Balazs Strenner
 *				Copyright 2013 UW-Madison. All rights reserved.
 *  DATE:		1/8/13
 *
 ******************************************************************************/

#include "Arc.h"


// sets values for the endpoints as inputs
void Arc::Set(const CirclePoint& LeftEndpoint, const CirclePoint& RightEndpoint){        
        // We assume that the length of the arc is less than 1. This is necessary because there are two arcs with LeftEndpoint = RightEndpoint, 
        // one with length 0, and one with length 1. So if the inputs are equal, we construct the arc of length 0.
        // Length 1 arcs therefore have to be constructed by giving length as one of the inputs.

        m_LeftEndpoint = LeftEndpoint;
        m_RightEndpoint = RightEndpoint;
        m_Length = DistanceBetween(LeftEndpoint, RightEndpoint);
} 



// decides whether t is in the arc
bool Arc::ContainsQ(const CirclePoint& t) const{
    return m_Length == 1 ? true : IsBetween(m_LeftEndpoint, m_RightEndpoint, t);
}



// Cuts off a portion of the arc
void Arc::CutOff(const CirclePoint& RefPoint, const CirclePoint& CuttingPoint){ 
    
    // Divides the arc into two at CuttingPoint, throws away the one that does not contain RefPoint,
    // and updates the arc to the one that contains RefPoint.
    
    // IMPORTANT: RefPoint and CuttingPoint must be contained in the arc! Also, if the original arc has length 1, 
    // RefPoint can't concide with the left and right endpoints. In this case
    // when we cut the arc, RefPoint is in both arcs, so the cut off arc is not well-defined!
    
    if (!ContainsQ(RefPoint)) {
        std::cerr << "Error in Arc::CutOff. Bad arguments.";
        exit(1);
    }
    
    if(ContainsQ(CuttingPoint)){ 
        if (IsBetween(m_LeftEndpoint, RefPoint, CuttingPoint)) { // If CuttingPoint is on the left side of RefPoint
            Set(CuttingPoint, m_RightEndpoint);
        }
        else {   // If CuttingPoint is on the right side of RefPoint
            Set(m_LeftEndpoint, CuttingPoint);
        }
    }
}






// Prints out the arc in interval form
std::ostream& operator<< (std::ostream &Out, const Arc &MyArc){
    Out << "[" << MyArc.m_LeftEndpoint << "," << MyArc.m_RightEndpoint << "]";
    return Out;
}




