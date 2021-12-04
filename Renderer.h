#pragma once
#include "FundLibs/ShRendCPU/ShWin.h"
#include "FundLibs/HyCoMath/hyco.h"

class camera {
private:
    void (*fncUpd)(hyco*, hyco*, puco*, float) = 0;
public:
    hyco rx = RQ(0, puco(1, 0, 0));
    hyco ry = RQ(0, puco(0, 1, 0));
    puco pos = puco(1, 1, -7);

    void init(void (*fncUpd)(hyco*, hyco*, puco*, float)) {
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

t2f sphere(puco ro, puco rd, float r) {
    float b = (ro * rd).re;
    float h = b * b + (ro * ro).re + r * r;
    if (h < 0) return t2f();
    h = sqrt(h);
    return t2f(b-h, b+h);
}

t2f plane(puco ro, puco rd, puco p) {
    return -(ro * p).re / (rd * p).re;
}
/*
t2f box(puco ro, puco rd, puco rad, puco* N) {
    puco m = puco(1 / rd.i, 1 / rd.j, 1 / rd.k);
    puco n = m * ro;
    puco k = abs(m) * rad;
    puco t1 = -n - k;
    puco t2 = -n + k;
    float tN = max(max(t1.i, t1.j), t1.k);
    float tF = min(min(t2.i, t2.j), t2.k);
    if (tN > tF || tF < 0) return t2f();
    *N = -sign(rd) * step(puco(t1.j, t1.k, t1.i), t1) * step(puco(t1.k, t1.i, t1.j), t1);
    return t2f(tN, tF);
}
*/