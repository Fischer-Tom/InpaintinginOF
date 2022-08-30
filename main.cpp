#include <iostream>
#include "Image/Image.h"

int main() {
    std::cout << "Hello World"<<std::endl;
    Image<float> M;
    M.readFromPNG("./img1.png");
    M.writeToPNG("test.png");
    ~M;
    return 0;
}
