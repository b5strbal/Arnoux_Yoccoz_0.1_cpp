/*******************************************************************************
 *  CLASS NAME:	Arc
 *  AUTHOR:		Balazs Strenner
 *				Copyright 2013 UW-Madison. All rights reserved.
 *  DATE:		1/8/13
 *
 *  DESCRIPTION: Objects of this class type represent (closed) arcs on the circle of circumference 1.
 
    MORE DETAILS: 
        - The arc can be of length 0, in this case as a set it is a point.
        - The arc can be of length 1, in this case as a set it is the whole circle, but these arcs are 
        distinguinged by their endpoints. (For instance, the arc starting at 0.2 of length 1
        and the arc starting at 0.3 of length 1 are different.
        - Or the length can be between 0 and 1.
        - "Left" always means the clockwise direction on the circle, "Right" means the counterclockwise direction
        So left endpoint means the endpoint in the clockwise direction, and right endpoint means 
        the other endpoint.
 	
    POSSIBLE IMPROVEMENTS:
        1. The Arc objects are primarily used by ArcsAroundDivPoints, so the DivPoint inside the arc sort of 
            belongs to the arc. That inside point could be added to this class so that it doesn't have to 
            as for that input (RefPoint) in Arc::CutOff, for example.
 
        2. Arcs of length 0 are not really necessary. However, arcs with endpoint that are infinitesimal CirclePoints
            would be interseting.
 
 
 ******************************************************************************/

#ifndef ArnouxYoccoz_Arc_h
#define ArnouxYoccoz_Arc_h

#include <iostream>
#include "CirclePoint.h"


class Arc {        
public:
// CONSTRUCTORS
    
    // constructor for the endpoints as inputs            
    Arc(const CirclePoint& LeftEndpoint, const CirclePoint& RightEndpoint) :
        m_LeftEndpoint(LeftEndpoint),
        m_RightEndpoint(RightEndpoint),
        m_Length(DistanceBetween(LeftEndpoint, RightEndpoint))
    {}
    
    Arc(const CirclePoint& EndPoint) :
        m_LeftEndpoint(EndPoint),
        m_RightEndpoint(EndPoint),
        m_Length(1)
    {}
    

    
// GETTING INFO
    inline const CirclePoint& GetLeftEndpoint() const{ return m_LeftEndpoint; }
    inline const CirclePoint& GetRightEndpoint() const { return m_RightEndpoint; }
    inline const floating_point_type& GetLength() const{ return m_Length; };
    bool ContainsQ(const CirclePoint& t) const;    // decides whether t is in the arc

// MODIFICATIONS
    void Set(const CirclePoint& LeftEndpoint, const CirclePoint& RightEndpoint); // sets values for the endpoints as inputs   
    void CutOff(const CirclePoint& RefPoint, const CirclePoint& CuttingPoint); // cuts off a portion of the arc

// OPERATORS
    friend std::ostream& operator<< (std::ostream &Out, const Arc &MyArc); // prints out the arc in interval form
    
    
private:
    CirclePoint m_LeftEndpoint;   // left endpoint of the arc
    CirclePoint m_RightEndpoint;     // right endpoint of the arc
    floating_point_type m_Length;  // the length of the arc, 0<=m_Length<=1
    
    Arc() {} // default constructor
};





#endif
