/*******************************************************************************
 *  CLASS NAME:	CirclePoint
 *  AUTHOR:		Balazs Strenner
 *				Copyright 2013 UW-Madison. All rights reserved.
 *  DATE:		1/8/13
 *
 *  DESCRIPTION: A point of the circle of circumference 1.
 
    MORE DETAILS: Point are encoded with one real number m_Value, such that 0<=m_Value<1
        
    POSSIBLE IMPROVEMENTS:
                1. We could introduce generalized points which are infinitesimally close
                    to some point on the circle. For the simplest case of builing simple closed curves,
                    when there is only one leaf segment, and one transverse arc, we can live without
                    it, but in general, separatrix segments (as discussed in the documentation of
                    other header file) are considered to run infinitesimally closely to separatrix segments,
                    so handling points like 0.5+ or 0.5- would be useful.
 
 ******************************************************************************/

#ifndef ArnouxYoccoz_CirclePoint_h
#define ArnouxYoccoz_CirclePoint_h

#include <iostream>
#include <cmath>

typedef long double floating_point_type;

enum LeftOrRight{
    LEFT = 0,
    RIGHT = 1,
    CENTER = 2
};

inline LeftOrRight Opposite(LeftOrRight x){
    switch (x) {
        case LEFT:
            return RIGHT;
            break;
            
        case RIGHT:
            return LEFT;
        default:
            return CENTER;
            break;
    }
}

// The fractional part of any real number. Works for negatives, too: FracPart(-0.3)=0.7.
inline floating_point_type FracPart(floating_point_type t){ return t - floor(t); }

class CirclePoint {        
public:
    CirclePoint() { }
    CirclePoint(floating_point_type t) : m_Value(FracPart(t)), m_Side(CENTER) {}        // usual constructor for an ordinary point
    CirclePoint(floating_point_type t, LeftOrRight Side) : m_Value(FracPart(t)), m_Side(Side) {}      // constructor for a generalized point
    
    
    inline const floating_point_type& GetValue() const { return m_Value; }     // Returns m_Value in actual number type

    
    inline CirclePoint Antipodal() const{ return CirclePoint(m_Value + 0.5, m_Side); }       // Returns the antipodal of the point
    inline CirclePoint Centered() const{ return CirclePoint(m_Value); }
    inline CirclePoint ShiftedTo(LeftOrRight Side) const{ return CirclePoint(m_Value, Side); }
    
    inline void Shift(LeftOrRight Side) { m_Side = Side; }
    inline bool IsShiftedTo(LeftOrRight Side) const{ return m_Side == Side ? true : false;}


    
    // Returns the distance between c1 and c2. It looks for c2 on the RIGHT of c1, so the return value may be bigger than 0.5.
    friend floating_point_type DistanceBetween(const CirclePoint& c1, const CirclePoint& c2);
    
    // Decides if cThird is between cLeft and cRight (to the right from cLeft and the left from cRight)
    // I.e. (0.5, 0.2, 0.1) returns true, but (0.5, 0.2, 0.4) returns false
    friend bool IsBetween(const CirclePoint& cLeft, const CirclePoint& cRight, const CirclePoint& cThird);
    


    // Shifts the point c into the positive direction by Distance
    inline friend CirclePoint operator+(const CirclePoint& c, floating_point_type Distance){
        return CirclePoint(c.m_Value + Distance, c.m_Side);
    }
    
    // Shifts the point c into the negative direction by Distance
    inline friend CirclePoint operator-(const CirclePoint& c, floating_point_type Distance){ return CirclePoint(c.m_Value - Distance, c.m_Side); }
    inline friend CirclePoint operator-(const CirclePoint& c){ return CirclePoint(-c.m_Value, Opposite(c.m_Side)); }
    inline friend bool operator==(const CirclePoint& c1, const CirclePoint& c2){
        return c1.m_Value == c2.m_Value && c1.m_Side == c2.m_Side ? true : false;
    }
    inline friend bool operator!=(const CirclePoint& c1, const CirclePoint& c2){ return c1 == c2 ? false : true; }


    friend std::ostream& operator<<(std::ostream& Out, const CirclePoint& c);

    
    
private:
    floating_point_type m_Value; // the point is described by this number, at least 0 and less than 1
    LeftOrRight m_Side;
    
    
    inline friend bool operator<(const CirclePoint& c1, const CirclePoint& c2);
    inline friend bool operator<=(const CirclePoint& c1, const CirclePoint& c2){ return c1 < c2 || c1 == c2 ? true : false;}
};


        
        
        
        


    








#endif
