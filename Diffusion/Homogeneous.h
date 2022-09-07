//
// Created by tom on 07.09.22.
//

#ifndef DIFFUSIONINOF_HOMOGENEOUS_H
#define DIFFUSIONINOF_HOMOGENEOUS_H

#include "Flow.h"


void diffuse(Flow& flow, Flow& newflow, Image<float>& mask, float tau);
void homdiff(Image<float>& mask, Flow& flow, Flow& newflow, float tau, int t);
#endif //DIFFUSIONINOF_HOMOGENEOUS_H
