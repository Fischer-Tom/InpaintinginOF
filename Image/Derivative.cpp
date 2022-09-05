//
// Created by tom on 9/5/22.
//

#include "Derivative.h"

float &Derivative::operator()(int x, int y) {
    return operator()(0,x,y);
}

float &Derivative::operator()(int d, int x, int y) {
    return data[x + width * (y + d * height)];
}

void Derivative::setData(float *dataPointer) {
    data = dataPointer;
}

Derivative::~Derivative() {
    delete [] data;
}

Derivative::Derivative()=default;
