#pragma once
#include "FundLibs/ShRendCPU/ShWin.h"
#include "FundLibs/QuatMath/quat.h"

class camera {
private:
    void (*fncUpd)(quat*, quat*, quat*, float) = 0;
public:
    quat rx = RQ(0, n(quat(1, 0, 0)));
    quat ry = RQ(0, n(quat(0, 1, 0)));
    quat pos = quat(1, 1, -7);

    void init(void (*fncUpd)(quat*, quat*, quat*, float)) {
        this->fncUpd = fncUpd;
    }

    void camUpdate(float dt) {
        fncUpd(&rx, &ry, &pos, dt);
    }
};

class RayTracer : public ShWinRendCPU {

protected:
    float EyeInertia = 0.1f;

    camera cam;

    bool loopSim(float dt) {
        cam.camUpdate(dt);
        rend();
        return true;
    }

    virtual void rend() = 0;
};

typedef struct t2f {
    float i, j;
    t2f(float i, float j) : i(i), j(j) {}
    t2f(float g = -1) : i(g), j(g) {}
}t2f;

t2f sphere(quat ro, quat rd, float r) {
    float b = (ro * rd).d_;
    float h = b * b + (ro * ro).d_ + r * r;
    if (h < 0.0) return t2f();
    h = sqrt(h);
    return t2f(b - h, b + h);
}

t2f plane(quat ro, quat rd, quat p, float w) {
    return -((ro * p).d_ - w) / (rd * p).d_;
}
