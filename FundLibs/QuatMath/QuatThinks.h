#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include "quaternion.h"

void print(quat Q);

quat n(quat q);
quat RQ(double a, quat n);