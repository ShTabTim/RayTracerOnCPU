#include <Windows.h>
#include "FundLibs/ShRendCPU/ShWin.h"
#include "FundLibs/QuatMath/quat.h"

#define ScrWW 512.0f
#define ScrHH 512.0f
#define MaxRenderL 16384
#define RefrIter 10
#define SkyCol quat(0.5, 0.7, 0.9)

class MainRenderer : public ShWinRendCPU {
    quat spherePos[4] = {
        quat(0, 0, 1),
        quat(1, 0, 1),
        quat(0, 1, 1),
        quat(1, 1, 1),
    };

    typedef struct t2f {
        float i, j;
        t2f(float i = -1, float j = -1) : i(i), j(j) {}
    }t2f;

    t2f sphere(quat ro, quat rd, float r) {
        float b = (ro * rd).d_;
        float h = b * b + (ro * ro).d_ + r * r;
        if (h < 0.0) return t2f();
        h = sqrt(h);
        return t2f(b - h, b + h);
    }

    float plane(quat ro, quat rd, quat p, float w) {
        return -((ro*p).d_ - w) / (rd * p).d_;
    }

    bool initSim() {
        AppName = L"RTCPU";

        return true;
    }

    float dot(quat a, quat b) {
        return a.i_ * b.i_ + a.j_ * b.j_ + a.k_ * b.k_;
    }

    quat reflect(quat rd, quat norm) { return rd + norm * 2 * (norm* rd).d_; }

    quat light = n(quat(0, 1, 0));

    quat castRay(quat *ro, quat *rd) {
        quat col(0);
        float albed = 1, diff = 1;
        bool free = true;
        float l = MaxRenderL, lMax = MaxRenderL;
        quat norm(0, -1, 0);

        for (quat sp : spherePos) {
            l = sphere(*ro - sp, *rd, 1).i;
            if (l > 0 && l < lMax) {
                col = sp;
                norm = n(*ro + *rd*l - sp);
                lMax = l;
                //albed = (rand() % 5) / 10.0f + 0.5f;
                free = false;
            }
        }

        l = plane(*ro, *rd, n(quat(0.1, 1, 0)), 0);
        if (l > 0 && l < lMax) {
            lMax = l;
            col = quat(1, 1, 0);
            norm = n(quat(0.1, 1, 0));
            albed = 0.5;// (rand() % 5) / 10.0f + 0.25f;
            free = false;
        }

        if (free)
            return SkyCol - _1;
        else {
            *ro += *rd * (lMax - 0.001);
            *rd += norm * 2 * (*rd * norm).d_;
            diff *= ((-norm * light).d_ * 0.5 + 0.5)*albed;
        }

        return col *diff;
    }

    color traceRay(quat ro, quat rd) {
        quat col(1, 1, 1), itCol(0);

        for (int refIt = 1; refIt <= RefrIter; ++refIt) {
            itCol = castRay(&ro, &rd);
            //col = itCol;
            col.i_ *= itCol.i_;
            col.j_ *= itCol.j_;
            col.k_ *= itCol.k_;
            if (itCol.d_ < 0)
                break;
        }

        //quat ll = light;
        //col = castRay(&ro, &rd);
        //if (col.i_ < 0) col = SkyCol;
        //else
        //if (castRay(&ro, &ll).i_ >= 0) col = col* 0.5;



        //col.i_ = pow(col.i_, 1/2.2);
        //col.j_ = pow(col.j_, 1/2.2);
        //col.k_ = pow(col.k_, 1/2.2);

        return color(255 * col.i_, 255 * col.j_, 255 * col.k_);
    }

    void rend(float dt) {
        static quat q(0, 0, 0), p(1, 0, 0), g = RQ(0.2f , n(quat(1, 1, 1)));
        p = !g * p * g;
        for (int i = 0; i < db.GetW(); ++i)
            for (int j = 0; j < db.GetH(); ++j) {
                quat uv( 2 * i / ScrWW - 1, 2 * j / ScrHH - 1, 0);
                quat ro = p + uv / 2 - _k*3 + _j;
                quat rd = n(uv/2+_k);
                db.DrawPixel(i, j, traceRay(ro, rd));
                //db.DrawPixelHalf(0.2f, i, j, traceRay(ro, rd));
            }
    }

    bool loopSim(float dt) {
        rend(dt);
        return true;
    }
    bool finitSim() {
        return true;
    }
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
    if (uiMsg == WM_CLOSE)
        PostQuitMessage(0);
    else
        return DefWindowProc(hWnd, uiMsg, wParam, lParam);
    return 0;
}
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR sCmdLine, int iShow) {
    MainRenderer simulation;
    if (simulation.init(hInst, ScrWW, ScrHH, WndProc))
        simulation.Start();
    return 0;
}