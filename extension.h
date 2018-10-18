#pragma once

#include <cmath>
#include <vector>
#include <iostream>
#include <utility>

#include <boost/math/tools/roots.hpp>

#include "domain.h"

using namespace boost::math::tools;

using std::vector;
using std::pair;
using pvd = pair< vector<double>, vector<double> >;

const double c_dkB = 1.38064852e-23;
const double c_dA = 0.8e-9;
const double c_dl_0 = 0.38;
const double c_dMinForce = 1e-14;
const double c_dMaxForce = 200.0;
const double c_dMinExt = 1e-14;
const double c_dMaxExtCoeff = 0.999999;

double getFJC_Extension(double dF, double db, double dT);
double getFJC_Extension(double dF, Domain &Current, double dT);
double getWLC_Force(double dExt, double dl, double dT);
double getWLC_Force(double dExt, Domain &Current, double dT);
double getWLC_Extension(double dF, double dl, double dT);
double getWLC_Extension(double dF, Domain &Current, double dT);
double getTotalExtension(double dF, vector<Domain> &vsDomains, double dT);
double getTotalForce(double dExt, vector<Domain> &vsDomains, double dT);