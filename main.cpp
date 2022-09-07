#include <iostream>
#include <vector>
#include <string>
#include "Image/Image.h"
#include "Flow/Flow.h"
#include "Diffusion/Homogeneous.h"

using namespace std;


int main(int argc, char** argv) {
    string file_path;
    Image<float> image1;
    Image<float> image2;
    Image<float> mask;

    Flow groundTruth;

    if (argc == 0) {
        cout << "Image 1: " << endl;
        getline(cin, file_path);
        image1.readFromPNG(file_path);

        cout << "Image 2: " << endl;
        getline(cin, file_path);
        image2.readFromPNG(file_path);

        cout << "Mask: " << endl;
        getline(cin, file_path);
        mask.readFromPNG(file_path);

        cout << "Flow Field " << endl;
        getline(cin, file_path);
        groundTruth.readFromFlo(file_path);
    }
    else{
        file_path = argv[1];
        image1.readFromPNG(file_path);
        file_path = argv[2];
        image2.readFromPNG(file_path);
        file_path = argv[3];
        mask.readFromPNG(file_path);
        file_path = argv[4];
        groundTruth.readFromFlo(file_path);
    }
    // Mask the ground Truth
    Flow flow = Flow(mask, groundTruth);
    Flow newflow = Flow(flow.getWidth(),flow.getHeight());
    flow.drawFlow("out1.png");

    // INPAINT
    homdiff(mask, flow, newflow, 0.25, 10000);

    image1.writeToPNG("A.png");
    image2.writeToPNG("B.png");
    mask.writeToPNG("C.png");
    flow.writeToFlo("D.flo");
    flow.drawFlow("out2.png");
    return 0;
}
