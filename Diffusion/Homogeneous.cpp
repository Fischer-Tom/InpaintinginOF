//
// Created by tom on 9/5/22.
//
#include "Homogeneous.h"



void diffuse(Flow& flow, Flow& newflow, Image<float>& mask, float tau) {
    int aX = flow.getWidth();
    int aY = flow.getHeight();
    double fxx, fyy;
    newflow.setBoundary();
    flow.setBoundary();
    for (int i=1;i<aX+1; i++)
        for (int j=1;j<aY+1;j++) {
            if(mask(i,j) == 1.){
                newflow(0,i,j) = flow(0,i,j);
                newflow(1,i,j) = flow(1,i,j);
            }
            else{
                fxx = flow(0,i-1,j) - 2. * flow(0,i,j) + flow(0,i+1,j);
                fyy =  flow(0,i,j-1) - 2. * flow(0,i,j) + flow(0,i,j+1);
                newflow(0,i,j) = flow(0,i,j) + tau*(fxx + fyy);
                fxx = flow(1,i-1,j) - 2. * flow(1,i,j) + flow(1,i+1,j);
                fyy =  flow(1,i,j-1) - 2. * flow(1,i,j) + flow(1,i,j+1);
                newflow(1,i,j) = flow(1,i,j) + tau*(fxx+fyy);
            }
        }
    for (int i=1;i<aX+1; i++)
        for (int j=1;j<aY+1;j++) {
            flow(0,i,j) = newflow(0,i,j);
            flow(1,i,j) = newflow(1,i,j);
        }
}

void homdiff(Image<float>& mask, Flow& flow, Flow& newflow, float tau, int t) {


    for (int i=0;i<t;i++){
        //std::cout<<i<<std::endl;
        diffuse(flow,newflow, mask, tau);
    }
}