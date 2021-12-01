#pragma once
#define _1 quat(1)
#define _i quat(1, 0)
#define _j quat(0, 1)
#define _k quat(0, 0, 1)

#define qType float

typedef struct quat {
	qType d_, i_, j_, k_;
	quat(qType d, qType i, qType j, qType k);
	quat(qType i, qType j, qType k);
	quat(qType d);
	quat operator ! ();
	quat operator - ();
	quat operator + (quat const& other);
	quat operator - (quat const& other);
	quat operator += (quat const& other);
	quat operator -= (quat const& other);
	quat operator * (quat const& other);
	float operator &(quat const& other);
	quat operator / (qType const& other);
	quat Re();
	quat Im();
}quat;

quat s(quat q);