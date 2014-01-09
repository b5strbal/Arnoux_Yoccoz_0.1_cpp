//
//  io.h
//  Arnoux_Yoccoz
//
//  Created by Balazs Strenner on 2/19/13.
//  Copyright (c) 2013 Balazs Strenner. All rights reserved.
//

#ifndef Arnoux_Yoccoz_io_h
#define Arnoux_Yoccoz_io_h

#include <iostream>
#include "FoliationRP2.h"

using namespace std;






char GetFirstCharOfLine(){
    string Line;
    getline(cin, Line);
    if (Line.empty()) {
        return 0;
    }
    else
        return Line[0];
}





void TrimEndingWhiteSpaces(string& str){
    string whitespaces (" \t\f\v\n\r");
    
    unsigned long found = str.find_last_not_of(whitespaces);
    if (found != string::npos)
        str.erase(found + 1);
    else
        str.clear();
}





void GetLine(string& Line){
    getline(cin, Line);
    TrimEndingWhiteSpaces(Line);
    if (Line.empty()) {
        throw("Type in something.");
    }
}





void GetWeighedTreeInput(std::vector<floating_point_type>& WeighedTreeInput){
    WeighedTreeInput.clear();
    string Line;
    while (WeighedTreeInput.empty()) {
        try {
            GetLine(Line);
            istringstream ss(Line);
            while (!ss.eof()) {
                floating_point_type d;
                ss >> d;
                if (ss.fail()) {
                    throw "Only numbers, please.";
                }
                WeighedTreeInput.push_back(d);
            }
        }
        catch (const char* error) {
            cout << "*** Error: " << error << "\n\n";
            WeighedTreeInput.clear();
        }
    }
}





void GetLengths(std::vector<floating_point_type>& Lengths){
    string Line;
    Lengths.clear();
    while (Lengths.size() == 0) {
        try {
            cout << "Lengths: ";
            GetLine(Line);
            istringstream ss(Line);
            while (!ss.eof()) {
                floating_point_type d;
                ss >> d;
                if (ss.fail()) {
                    throw "Only numbers, please.";
                }
                if (d <=0) {
                    throw "Lengths can only be positive.";
                }
                Lengths.push_back(d);
            }
            if (Lengths.size() % 2 == 1) {
                throw "An even number of lengths must be specified.";
            }
            if (Lengths.size() < 6) {
                throw "At least 6 lengths are needed for a non-trivial foliation.";
            }
        }
        catch (const char* error) {
            cout << "*** Error: " << error << "\n\n";
            Lengths.clear();
        }
    }
}


void GetPair(std::vector<int>& Pair, int Size){
    Pair.clear();
    string Line;
    while (Pair.size() == 0) {
        try {
            cout << "Pairing: ";
            GetLine(Line);
            istringstream ss(Line);
            while (!ss.eof()) {
                int n;
                ss >> n;
                if (ss.fail()) {
                    throw "Only integers, please.";
                }
                Pair.push_back(n);
                if (n < 0 || n >= Size) {
                    throw "Indices out of range.";
                }
            }
            if (Size != Pair.size()) {
                throw "The lengths list and the pairing list should be equally long.";
            }
            ValidatePair(Pair);
        }
        catch (const char* error) {
            cout << "*** Error: " << error << "\n\n";
            Pair.clear();
        }
    }
}




int GetGenus(){
    int Genus = 0;
    string Line;
    while(Genus == 0){
        try{
            cout << "Genus: ";
            GetLine(Line);
            istringstream ss(Line);
            ss >> Genus;
            if (ss.fail()) {
                throw "The genus must be an integer.";
            }
            if (Genus < 3) {
                throw "The genus must be at least three.";
            }
        }
        catch (const char* Error){
            cout << "*** Error: " << Error << "\n\n";
            Genus = 0;
        }
    }
    return Genus;
}



int GetDepth(){
    int Depth = 0;
    string Line;
    while(Depth == 0){
        try{
            cout << "Depth: ";
            GetLine(Line);
            istringstream ss(Line);
            ss >> Depth;
            if (ss.fail()) {
                throw "The depth must be an integer.";
            }
            if (Depth < 1) {
                throw "The depth must be positive.";
            }
        }
        catch (const char* Error){
            cout << "*** Error: " << Error << "\n\n";
            Depth = 0;
        }
    }
    return Depth;
}
















void Greetings(){
    cout << "888    888 d8b      8888888b.  d8b          888                           888 888\n";
    cout << "888    888 Y8P      888   Y88b Y8P          888                           888 888\n";
    cout << "888    888          888    888              888                           888 888\n";
    cout << "8888888888 888      888   d88P 888  .d8888b 88888b.   8888b.  888d888 .d88888 888\n";
    cout << "888    888 888      8888888P   888 d88P     888  88b      88b 888P   d88  888 888\n";
    cout << "888    888 888      888 T88b   888 888      888  888 .d888888 888    888  888 Y8P\n";
    cout << "888    888 888      888  T88b  888 Y88b.    888  888 888  888 888    Y88b 888    \n";
    cout << "888    888 888      888   T88b 888   Y8888P 888  888  Y888888 888      Y88888 888\n";
    
    cout << "\nWelcome to Balazs's program for finding Arnoux-Yoccoz-type pseuso-anosovs!\n\n";
}



void WaitForEnter(){
    cout << "(Press ENTER)\n";
    string Line;
    getline(cin, Line);
}


void PrintTitle(const char* Title){
    cout << "\n\n---------------------------------------------------\n";
    cout << Title << "\n\n";
}




void PrintInfo(){
    PrintTitle("THE GENERAL IDEA");
    
    cout << "The main objects of study in the program are measured singular foliations on RP^2. They are represented as measured foliation on the disk, with antipodal points identified. The two major ways to encode a foliation on the disk are by weighed trees and by a list of lengths and a pairing between them (see below). The measured foliations we consider are transverse to the boundary, and the length (measure) of the boundary circle is always scaled to 1.\n\n";
    cout << "Once the foliation is specified, different operations can be performed, such as searching for pseudo-anosovs fixing a foliation close it, or listing good one-sided curves.\n";


    
    PrintTitle("WEIGHED TREES");
    cout << "A weighed tree is a tree embedded in the plane with positive weights on the edges. Imagine that the edges of the tree are made just a little thicker, and air is pumped into the inside of the tree. Since it is a tree, it expands to a disk. Assume that the boundary doesn't stretch, so each side of each edge takes up as long part of the boundary circle as it's weighed.\n\n";
    cout << "The circumference of the circle will be twice the sum of the weights of the edges. There is a unique measured foliation up to Whitehead equivalence that pairs up the segments of the boundary circle that come from the same edge.\n\n";
    cout << "Each vertex of the tree corresponds to a singularity of the foliation, the degree of the vertex and number of prongs being the same. In particular, leafs correspond to 1-pronged singularities, and we may assume that there are no degree 2-vertices, and that there is at least one vertex of degree at least 3.\n";

    
    PrintTitle("LENGTHS AND PAIRING");
    cout << "The intersection of the separatrices with the boundary circle divide the boundary circle into even number of arcs. The foliation induces a pairing on these arcs. Therefore any measured foliation can be described by the list of lengths of the arcs and the pairing between them.\n\n";
    cout << "Conversely given a list of even number of positive numbers and a valid pairing of these numbers (pairs should be equal and no two pairs should \"cross\" in the sense that the corresponding arcs can be connected inside the disk without intersecting) defines a measured foliation in the disk that is unique up to Whitehead equivalence.\n\n";
    cout << "In a way, this way of describing a measured foliation is less elegant than by a weighed tree. A weighed tree more clearly preserves the combinatorial properties of the foliation, and eliminates the need for thinking about valid pairings.\n";

    
    PrintTitle("ARNOUX-YOCCOZ FOLIATIONS");
    cout << "Arnoux and Yoccoz constructed a family of pseudo-anosovs on each genus g oriented surface with g>=3.\n\n";
    cout << "These pseudo-anosovs are lifts of pseudo-anosovs on RP^2 whose stable/unstable foliations have one g-pronged singularity and g 1-pronged singularity. In the genus g case the stretch factor ALPHA is the Perron-Frobenius root of the polynomial x^g - x^(g-1) - ... - x - 1. The length parameters of the foliation are (1/ALPHA + 1/ALPHA^2)/2, (1/ALPHA + 1/ALPHA^2)/2, (1/ALPHA^2 + 1/ALPHA^3)/2,, (1/ALPHA^2 + 1/ALPHA^3)/2, ... , (1/ALPHA^g + 1/ALPHA)/2, (1/ALPHA^g + 1/ALPHA)/2.\n\n";
    
    WaitForEnter();
}














FoliationRP2* GetFoliation(){
    while(true) {
        PrintTitle("ENTER A MEASURED FOLIATION");
        cout << "Choose a way:" << endl;
        cout << "- Weighed tree (w)" << endl;
        cout << "- Lengths and pairing (l)" << endl;
        cout << "- The Arnoux-Yoccoz foliations (a)" << endl;
        cout << "- Random (r)" << endl;
        cout << "- More info (i)" << endl;
        cout << "- Quit (q)" << endl;
        cout << "(Press key and ENTER)" << endl;
        
        char c = 0;
        while (c != 'w' && c != 'l' && c != 'a' && c != 'r' && c != 'i' && c != 'q') {
            c = GetFirstCharOfLine();
        }
        FoliationRP2* f = NULL;
        switch (c) {
            case 'w':
            {
                PrintTitle("ENTERING A WEIGHED TREE");
                
                cout << "Weighed trees can be specified by a sequence of non-negative numbers. Pick a root for the tree, and draw it \"hanging down\" from the root. First, enter the weights of the edges hanging down from root from left to right, then enter 0. Proceed to the next generation, enter the weights hanging from the first vertex in the generation and enter 0, and so on until all weights are entered." << endl << endl;
                cout << "E.g. the sequence 0.1 0.2 0.3 0 0.1 0.2 0.3 0 0.5 0.6 0 0.9 0.5 encodes a weighed tree with 11 vertices, 7 of them are leaves, 3 vertices are of degree 3, one vertex is of degree 4." << endl << endl;
                cout << "Now enter the coding of a weighed tree, in the above form." << endl;
                
                std::vector<floating_point_type> WeighedTreeInput;
                
                while (f == NULL) {
                    try {
                        GetWeighedTreeInput(WeighedTreeInput);
                        f = new FoliationRP2(WeighedTree(WeighedTreeInput));
                    }
                    catch (const char* Error){
                        cout << "*** Error: " << Error << "\n\n";
                    }
                }
                return f;
            }
                
            case 'l':
            {
                PrintTitle("ENTERING LENGTHS AND PAIRING");
                
                cout << "Here is an example of specifying a valid sequence of lengths and pairing:\n";
                cout << "Lengths: 0.1 0.1 0.2 0.2 0.3 0.3\n";
                cout << "Pairing: 1 0 3 2 5 4 (0th arc paired up with the 1st, the 1st with the 0th, etc.)\n\n";
                cout << "You may provide lengths such that the values in pairs are not equal, in which case they will be averaged out. E.g.\n";
                cout << "Lengths: 0.1232 0.232 0.34523 0.12523 0.263432 0.1235\n";
                cout << "Pairing: 3 2 1 0 5 4\n\n";
                
                std::vector<floating_point_type> Lengths;
                std::vector<int> Pair;
                
                while (f == NULL) {
                    try {
                        GetLengths(Lengths);
                        GetPair(Pair, static_cast<int>(Lengths.size()));
                        f = new FoliationRP2(Lengths, Pair);
                    }
                    catch (const char* Error){
                        cout << "*** Error: " << Error << "\n\n";
                    }
                }
                return f;
            }
                
            case 'a':
            {
                PrintTitle("ENTERING AN ARNOUX-YOCCOZ FOLIATION");
                cout << "The Arnoux-Yoccoz foliation of which genus do you mean?\n\n";
                
                int genus = GetGenus();
                AlmostPFMatrix AYMatrix = ArnouxYoccozMatrix(genus);
                floating_point_type alpha = 1/AYMatrix.GetPFEigenvalue();
                std::vector<floating_point_type> Lengths(genus);
                for (int i = 1; i < genus; i++) {
                    Lengths[i - 1] = (pow(alpha, i) + pow(alpha, i + 1))/4;
                }
                Lengths[genus - 1] = (alpha + pow(alpha, genus))/4;
                
                f = new FoliationRP2(WeighedTree(Lengths));
                return f;
            }
                
            case 'r':
            {
                PrintTitle("RANDOM FOLIATION");
                cout << "The genus of a foliation on RP^2 is the genus of the orintable surface obtained by the 4-fold branched cover over the singularities that orients both the surface and the foliation. The random foliation will be generated in the genus provided.\n\n";

                f = new FoliationRP2(WeighedTree(GetGenus()));
                return f;
            }
                
            case 'i':
                PrintInfo();
                break;
            case 'q':
                throw -1;
                break;
        }
    }
}



void PrintOperationInfo(){
    PrintTitle("GOOD SHIFTED SEPARATRIX SEGMENTS");
    cout << "A SEPARATRIX SEGMENT is a segment of a separatrix, the starting point being the singularity the separatrix is emanating from. (In general we assume that there are no saddle connections.) The depth of a separatrix segment is roughly how many times it intersects the boundary circle. More precisely, a depth 1 segment does not intersect the circle, its endpoint is on the inner side of the boundary circle. A depth 2 segment is just a little bit longer, the endpoint being on the inner side of the antipodal of the first intersection. We get a depth 3 segment by lengthening a depth 2 segment by following the separatrix until we hit the boundary from inside again, and so on.\n\n";
    cout << "Assuming for a moment that the antipodal points are not identified, the intersections of the separatrices with the boundary circle are called DIVISION POINTS. The number of division points is the same as the sum of the pronges of singularities, i.e. 2 * genus. Therefore the endpoints of separatrix segments of depth 1 are exactly the division points.\n\n";
    cout << "A SHIFTED SEPARATRIX SEGMENT is obtained from a separatrix segment by shifting it a little to either side.\n\n";
    cout << "A GOOD SHIFTED SEPARATRIX SEGMENT is a shifted separatrix segment such that its endpoint (which is on the boundary circle) can be connected to a division point on the boundary circle without self-intersection. It turns out that it is sufficient to consider only these special shifted separatrix segments as building blocks of simple closed curves.\n\n";
    cout << "This observation speeds up the computation enormously since the number of good separatrix segments with bounded depth grows only logarithmically with depth. For example, for the Arnoux-Yoccoz foliation in genus 3, there only about 150 good segments with depth less than a million for each separatrix.\n";
    
    
    
    PrintTitle("TRANSVERSE ONE-SIDED SIMPLE CLOSED CURVES (GOOD CURVES)");
    cout << "GOOD CURVES are one-sided (orienation-reversing) simple closed curves in RP^2 that are transverse to a given measured foliation. The reason such curves are interesting is that when we cut out a good curve from RP^2, we get a measured foliation on the disk which is transverse to the boundary.\n\n";
    cout << "Since we already represent measured foliations on RP^2 by measured foliations on the disk that are transverse to the boundary (antipodal point identify), there is an obvious good curve in all cases: the \"half\" of the boundary circle.\n\n";
    cout << "The appearance of other good curves is quite different for different foliations though. In a nutshell, our algorithm tries to build them from two good shifted separatrix segments. If two such segments correspond to the same singularity and they are contained in the same leaf, then they together yield a longer leaf segment which may be closed in to a closed curve by adding an arc of the boundary circle. Many times such a closed curve is not transverse (and cannot be made transverse by isotopy), not one-sided, and not simple. But the few that reamains will be what we are looking for. Note that this algorithm probably does not generate all transverse one-sided simple closed curves.\n\n";
    
    
    PrintTitle("SEARCH FOR PSEUDO-ANOSOVS");
    cout << "The algorithm is the generalization of the idea of the original construction of Arnoux and Yoccoz. Essentially what they did is they found a good curve in the combinatorically simplest situation of one 3-pronged singularity and 3 1-pronged singularity, and noticed that if the length parameters are appropriate, the measured foliation of the disk they get by cutting out the curve are similar, i.e. one gets the other by multiplying the measure by a constant. This immediately induces a pseudo-anosov on RP^2, which induces a pseudo-anosov on the genus 3 orientable surface when lifted up.\n\n";
    cout << "So the algorithm first searches for good curves. Then computes the parameters of measured foliations obtained by cutting along them, and compares it to the original parameters.\n\n";
    cout << "A randomly chosen measured foliation is not going to be a stable/unstable foliation of a pseudo-anosov though. So when a good curve is found, a transition matrix with non-negative integer entries is computed which relates the original length vector to the new one, and if the matrix is Perron-Frobenius, the Perron-Frobenius eigenvector provides length parameters which are better candidates for a measured foliation belonging to a pseudo-anosov. Since the combinatorics of transverse one-sided simple closed curves changes when the measured foliation changes, another search for this new measured foliation has to be run.\n\n";
    
    
    
    WaitForEnter();
}







void PerformOperation(FoliationRP2* Foliation){
    bool Quit = false;
    while(!Quit) {
        PrintTitle("PERFORM AN OPERATION");
        cout << "Choose one:" << endl;
        cout << "- List good shifted separatrix segments (s)" << endl;
        cout << "- List good curves (c)" << endl;
        cout << "- Search for pseudo-anosovs (p)" << endl;
        cout << "- More info (i)" << endl;
        cout << "- Choose another foliation (f)" << endl;
        cout << "- Quit (q)" << endl;
        cout << "(Press key and ENTER)" << endl;
        
        char c = 0;
        while (c != 's' && c != 'c' && c != 'p' && c != 'i' && c != 'f' && c != 'q') {
            c = GetFirstCharOfLine();
        }
        switch (c) {
            case 's':
            {
                PrintTitle("LISTING GOOD SHIFTED SEPARATRIX SEGMENTS");
                cout << "You can choose between concise and verbose listing.\n";
                cout << "Verbose lists a lot of information, and most of the time it is unnecessary.\n";
                cout << "Concise lists only the depths of good segments, and only odd ones (this is not a big loss of information since it is almost true that 2k+1 is a good depth for a certain separatrix and side if and only if 2k is good as well).\n";
                cout << "Choose between two types of listing (unless you really need all information, verbose is not recommended):\n";
                cout << "- Concise (c)\n";
                cout << "- Verbose (v)\n";
                cout << "(Press key and ENTER)\n";

                char c = 0;
                while (c != 'c' && c != 'v') {
                    c = GetFirstCharOfLine();
                }
                cout << "Enter the depth of search for good shifted separatrix segments. (A depth of 1 million typically takes a few seconds, and the running time is linear in depth.)\n\n";
                
                switch (c) {
                    case 'c':
                        Foliation->PrintGoodShiftedSeparatrixSegmentsConcise(GetDepth());
                        break;
                        
                    case 'v':
                        Foliation->PrintGoodShiftedSeparatrixSegmentsVerbose(GetDepth());
                        break;
                }
                
                WaitForEnter();
                break;
            }
            case 'c':
                PrintTitle("LISTING GOOD CURVES");
                cout << "Enter the depth of search for good curves.\n\n";
                Foliation->PrintGoodCurves(GetDepth());
                WaitForEnter();
                break;
            case 'p':
                PrintTitle("SEARCH FOR PSEUDO-ANOSOVS");
                cout << "Enter the depth of search for pseudo-anosovs.\n\n";
                Foliation->PrintPseudoAnosovs(GetDepth());
                WaitForEnter();
                break;
            case 'i':
                PrintOperationInfo();
                break;
            case 'f':
                Quit = true;
                break;
            case 'q':
                throw -1; // exiting
                break;
        }
    }
}




#endif
