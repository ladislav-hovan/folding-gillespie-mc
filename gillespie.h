#pragma once

#include <unordered_map>
#include <set>
#include <vector>
#include <utility>
#include <random>
#include <cmath>
#include <iostream>
#include <algorithm>

#include <boost/math/quadrature/gauss_kronrod.hpp>
#include <boost/math/quadrature/tanh_sinh.hpp>

#include "domain.h"
#include "extension.h"

using std::vector;
using std::pair;
using vpd = vector< pair<double, double> >;

inline bool isNearlyEqual(double dX, double dY);
double getRand();
double getDeltaPhi(Domain &Current, double dF, double dT);
vpd precomputeDeltaPhi(Domain &Current, double dT);
double getRate(vector<Domain> &vsDomains, int nPos, double dF, double dT);
pair<int, double> iterateGillespieDirect(vector<double> &vdCumSums, bool bBinary);
pair<int, double> iterateGillespieFirst(vector<double> &vdRates);
pair<int, double> iterateGillespie(vector<double> &vdRates, bool bFirst = false,
	bool bBinary = false);