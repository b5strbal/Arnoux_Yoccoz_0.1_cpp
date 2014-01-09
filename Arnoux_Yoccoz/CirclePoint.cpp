//
//  CirclePoint.cpp
//  Arnoux_Yoccoz
//
//  Created by Balazs Strenner on 1/26/13.
//  Copyright (c) 2013 Balazs Strenner. All rights reserved.
//

#include "CirclePoint.h"






bool operator<(const CirclePoint& c1, const CirclePoint& c2){
    if(c1.m_Value < c2.m_Value)
        return true;
    if(c1.m_Value > c2.m_Value)
        return false;
    if((c1.m_Side == LEFT && (c2.m_Side == RIGHT || c2.m_Side == CENTER)) || (c1.m_Side == CENTER && c2.m_Side == RIGHT))
        return true;
    return false;
}


std::ostream& operator<<(std::ostream& Out, const CirclePoint& c){
    Out << c.m_Value;
    switch(c.m_Side){
        case RIGHT: Out << "(+)";
            break;
        case LEFT: Out << "(-)";
            break;
        case CENTER:
            break;
    }
    return Out;
}

bool IsBetween(const CirclePoint& cLeft, const CirclePoint& cRight, const CirclePoint& cThird)
{
    
    if (cLeft <= cThird && cThird <= cRight) {
        return true;
    }
    if (cRight < cLeft && (cLeft <= cThird || cThird <= cRight)) {
        return true;
    }
    return false;
    
}


floating_point_type DistanceBetween(const CirclePoint& c1, const CirclePoint& c2)
{
    return c1.m_Value <= c2.m_Value ? c2.m_Value - c1.m_Value : c2.m_Value - c1.m_Value + 1;
}