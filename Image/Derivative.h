//
// Created by tom on 9/5/22.
//

#ifndef DIFFUSIONINOF_DERIVATIVE_H
#define DIFFUSIONINOF_DERIVATIVE_H


class Derivative {

public:
    // Constructors
    Derivative();

    // Destructor
    ~Derivative();

    void setData(float *dataPointer);

    // operators
    inline float& operator() (int d, int x, int y);
    inline float& operator() (int x, int y);

private:
    int width{},height{},depth{};
    float *data{};
};


#endif //DIFFUSIONINOF_DERIVATIVE_H
