/*******************************************************************************
 *  CLASS NAME:	ArcsAroundDivPoints
 *  AUTHOR:		Balazs Strenner
 *				Copyright 2013 UW-Madison. All rights reserved.
 *  DATE:		1/8/13
 *
 *
 *	DESCRIPTION: A collection of arcs around a set of point on the circle of circumference 1.
 
    MOTIVATION: This class is the heart of all calculations. The use of this class is primarily
                finding simple closed curves on RP^2 or on the sphere that are transverse to a given
                measured foliation.
 
                The basic idea is that every simple closed curve has be homotoped so that it is a chain
                of arcs which are alternatingly either leaf segments or transverse arcs. The leaf segments 
                can be homotoped in the transverse direction of the foliation until they hit a singularity,
                so the leaf segments can be assumed to be separatrix segments. The the basic idea of finding
                simple closed curves is to consider separatrix segments of different lengths and try to connect
                their endpoints transversely without intersecting the curve itself. (These closed curves are not
                transverse yet and can't always be made transverse, but it is easy to check which one correspond to 
                transverse curves.)
 
                These transverse arcs that will connect the separatrix segments will be taken near and parallel to
                the boundary circle in the case of RP^2 and near the equator in the case of the sphere.
                For simplicity, assume now that the endpoint of the separatrices are always on the boundary circle or
                on the equator, in the future called only the Circle.
 
                So for a start, we can say that two endpoints of separatrix segments can be connected if at least 
                one of the two arcs between the two endpoints on the Circle don't intersect either of the
                separatrix segments. For checking this we would have to store all the intersection of the
                separatrix segments with the Circle, and check if the two endpoints are in the same subarc. 
                
                There are more problems with this approach. First, it is slow. If we don't order the intersections,
                then every the time needed to search for being in the same arc changes linearly with the number 
                of intersections, which is quite a lot of a separatrix segment of length 1 million for instance.
                So it is better to sort the intersection points. Then adding a new intersection point or checking 
                if two points are in the same arcs takes log(#intersections) time, which is much better, but it
                turns out, that storing all the intersection points is unnecessary.
 
                That brings us to the second problem with the above approach, namely that we would get each simple 
                closed curves many times. Why is that? Think about the simple case when our simple closed curve
                has only two parts, a separatrix segment, containing a singularity, and a transverse arc of the Circle,
                that connects its two endpoints. That transverse arcs can be homotoped by moving its endpoint on
                the same leaves into either direction as long as we don't hit a singularity. So we could possibly 
                push the transverse arc through the Circle multiple times before hitting a singularity. Each stages 
                along the way when the transverse arc is on the Circle gives a different representation of the
                simple closed curve. So we can restrict ourselves to connecting transverse arcs that contain
                a division point (the first intersection of a separatrix to the Circle). This means that the only
                separatrix segments that are used to build simple closed curves are those whose endpoint can be
                connected on the Circle to a division point without intersecting itself.
 
                Because of that, we don't need to store all the intersection points of a separatrix segments with the
                Circle, just the ones that are closest to the division points. More precisely: the intersection
                points divide the circle into arcs, and for each division point we only have to remember the arc that 
                contains that division point. (Actually two arcs if an intersection point coinsides with the division
                point.) 
 
                In terms of both storage and running time this is much better than previously. Making a separatrix
                segment one longer, and updating data takes only constant times, because only constant number of
                arcs are stored. Moreover, most of the separatrix segments we don't even have to store at all. We
                only have to remember the good ones, and assemble simple closed curves from them in certain conbinations
                later on. The good news is that the number of good separatrix segments grows only logarithmically 
                with their length (more precisely the number of intersections with the Circle). For the Arnoux-Yoccoz
                foliation on RP^2, there are only about 150 good separatrix segments with length less than 1 million.
 
    CLASS DETIALS: 
                
                1. m_DivPoints. This is the array of division points on the circle. In the application of this class to
                    the RP^2 case these points are sorted, but they are not assumed to be.
                    Memory for m_DivPoints is not allocated by the object, and m_DivPoints is not stored by the object.
                    The input is simply the pointer to the array of division points in the foliation. Since a lot of 
                    ArcsAroundDivPoints objects are handles for the same foliation and therefore the same set of 
                    division points, this saves a lot of unnecessary copying. 
                    
                    But because of this, the ENTRIES OF M_DIVPOINTS MUST NOT BE CHANGED BY THIS OBJECT!
 
                2. m_Arcs. We imagine that a lot of intersection points are scattered on the Circle that divide the Circle
                    into smaller arcs. m_Arcs[i][0] contains the arc that contains m_DivPoints[i], or, if m_DivPoints[i]
                    itself is an intersection point, then m_Arcs[i][0] contains the one on its left side, and
                    m_Arcs[i][1] contains the one on the right side. (These two arcs coincide in the case when 
                    the only intersection point is m_DivPoints[i] itsef.)
 
                3. Without any intersection points, we condider the object "empty". In this case m_IsEmpty is true, and
                    no Arcs are stored. Once the first intersection point is added, m_IsEmpty becomes false and Arcs 
                    are initialized.
 
 
    POSSIBLE IMPROVEMENTS:
                
                1. As we discussed, in reality, we would not take exactly separatrix segments as building blocks, but 
                    leaf segments infinitesimally close to them. Therefore the intersection points (so called separating points)
                    are also a little off which means that in reality, the division points are always in the inside of the
                    Arcs around them. Moreover, when the leaf infinitesimally close to a separatrix exits the Circle, 
                    we should not really keep the arcs on both sides. On the side of the division point that the leaf
                    is going the arc should be cut off infinitesimally close, and only the other part should be kept.
                    For the simplest case of simple closed curves (having only one leaf segment), this simple model will 
                    do it though.
 
 *
 *
 ******************************************************************************/


#ifndef ArnouxYoccoz_ArcsAroundDivPoints_h
#define ArnouxYoccoz_ArcsAroundDivPoints_h

#include <iostream>
#include <vector>
#include "Arc.h"




class ArcsAroundDivPoints{
public:
    
    ArcsAroundDivPoints(const std::vector<CirclePoint>& DivPoints) : m_DivPoints(DivPoints) {}   // contructor empty object

  //  void Init(CirclePoint DivPoints[], int NumDivPoints);        // Initializing values for empty object
    void InsertPoint(const CirclePoint& NewIntersectionPoint);  // Inserts a new intersection point
    bool ContainsQ(const CirclePoint& c) const;                 // Decides if a point is contained in any of the Arcs around the division points
    
    // Takes the intersection of two ArcsAroundDivPoints by taking the union of intersection points in them
    friend ArcsAroundDivPoints Intersect(const ArcsAroundDivPoints& adp1, const ArcsAroundDivPoints& adp2);
    bool ContainsArcThroughADivPointQ(const Arc&) const;
    friend std::ostream& operator<<(std::ostream& Out, const ArcsAroundDivPoints& adp); // Prints out the DivPoints and all the Arcs.

private:

    const std::vector<CirclePoint>& m_DivPoints;   // the array of division points, ENRTIES NOT TO BE CHANGED BY THIS OBJECT!
    std::vector<Arc> m_Arcs;               // the array of Arcs around division points
    
    inline bool IsEmpty() const { return m_Arcs.size() == 0 ? true : false; }
    bool ContainsADivPoint(const Arc&) const;
    bool ContainsArcQ(const Arc&) const;
    
    // Checks if the DivPoints in the two objects are the same
    friend bool IsSameDivPointType(const ArcsAroundDivPoints& adp1, const ArcsAroundDivPoints adp2);
    
  //  ArcsAroundDivPoints& operator=(const ArcsAroundDivPoints& adp);                     // assignment operator
  //  ArcsAroundDivPoints() {}      // default constructor

};






#endif
