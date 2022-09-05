//
// Created by tom on 9/5/22.
//
#include "Image.h"
#include "Flow.h"

void diffuse(Image<float> I1, Flow Flow) {
    I1.setDerivatives();
    Flow.setDerivative();


}