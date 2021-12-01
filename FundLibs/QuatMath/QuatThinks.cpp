#include "QuatThinks.h"
quat n(quat q) { return (q / (q * s(q)).d_).Im(); }

void print(quat Q) { std::cout << "[ 1*" << Q.d_ << " + i*" << Q.i_ << " + j*" << Q.j_ << " + k*" << Q.k_ << " ]"; }

quat RQ(double a, quat n) {
	return quat(
		cos(a / 2),
		n.i_ * sin(a / 2),
		n.j_ * sin(a / 2),
		n.k_ * sin(a / 2)
	);
}