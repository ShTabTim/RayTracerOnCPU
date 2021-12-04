#include "HyCoThinks.h"

#include <cmath>
#include <iostream>
#include <string>

void print(hyco hc) { std::cout << "[ " << hc.re << " + i*" << hc.im.i << " + j*" << hc.im.j << " + k*" << hc.im.k << " ]"; }
void print(puco pc) { std::cout << "[ i*" << pc.i << " + j*" << pc.j << " + k*" << pc.k << " ]"; }

hyco n(hyco hc) { return hc / sqrt((hc * s(hc)).re); }
puco n(puco pc) { return pc / sqrt((-pc*pc).re); }
hyco RQ(double a, puco n) { return hyco(cos(a / 2),	n * sin(a / 2) );}