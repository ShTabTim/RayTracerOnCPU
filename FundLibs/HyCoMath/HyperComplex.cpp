#include "HyperComplex.h"

puco::puco(qType i, qType j, qType k):i(i), j(j), k(k) {}
puco::puco(hyco hc) : i(hc.im.i), j(hc.im.j), k(hc.im.k) {}

puco puco::operator ! () { return *this / (*this * *this).re; }
puco puco::operator - () { return puco(-i, -j, -k); }
puco puco::operator + (puco const& o) { return puco(i + o.i, j + o.j, k + o.k); }
puco puco::operator - (puco const& o) { return puco(i - o.i, j - o.j, k - o.k); }
puco puco::operator += (puco const& o){ i += o.i; j += o.j; k += o.k; return *this; }
puco puco::operator -= (puco const& o){ i -= o.i; j -= o.j; k -= o.k; return *this; }
puco puco::operator &&(puco const& o) { return puco(o.i * i, o.j * j, o.k * k); }
puco puco::operator * (qType const& o){ return puco(i * o, j * o, k * o); }
puco puco::operator / (qType const& o){ return puco(i / o, j / o, k / o); }
hyco puco::operator * (puco const& o) { return hyco( - i * o.i - j * o.j - k * o.k,	j * o.k - k * o.j,	- i * o.k + k * o.i,	i * o.j - j * o.i); }

hyco::hyco(qType r, qType i, qType j, qType k) : re(r), im(i, j, k) {}
hyco::hyco(qType i, qType j, qType k) : re(0), im(i, j, k){}
hyco::hyco(qType r, puco im) : re(r), im(im){}
hyco::hyco(puco im) : re(0), im(im){}
hyco::hyco(qType r) : re(r), im(0){}

hyco s(hyco q) { return hyco( q.re, -q.im); }
hyco hyco::operator ! () { return s(*this) / (*this * s(*this)).re; }
hyco hyco::operator - () { return hyco(-re, -im); }
hyco hyco::operator + (hyco const& o) { return hyco(re + o.re, im + o.im); }
hyco hyco::operator - (hyco const& o) { return hyco(re - o.re, im - o.im); }
hyco hyco::operator += (hyco const& o) { re += o.re; im += o.im; return *this; }
hyco hyco::operator -= (hyco const& o) { re -= o.re; im -= o.im; return *this; }
hyco hyco::operator &&(hyco const& o) { return hyco(re*o.re, im && o.im); }
hyco hyco::operator * (qType const& o) {return hyco(re*o, im*o);}
hyco hyco::operator / (qType const& o) {return hyco(re/o, im/o);}
hyco hyco::operator * (hyco const& o) {
	return hyco(
		re *  o.re  - im.i * o.im.i - im.j * o.im.j - im.k * o.im.k,
		re * o.im.i + im.i *  o.re  + im.j * o.im.k - im.k * o.im.j,
		re * o.im.j - im.i * o.im.k + im.j *  o.re  + im.k * o.im.i,
		re * o.im.k + im.i * o.im.j - im.j * o.im.i + im.k *  o.re
	);
}