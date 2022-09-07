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

    void initData(int w, int h, int d);

    // operators
    double& operator() (int d, int x, int y);
    double& operator() (int x, int y);

private:
    int width{},height{},depth{};
    double *data{};
};


#endif //DIFFUSIONINOF_DERIVATIVE_H
