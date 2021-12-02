#include <Windows.h>
#include "FundLibs/ShRendCPU/ShWin.h"
#include "FundLibs/QuatMath/quat.h"
#include "Renderer.h"

#define ScrWW 256.0f
#define ScrHH 256.0f
#define MaxRenderL 16384
#define TraceSamples 60
#define RefrIter 5
#define SkyCol quat(0.5, 0.7, 0.9)
#define DefOldCol quat(0.1, 0.1, 0.1)

void updCam(quat* rx, quat* ry, quat* pos, float dt) {
    //static float t = 0;
    //t += dt/4;
    //*ry = RQ(t, quat(0, 1, 0));
    //*rx = RQ(-t, quat(1, 0, 0));
    //*pos += quat(0, 0, -0.4f) * dt;
}

class MainRenderer : public RayTracer {
    quat spherePos[4] = {
        quat(0, 0, 1),
        quat(1, 0, 1),
        quat(0, 1, 1),
        quat(1, 1, 1),
    };

    bool initSim() {
        AppName = L"RTCPU";
        cam.init(updCam);
        return true;
    }

    quat lSp = quat(5, 5, 0);

    quat castRay(quat ro, quat rd, int iter) {
        if (iter == 0)
            return DefOldCol;
        else {
            quat col(0);
            bool free = true;
            float l = MaxRenderL, lMax = MaxRenderL;
            quat norm(0, 1, 0);

            for (quat sp : spherePos) {
                l = sphere(ro - sp, rd, 1).i;
                if (l > 0 && l < lMax) {
                    col = sp;
                    norm = n(ro + rd * l - sp);
                    lMax = l;
                    free = false;
                }
            }

            l = plane(ro, rd, n(quat(0.1, 1, 0)), 0).i;
            if (l > 0 && l < lMax) {
                lMax = l;
                col = quat(1, 1, 0);
                norm = n(quat(0.1, 1, 0));
                free = false;
            }

            l = sphere(ro - lSp, rd, 5).i;
            if (l > 0 && l < lMax) {
                return quat(1, 1, 1);
            }

            if (free)
                return DefOldCol;
            else {
                ro += rd * (lMax - 0.01);
                quat randTAn = quat((rand() % 19) / 20.0f, 0.1 + (rand() % 19) / 20.0f, 0.1 + (rand() % 19) / 20.0f);
                quat ref = rd + norm * 2 * (rd * norm).d_;
                randTAn = -randTAn / (ref * randTAn).d_;
                rd = n(randTAn);
                return col*0.9 && castRay(ro, rd, iter - 1);
            }

        }
    }

    color traceRay(quat ro, quat rd) {

        quat col(0);

        for (int i = 0; i < TraceSamples; i++)
            col += castRay(ro, rd, RefrIter) / TraceSamples;

        return color(255 * col.i_, 255 * col.j_, 255 * col.k_);
    }

    void rend() {
        for (int i = 0; i < ScrWW; i++)
            for (int j = 0; j < ScrHH; j++) {
                quat uv( 2 * i / ScrWW - 1, 2 * j / ScrHH - 1, 0);
                quat ro = cam.pos + !cam.ry*(!cam.rx*(uv / 2)*cam.rx)*cam.ry;
                quat rd = !cam.ry * n((!cam.rx * (uv / 2 + _k) * cam.rx) * cam.ry);
                db.Draw4PixelHalf(EyeInertia, i, j, traceRay(ro, rd));
            }
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
    if (simulation.init(hInst, ScrWW*2, ScrHH*2, WndProc))
        simulation.Start();
    return 0;
}