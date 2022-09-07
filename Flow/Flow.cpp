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
    data = (float*)malloc(sizeof(float)*(width+2)*(height+2)*2);
}
Flow::Flow(Image<float>& mask, Flow& groundTruth){
    width = mask.getWidth();
    height = mask.getHeight();
    data = (float*)malloc(sizeof(float)*(width+2)*(height+2)*2);

    for (int i=0;i<width+2;i++)
        for (int j=0;j<height+2;j++) {
            if (mask(i,j) == 255.) {
                operator()(0, i, j) = groundTruth(0, i, j);
                operator()(1, i, j) = groundTruth(1, i, j);
                mask(i,j) = 1.;
            }
            else{
                operator()(0,i,j) = 0.;
                operator()(1,i,j) = 0.;
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
        int size = (width+2) * (height+2) * 2;
        data = (float*)malloc(sizeof(float) * (width+2)*(height+2)*2 );
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
    fread(&width,sizeof(int),1,fileStream);
    fread(&height,sizeof(int),1,fileStream);

    data = (float*)malloc(sizeof(float) * (width+2) * (height+2) * 2);

    for (int j = 1; j < height+1; j++)
        for (int i = 1; i < width+1; i++) {
            fread(&help,sizeof(float),1,fileStream);
            operator()(0,i,j) = help;
            fread(&help,sizeof(float),1,fileStream);
            operator()(1,i,j) = help;
        }

    // Set boundary with reflective padding
    setBoundary();
    fclose(fileStream);

}
void Flow::writeToFlo(const std::string &filename) {
    FILE *fileStream;
    fileStream = fopen(filename.c_str(), "wb");
    fprintf(fileStream,"PIEH");;
    if ((int)fwrite(&width,sizeof(int),1,fileStream) != 1 ||	(int)fwrite(&height,sizeof(int),1,fileStream) != 1)
        std::cout << "PROBLEM WRITING HEADER" << std::endl;
    // write the data
    for (int j = 1; j < height+1; j++)
        for (int i = 1; i < width+1; i++) {
            float u = operator()(0,i,j);
            float v = operator()(1,i,j);
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


void Flow::drawFlow(std::string fileName) {
    Image<float> img = Image<float>(width, height, 3);
    float factor = sqrt(0.5) * 0.5/8.;
    float v1,v2;
    for (int i=1;i<width+1;i++)
        for(int j=1;j<height+1;j++){
            float r,g,b;
            v1 = operator()(0,i,j);
            v2 = operator()(1,i,j);
            vecToRGB(factor*v1, factor*v2, r, g, b);
            img(0,i,j) = r;
            img(1,i,j) = g;
            img(2,i,j) = b;
        }
    img.writeToPNG(fileName);
}

void Flow::vecToRGB(float v1, float v2, float &r, float &g, float &b) {
    float radius = sqrt (v1 * v1 + v2 * v2);
    if (radius > 1) radius = 1;
    float phi;
    if (v1 == 0.0)
        if (v2 >= 0.0) phi = 0.5 * M_PI;
        else phi = 1.5 * M_PI;
    else if (v1 > 0.0)
        if (v2 >= 0.0) phi = atan (v2/v1);
        else phi = 2.0 * M_PI + atan (v2/v1);
    else phi = M_PI + atan (v2/v1);
    double alpha, beta;    // weights for linear interpolation
    phi *= 0.5;
    // interpolation between red (0) and blue (0.25 * NMath::Pi)
    if ((phi >= 0.0) && (phi < 0.125 * M_PI)) {
        beta  = phi / (0.125 * M_PI);
        alpha = 1.0 - beta;
        r = (int)(radius * (alpha * 255.0 + beta * 255.0));
        g = (int)(radius * (alpha *   0.0 + beta *   0.0));
        b = (int)(radius * (alpha *   0.0 + beta * 255.0));
    }
    if ((phi >= 0.125 * M_PI) && (phi < 0.25 * M_PI)) {
        beta  = (phi-0.125 * M_PI) / (0.125 * M_PI);
        alpha = 1.0 - beta;
        r = (int)(radius * (alpha * 255.0 + beta *  64.0));
        g = (int)(radius * (alpha *   0.0 + beta *  64.0));
        b = (int)(radius * (alpha * 255.0 + beta * 255.0));
    }
    // interpolation between blue (0.25 * NMath::Pi) and green (0.5 * NMath::Pi)
    if ((phi >= 0.25 * M_PI) && (phi < 0.375 * M_PI)) {
        beta  = (phi - 0.25 * M_PI) / (0.125 * M_PI);
        alpha = 1.0 - beta;
        r = (int)(radius * (alpha *  64.0 + beta *   0.0));
        g = (int)(radius * (alpha *  64.0 + beta * 255.0));
        b = (int)(radius * (alpha * 255.0 + beta * 255.0));
    }
    if ((phi >= 0.375 * M_PI) && (phi < 0.5 * M_PI)) {
        beta  = (phi - 0.375 * M_PI) / (0.125 * M_PI);
        alpha = 1.0 - beta;
        r = (int)(radius * (alpha *   0.0 + beta *   0.0));
        g = (int)(radius * (alpha * 255.0 + beta * 255.0));
        b = (int)(radius * (alpha * 255.0 + beta *   0.0));
    }
    // interpolation between green (0.5 * NMath::Pi) and yellow (0.75 * NMath::Pi)
    if ((phi >= 0.5 * M_PI) && (phi < 0.75 * M_PI)) {
        beta  = (phi - 0.5 * M_PI) / (0.25 * M_PI);
        alpha = 1.0 - beta;
        r = (int)(radius * (alpha * 0.0   + beta * 255.0));
        g = (int)(radius * (alpha * 255.0 + beta * 255.0));
        b = (int)(radius * (alpha * 0.0   + beta * 0.0));
    }
    // interpolation between yellow (0.75 * NMath::Pi) and red (Pi)
    if ((phi >= 0.75 * M_PI) && (phi <= M_PI)) {
        beta  = (phi - 0.75 * M_PI) / (0.25 * M_PI);
        alpha = 1.0 - beta;
        r = (int)(radius * (alpha * 255.0 + beta * 255.0));
        g = (int)(radius * (alpha * 255.0 + beta *   0.0));
        b = (int)(radius * (alpha * 0.0   + beta *   0.0));
    }
    if (r<0) r=0;
    if (g<0) g=0;
    if (b<0) b=0;
    if (r>255) r=255;
    if (g>255) g=255;
    if (b>255) b=255;

}

int Flow::getWidth() {
    return width;
}

int Flow::getHeight() {
    return height;
}

void Flow::setBoundary() {
    for (int y=0;y<height+2;y++){
        operator()(0,0,y) = operator()(0,1,y);
        operator()(1,0,y) = operator()(1,1,y);
        operator()(0,width+1,y) = operator()(0,width,y);
        operator()(1,width+1,y) = operator()(1,width,y);
    }
    for (int x=0;x<width+2;x++){
        operator()(0,x,0) = operator()(0,x,1);
        operator()(1,x,0) = operator()(1,x,1);
        operator()(0,x,height+1) = operator()(0,x,height);
        operator()(1,x,height+1) = operator()(1,x,height);
    }
}
