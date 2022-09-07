//
// Created by tom on 9/5/22.
//

#include "Derivative.h"
#include <stdlib.h>

double &Derivative::operator()(int x, int y) {
    return operator()(0,x,y);
}

double &Derivative::operator()(int d, int x, int y) {
    return data[x + width * (y + d * height)];
}


Derivative::~Derivative() {
    delete [] data;
}

void Derivative::initData(int w, int h, int d) {
    if (data == nullptr)
        data = (double *) malloc(sizeof(double) * (w + 2) * (h + 2) * d);
}

Derivative::Derivative()=default;
