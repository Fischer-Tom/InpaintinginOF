//
// Created by tom on 9/2/22.
//

#include "Flow.h"
#include <iostream>
#include <cmath>

#define FLO_TAG 202021.25


Flow::Flow():width(0),height(0){
}
Flow::Flow(int width, int height):width(width),height(height){
}
Flow::Flow(Image<float>& mask, Flow& groundTruth){
    width = mask.getWidth();
    height = mask.getHeight();
    data = (float*)malloc(sizeof(float)*width*height*2);

    for (int i=0;i<width;i++)
        for (int j=0;j<height;j++) {
            if (mask(i,j,0) == 1.) {
                operator()(i, j, 0) = groundTruth(i, j, 0);
                operator()(i, j, 1) = groundTruth(i, j, 1);
            }
            else{
                operator()(i,j,0) = 0.;
                operator()(i,j,1) = 0.;
            }
        }
}
// copy constructor
Flow::Flow(const Flow& aCopyFrom){
    width = aCopyFrom.width;
    height = aCopyFrom.height;
    if (width == 0 || height == 0){
        data = nullptr;
    }
    else{
        int size = width * height * 2;
        for(int i=0;i<size;i++){
            data[i] = aCopyFrom.data[i];
        }
    }
}

// destructor
Flow::~Flow(){
    delete [] data;
}

// I0 Operations
void Flow::readFromFlo(const std::string &filename){
    FILE *fileStream;
    float help;
    fileStream = fopen(filename.c_str(), "rb");
    if (fileStream == 0)
        std::cout << "COULD NOT OPEN " << filename << std::endl;

    fread(&help,sizeof(float),1,fileStream);
    if (help != FLO_TAG) {
        std::cout << "FLOW TAG DOES NOT MATCH" << std::endl;
    }
    fread(&height,sizeof(int),1,fileStream);
    fread(&height,sizeof(int),1,fileStream);

    data = (float*)malloc(sizeof(float) * (width+1) * (height+1) * 2);

    for (int y = 1; y < height+1; y++)
        for (int x = 1; x < width+1; x++) {
            fread(&help,sizeof(float),1,fileStream);
            operator()(0,x,y) = help;
            fread(&operator()(x,y,1),sizeof(float),1,fileStream);
            operator()(1,x,y) = help;
        }

    // Set boundary with reflective padding
    for (int y=0;y<height+2;y++){
        operator()(0,0,y) = operator()(0,1,y);
        operator()(1,0,y) = operator()(1,1,y);
        operator()(0,width+2,y) = operator()(0,width+1,y);
        operator()(1,width+2,y) = operator()(1,width+1,y);
    }
    for (int x=0;x<width+2;x++){
        operator()(0,x,0) = operator()(0,x,1);
        operator()(1,x,0) = operator()(1,x,1);
        operator()(0,x,height+2) = operator()(0,x,height+1);
        operator()(1,x,height+2) = operator()(1,x,height+1);
    }
    fclose(fileStream);

}
void Flow::writeToFlo(const std::string &filename) {
    FILE *fileStream;
    fileStream = fopen(filename.c_str(), "wb");
    fprintf(fileStream,"PIEH");;
    if ((int)fwrite(&width,sizeof(int),1,fileStream) != 1 ||	(int)fwrite(&height,sizeof(int),1,fileStream) != 1)
        std::cout << "PROBLEM WRITING HEADER" << std::endl;
    // write the data
    for (int y = 1; y < height+1; y++)
        for (int x = 1; x < width+1; x++) {
            float u = operator()(x,y,0);
            float v = operator()(x,y,1);
            fwrite(&u,sizeof(float),1,fileStream);
            fwrite(&v,sizeof(float),1,fileStream);
        }
    fclose(fileStream);
}

// Utilities
double Flow::EPE(Flow& reference) {
    double EPE = 0;
    double helper;
    if (width != reference.width || height != reference.height) {
        std::cout << "DIMENSIONS DO NOT MATCH" << std::endl;
    }

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            helper = (operator()(0, i, j) - reference(0, i, j)) *
                     (operator()(0, i, j) - reference(0, i, j)) +
                     (operator()(1, i, j) - reference(1, i, j)) *
                     (operator()(1, i, j) - reference(1, i, j));
            EPE += sqrt(helper);
        }
    }
    return EPE;
}

// Operators
float &Flow::operator()(int d, int x, int y) {
    return data[x + width * (y + d * height)];
}

void Flow::setDerivative(float h) {

    auto* fxp = (float*)malloc(sizeof(float) * (width+2) * (height+2) * 2);
    auto* fyp = (float*)malloc(sizeof(float) * (width+2) * (height+2) * 2);
    auto* fxxp = (float*)malloc(sizeof(float) * (width+2) * (height+2) * 2);
    auto* fyyp = (float*)malloc(sizeof(float) * (width+2) * (height+2) * 2);
    auto* fxyp = (float*)malloc(sizeof(float) * (width+2) * (height+2) * 2);
    fx.setData(fxp);
    fy.setData(fyp);
    fxx.setData(fxxp);
    fyy.setData(fyyp);
    fxy.setData(fxyp);
    for (int d=0;d<2;d++)
        for (int i=1;i<width;i++)
            for (int j=1;j<height;j++){
                fx(d,i,j) = (0.5f* (operator()(d,i+1,j) + operator()(d,i,j)) -
                             0.5f*(operator()(d,i,j)+operator()(d,i-1,j))) / (2*h);
                fy(d,i,j) = (0.5f* (operator()(d,i,j+1) + operator()(d,i,j)) -
                             0.5f*(operator()(d,i,j)+operator()(d,i,j-1))) / (2*h);
                fxx(d,i,j) = (operator()(d,i-1,j) - 2* operator()(d,i,j) + operator()(d,i+1,j)) / (h*h);
                fxx(d,i,j) = (operator()(d,i,j+1) - 2* operator()(d,i,j) + operator()(d,i,j+1)) / (h*h);
                fxy(d,i,j) = (operator()(d,i+1,j+1)-operator()(d,i+1,j-1)-
                              operator()(d,i-1,j+1)-operator()(d,i-1,j-1))/(4*h*h);
            }
}
