#include "quaternion.h"

quat::quat(qType d, qType i, qType j, qType k) : d_(d), i_(i), j_(j), k_(k) {}
quat::quat(qType i, qType j, qType k) : d_(0), i_(i), j_(j), k_(k){}
quat::quat(qType d) : d_(d), i_(0), j_(0), k_(0){}
quat s(quat q) {return quat( q.d_, -q.i_, -q.j_, -q.k_);}
quat quat::operator ! () { return s(*this) / (*this * s(*this)).d_; }
quat quat::operator - () {return quat(-d_, -i_, -j_, -k_);}
quat quat::operator + (quat const& o) {return quat(d_ + o.d_, i_ + o.i_, j_ + o.j_, k_ + o.k_);}
quat quat::operator - (quat const& o) {return quat(d_ - o.d_, i_ - o.i_, j_ - o.j_, k_ - o.k_);}
quat quat::operator += (quat const& o) {
	d_ += o.d_;
	i_ += o.i_;
	j_ += o.j_;
	k_ += o.k_;
	return *this;
}
quat quat::operator &&(quat const& o) {
	return quat(d_*o.d_, i_ * o.i_, j_ * o.j_, k_*o.k_);
}

quat quat::operator -= (quat const& o) {
	d_ -= o.d_;
	i_ -= o.i_;
	j_ -= o.j_;
	k_ -= o.k_;
	return *this;
}

quat quat::operator * (quat const& o) {
	return quat(
		d_*o.d_ - i_*o.i_ - j_*o.j_ - k_*o.k_, 
		d_*o.i_ + i_*o.d_ + j_*o.k_ - k_*o.j_,
		d_*o.j_ + j_*o.d_ + k_*o.i_ - i_*o.k_,
		d_*o.k_ + k_*o.d_ + i_*o.j_ - j_*o.i_
	);
}
quat quat::operator / (qType const& o) {return quat(d_/o, i_/o, j_/o, k_/o);}
quat quat::Re() { return quat(d_); }
quat quat::Im() { return quat(i_, j_, k_); }