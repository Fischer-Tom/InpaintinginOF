#include <iostream>
#include <vector>
#include <string>
#include "Image/Image.h"
#include "Flow/Flow.h"

using namespace std;


int main() {
    string file_path;
    Image<float> image1;
    Image<float> image2;
    Image<float> mask;

    Flow groundTruth;
    Flow flow;

    cout << "Image 1: "<<endl;
    getline(cin, file_path);
    image1.readFromPNG(file_path);

    cout << "Image 2: "<<endl;
    getline(cin, file_path);
    image2.readFromPNG(file_path);

    cout << "Mask: "<<endl;
    getline(cin, file_path);
    mask.readFromPNG(file_path);

    cout << "Flow Field "<<endl;
    getline(cin, file_path);
    groundTruth.readFromFlo(file_path);

    // Mask the ground Truth
    flow = Flow(mask, groundTruth);

    // INPAINT


    image1.writeToPNG("A.png");
    image2.writeToPNG("B.png");
    mask.writeToPNG("C.png");
    flow.writeToFlo("D.flo");

    return 0;
}
