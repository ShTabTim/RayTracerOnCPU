#include <Windows.h>
#include "FundLibs/ShRendCPU/ShWin.h"
#include "FundLibs/HyCoMath/hyco.h"
#include "Renderer.h"

#define ScrWW 512.0f
#define ScrHH 512.0f
#define MaxRenderL 16384
#define TraceSamples 80
#define RefrIter 20
#define SkyCol puco(0.4, 0.6, 0.7)
#define DefOldCol puco(0.1, 0.1, 0.1)

enum material {
    mirror,
    diffuse,
    light
};

typedef struct body {
    puco norm = 0;
    puco col = 0;
    float lMax = MaxRenderL;
    material mat = mirror;
}body;

void updCam(hyco* rx, hyco* ry, puco* pos, float dt) {
    //static float t = -2;
    //t += dt/2;
    //*ry = hyco(sin(t/4), 0, cos(t/4), 0);
    //*rx = RQ(-t, puco(1, 0, 0));
    //*pos += puco(0, 0, -0.4f) * dt;
}

class MainRenderer : public RayTracer {
    puco spherePos[8] = {
        puco(0, 0, 0),
        puco(1, 0, 0),
        puco(0, 1, 0),
        puco(1, 1, 0),
        puco(0, 0, 1),
        puco(1, 0, 1),
        puco(0, 1, 1),
        puco(1, 1, 1),
    };

    bool initSim() {
        AppName = L"RTCPU";
        cam.init(updCam);
        EyeInertia = 0;
        return true;
    }

    puco lSp = puco(5, 5, 0);

    void intersect(puco ro, puco rd, body* bod) {
        float l = MaxRenderL;
        bod->lMax = MaxRenderL;

        for (puco sp : spherePos) {
            l = sphere(ro - sp, rd, 0.5).i;
            if (l > 0 && l < bod->lMax) {
                bod->col = (sp);
                bod->norm = n(ro + rd * l - sp);
                bod->lMax = l;
                bod->mat = diffuse;
            }
        }

        l = plane(ro, rd, _j).i;
        if (l > 0 && l < bod->lMax) {
            bod->col = puco(1, 1, 0.8);
            bod->norm = _j;
            bod->lMax = l;
            bod->mat = diffuse;
        }

        l = plane(ro - puco(-2, 4, 2), rd, _i).i;
        if (l > 0 && l < bod->lMax) {
            bod->col = puco(1, 1, 0.2);
            bod->norm = _i;
            bod->lMax = l;
            bod->mat = mirror;
        }

        l = sphere(ro - lSp, rd, 2).i;
        if (l > 0 && l < bod->lMax) {
            bod->col = puco(1, 1, 0);
            bod->norm = n(ro + rd * l - lSp);
            bod->lMax = l;
            bod->mat = light;
        }
    }

    puco castRay(puco ro, puco rd, int iter) {
        if (iter == 0)
            return DefOldCol;
        else {
            body bod;

            intersect(ro, rd, &bod);

            if (bod.lMax == MaxRenderL)
                return SkyCol;
            else {
                switch (bod.mat) {
                case light:
                    return bod.col;
                case mirror:
                    ro += rd * (bod.lMax - 0.0001);
                    rd = n(rd - bod.norm * 2 * (-rd * bod.norm).re);
                    return bod.col * 0.9 && castRay(ro, rd, iter - 1);
                    break;
                case diffuse:
                    ro += rd * (bod.lMax - 0.0001);
                    puco ref = rd + bod.norm * 2 * (rd * bod.norm).re;
                    puco randTAn((rand() % 19) / 20.0f, 0.1 + (rand() % 19) / 20.0f, 0.1 + (rand() % 19) / 20.0f);
                    randTAn = -randTAn / (bod.norm * randTAn).re;
                    rd = n(randTAn);
                    return bod.col * 0.8 && castRay(ro, rd, iter - 1);
                    break;
                }
            }

        }
    }

    color traceRay(puco ro, puco rd) {
        puco col(0);

        for (int i = 0; i < TraceSamples; i++)
            col += castRay(ro, rd, RefrIter) / TraceSamples;

        return color(255 * col.i, 255 * col.j, 255 * col.k);
    }

    void rend() {
        //static hyco rot(cos(0.1), 0, sin(0.1), 0);

        //lSp = !rot * lSp * rot;

        for (int i = 0; i < ScrWW; i++)
            for (int j = 0; j < ScrHH; j++) {
                puco uv( 2 * i / ScrWW - 1, 2 * j / ScrHH - 1, 0);
                puco ro = cam.pos + !cam.ry * (!cam.rx * (uv / 2) * cam.rx) * cam.ry;
                puco rd = !cam.ry * (!cam.rx * n(uv / 2 + _k) * cam.rx) * cam.ry;
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