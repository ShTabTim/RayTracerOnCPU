#include <Windows.h>
#include "FundLibs/ShRendCPU/ShWin.h"
#include "FundLibs/HyCoMath/hyco.h"
#include "Renderer.h"

#include <thread>

#define ScrWW 128.0f
#define ScrHH 128.0f
#define MaxRenderL 16384
#define TraceSamples 2
#define RefrIter 5
#define SkyCol puco(0.4, 0.6, 0.7)
#define DefOldCol puco()//(0.1, 0.1, 0.1)

#define K3 1.73205080756887729
#define K3on2 0.8660254037844386


enum material {
    mirror,
    diffuse,
    gloss,
    light
};

typedef struct body {
    puco norm = 0;
    puco col = 0;
    float lMax = MaxRenderL;
    material mat = mirror;
    float glossy;
    float bright = 0.8;
}body;

bool Wu = false;
bool Su = false;
bool Au = false;
bool Du = false;
bool Spu = false;
bool Shu = false;
bool Rau = false;
bool Lau = false;
bool Uau = false;
bool Dau = false;
float ya = 0;
float xa = 0;
float speed = 2;
puco vel = puco(0, 0, 0);

void updCam(hyco* rx, hyco* ry, puco* pos, float dt) {
    vel = 0;
    if (Wu)
        vel.k += speed;
    if (Su)
        vel.k -= speed;
    if (Du)
        vel.i += speed;
    if (Au)
        vel.i -= speed;
    if (Spu)
        vel.j += speed;
    if (Shu)
        vel.j -= speed;

    if (Rau)
        ya -= dt;
    if (Lau)
        ya += dt;
    if (Uau)
        xa += dt;
    if (Dau)
        xa -= dt;

    *ry = hyco(cos(ya), 0, sin(ya), 0);
    *rx = hyco(cos(xa), sin(xa), 0, 0);
    *pos += !*ry*(vel * dt)**ry;
}

class MainRenderer : public RayTracer {
    puco spherePos[38] = {
        puco(0, 0, 0),
        puco(1, 0, 0),
        puco(2, 0, 0),
        puco(3, 0, 0),

        puco(0, 0, 1),
        puco(3, 0, 1),
        puco(0, 0, 2),
        puco(3, 0, 2),

        puco(0, 0, 3),
        puco(1, 0, 3),
        puco(2, 0, 3),
        puco(3, 0, 3),


        puco(0, 3, 0),
        puco(1, 3, 0),
        puco(2, 3, 0),
        puco(3, 3, 0),
                
        puco(0, 3, 1),
        puco(3, 3, 1),
        puco(0, 3, 2),
        puco(3, 3, 2),
                
        puco(0, 3, 3),
        puco(1, 3, 3),
        puco(2, 3, 3),
        puco(3, 3, 3),


        puco(0, 1, 0),
        puco(3, 1, 0),
        puco(0, 1, 3),
        puco(3, 1, 3),


        puco(0, 2, 0),
        puco(3, 2, 0),
        puco(0, 2, 3),
        puco(3, 2, 3),
    };

    bool initSim() {
        AppName = L"RTCPU";
        cam.init(updCam);
        EyeInertia = 0.5;
        return true;
    }

    puco lSp = puco(7, 7, 0);

    void intersect(puco ro, puco rd, body* bod) {
        float l = MaxRenderL;
        bod->lMax = MaxRenderL;

        for (puco sp : spherePos) {
            l = sphere(ro - sp, rd, K3on2).i;
            if (l > 0 && l < bod->lMax) {
                bod->col = n(sp);
                bod->norm = n(ro + rd * l - sp);
                bod->lMax = l;
                bod->mat = gloss;
                bod->glossy = 2;
            }
        }

        l = plane(ro, rd, _j).i;
        if (l > 0 && l < bod->lMax) {
            float kol = abs(sin(rd.i * l + ro.i) * sin(rd.k * l + ro.k));
            bod->col = puco(kol, kol, kol);
            bod->norm = _j;
            bod->lMax = l;
            bod->mat = gloss;
            bod->glossy = 10;
        }

        l = plane(ro - puco(-2, 4, 2), rd, _i).i;
        if (l > 0 && l < bod->lMax) {
            bod->col = puco(0.8, 0.8, 0.2);
            bod->norm = _i;
            bod->lMax = l;
            bod->mat = mirror;
        }

        l = sphere(ro - lSp, rd, 2).i;
        if (l > 0 && l < bod->lMax) {
            bod->norm = n(ro + rd * l - lSp);
            float colr = 
                cos(acos((n(puco(bod->norm.i, 0, bod->norm.k)) * _i).re) * 8) *
                cos(acos((n(puco(bod->norm.i, 0, bod->norm.k)) * bod->norm).re) * 8);
            colr *= colr;
            bod->col = puco(colr, colr, colr);
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
                    return bod.col * bod.bright && castRay(ro, rd, iter - 1);
                    break;
                case diffuse: {
                    ro += rd * (bod.lMax - 0.0001);
                    puco randTAn((rand() % 80 - 40) / 40.0f, (rand() % 80 - 40) / 40.0f, (rand() % 80 - 40) / 40.0f);
                    randTAn = -randTAn / (bod.norm * randTAn).re;
                    rd = n(randTAn); }
                    return bod.col * bod.bright && castRay(ro, rd, iter - 1);
                    break;
                case gloss: {
                    ro += rd * (bod.lMax - 0.0001);
                    puco ref = rd + bod.norm * 2 * (rd * bod.norm).re;
                    puco randTAn((rand() % 80 - 40) / 40.0f, (rand() % 80 - 40) / 40.0f, (rand() % 80 - 40) / 40.0f);
                    float gloss = -(ref * bod.norm).re;
                    randTAn = n(-randTAn / (ref * randTAn).re) / bod.glossy + ref * (sqrt(1 - gloss * gloss) + K3 / 2) / gloss;
                    rd = n(randTAn); }
                    return bod.col * bod.bright && castRay(ro, rd, iter - 1);
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

    void rendThread(int my, int numTh, int seed) {
        srand(seed);
        for (int i = my/(float)numTh*ScrWW; i < (my+1) / (float)numTh*ScrHH; i++)
            for (int j = 0; j < ScrHH; j++) {
                puco uv(2 * i / ScrWW - 1, 2 * j / ScrHH - 1, 0);
                puco ro = cam.pos + !cam.ry * (!cam.rx * (uv / 2) * cam.rx) * cam.ry;
                puco rd = !cam.ry * (!cam.rx * n(uv / 2 + _k) * cam.rx) * cam.ry;
                //db.Draw4PixelHalf((30 + rand() % 10) / 40.0f * EyeInertia, i, j, traceRay(ro, rd));
                db.Draw16PixelHalf(EyeInertia, i, j, traceRay(ro, rd));
            }
    }

    void rend() {
        Wu = GetKey('W').bHeld;
        Au = GetKey('A').bHeld;
        Su = GetKey('S').bHeld;
        Du = GetKey('D').bHeld;

        Spu = GetKey(VK_SPACE).bHeld;
        Shu = GetKey(VK_SHIFT).bHeld;

        Rau = GetKey(VK_RIGHT).bHeld;
        Lau = GetKey(VK_LEFT).bHeld;
        Uau = GetKey(VK_UP).bHeld;
        Dau = GetKey(VK_DOWN).bHeld;

        std::thread th0(&MainRenderer::rendThread, this, 0, 8, rand());
        std::thread th1(&MainRenderer::rendThread, this, 1, 8, rand());
        std::thread th2(&MainRenderer::rendThread, this, 2, 8, rand());
        std::thread th3(&MainRenderer::rendThread, this, 3, 8, rand());
        std::thread th4(&MainRenderer::rendThread, this, 4, 8, rand());
        std::thread th5(&MainRenderer::rendThread, this, 5, 8, rand());
        std::thread th6(&MainRenderer::rendThread, this, 6, 8, rand());
        std::thread th7(&MainRenderer::rendThread, this, 7, 8, rand());

        th0.join();
        th1.join();
        th2.join();
        th3.join();
        th4.join();
        th5.join();
        th6.join();
        th7.join();
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
    if (simulation.init(hInst, ScrWW*4, ScrHH*4, WndProc))
        simulation.Start();
    return 0;
}