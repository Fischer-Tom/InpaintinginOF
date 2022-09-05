//
// Created by tom on 9/2/22.
//

#ifndef DIFFUSIONINOF_FLOW_H
#define DIFFUSIONINOF_FLOW_H

#include <string>
#include "Image.h"
#include "Derivative.h"

class Flow {

public:
    // Constructors
    Flow();
    Flow(int width, int height);
    Flow(Image<float>& mask, Flow& groundTruth);
    // copy constructor
    Flow(const Flow& aCopyFrom);
    // Destructor
    ~Flow();

    // I0 Operations
    void readFromFlo(const std::string &filename);
    void writeToFlo(const std::string &filename);

    // Utilities
    double EPE(Flow& reference);
    void setDerivative(float h=1);

    // Operators
    float& operator()(int d, int x, int y);

private:
    int width, height;
    float *data{};
    Derivative fx, fy, fxx, fyy, fxy;
};


#endif //DIFFUSIONINOF_FLOW_H
