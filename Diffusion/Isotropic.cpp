//
// Created by tom on 07.09.22.
//

#include "Isotropic.h"


void step(Flow& flow, Flow& newflow, Image<float>& mask, float tau){

    int aX = flow.getWidth();
    int aY = flow.getHeight();
    float fxx, fyy;
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


void isodiff(Image<float> I1,Image<float>& mask, Flow& flow, Flow& newflow, float tau, int t){


    for (int i=0;i<t;i++){
        //std::cout<<i<<std::endl;
        step(flow,newflow, mask, tau);
    }

}


void diffusivity(float& v1, float& v2, float& g, float lambda) {

    g = 1 / ((v1*v1 + v2*v2) / (lambda*lambda));
}

void edgeImage(float ***gradientImage, Image<float> I1) {
    for (int i=1;i<I1.getWidth()+1;i++)
        for (int j=1;j<I1.getHeight()+1;j++){
            float helpX = -I1(0,i-1,j)+ I1(0,i+1,j);
            float helpY = -I1(0,i,j-1)+ I1(0,i,j+1);
            gradientImage[0][i][j] = helpX * helpX + helpY * helpY;
            helpX = -I1(1,i-1,j) + I1(1,i+1,j);
            helpY = -I1(1,i,j-1) +I1(1,i,j+1);
            gradientImage[1][i][j] = helpX * helpX + helpY * helpY;
            helpX = -I1(2,i-1,j) + I1(2,i+1,j);
            helpY = -I1(2,i,j-1) + I1(2,i,j+1);
            gradientImage[2][i][j] = helpX * helpX + helpY * helpY;
        }
}