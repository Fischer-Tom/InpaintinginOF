//
// Created by tom on 07.09.22.
//

#ifndef DIFFUSIONINOF_ISOTROPIC_H
#define DIFFUSIONINOF_ISOTROPIC_H

#include "Flow.h"


void step(Flow& flow, Flow& old, Image<float>& mask, float tau);
void isodiff(Image<float> I1,Image<float>& mask, Flow& flow, Flow& newflow, float tau, int t);
#endif //DIFFUSIONINOF_ISOTROPIC_H
