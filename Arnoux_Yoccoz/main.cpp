//
//  main.cpp
//  ArnouxYoccoz
//
//  Created by Balazs Strenner on 1/8/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <time.h>
#include "io.h"

using namespace std;







int main (int argc, const char * argv[])
{
    /*
   // double alpha = 0.5436890126920764; // The stretch factor of the Arnoux-Yoccoz pseudo-anosov.
    long double alpha = 0.54368901269207636157085597180174799;
    long double list[] = {(alpha + pow(alpha,2))/2, (pow(alpha,2) + pow(alpha,3))/
        2, (alpha + pow(alpha,3))/2};

    WeighedTree wt(list, 3);
    FoliationRP2 fAY(wt);
  //  std::cout << std::setprecision(30);

    std::cout << "aplha = " << 1/alpha << std::endl;
    std::cout << "aplha^2 = " << 1/(alpha * alpha) << std::endl;
    std::cout << "aplha^3 = " << 1/(alpha * alpha * alpha) << std::endl;

    std::cout << "The Arnoux-Yoccoz pseudo-anosov: " << fAY << std::endl;
    

    
    clock_t t1, t2;
    t1 = clock();
    
    
    // fAY.GenerateGoodShiftedSeparatrixSegments(10);
    // fAY.GenerateSimpleClosedCurves();
    fAY.GenerateNewParametrizations(100000);
    t2 = clock();
    float seconds = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
    std::cout << "Time1:" << seconds << std::endl ;
*/
    Greetings();
    FoliationRP2* Foliation;
    bool Exit = false;
    while(!Exit){
        try {
            Foliation = GetFoliation();
            cout << *Foliation;
            WaitForEnter();
            PerformOperation(Foliation);
        } catch (int i) {
            if (i < 0) {
                Exit = true;
            }
        }
    }
    
    
    delete Foliation;
    return 0;
}

