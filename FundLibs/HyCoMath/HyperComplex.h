#pragma once
#define _i puco(1, 0, 0)
#define _j puco(0, 1, 0)
#define _k puco(0, 0, 1)

#define qType float

typedef qType reco;

struct hyco;

//pure complex
typedef struct puco {
	qType i, j, k;
	puco(qType i = 0, qType j = 0, qType k = 0);
	puco(hyco hc);

	puco operator ! ();
	puco operator - ();
	puco operator + (puco const& other);
	puco operator - (puco const& other);
	puco operator += (puco const& other);
	puco operator -= (puco const& other);
	puco operator &&(puco const& other);
	puco operator * (qType const& other);
	puco operator / (qType const& other);
	hyco operator * (puco const& other);
} puco;

//hypercomplex nums
typedef struct hyco{
	reco re;
	puco im;
	hyco(qType re, qType i, qType j, qType k);
	hyco(qType i, qType j, qType k);
	hyco(qType re, puco im);
	hyco(puco im);
	hyco(qType re = 0);
	hyco operator ! ();
	hyco operator - ();
	hyco operator + (hyco const& other);
	hyco operator - (hyco const& other);
	hyco operator += (hyco const& other);
	hyco operator -= (hyco const& other);
	hyco operator &&(hyco const& other);
	hyco operator * (qType const& other);
	hyco operator / (qType const& other);
	hyco operator * (hyco const& other);
} hyco;

hyco s(hyco hc);