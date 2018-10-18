#include "extension.h"

double getFJC_Extension(double dF, double db, double dT)
{
	// Expect force in pN, b in nm
	// Return extension in nm
	double kBT = c_dkB * dT;
	double dProduct = 1e-21 * dF * db;

	return db * (1 / tanh(dProduct / kBT) - kBT / dProduct);
}

double getFJC_Extension(double dF, Domain &Current, double dT)
{
	// Expect force in pN, b in nm
	// Return extension in nm
	double kBT = c_dkB * dT;
	double dProduct = 1e-21 * dF * Current.b;

	return Current.b * (1 / tanh(dProduct / kBT) - kBT / dProduct);
}

double getWLC_Force(double dExt, double dl, double dT)
{
	// Expect extension in nm
	// Return force in pN
	double kBT = c_dkB * dT;
	double dRatio = dExt / dl;

	return (1e12 * kBT / c_dA) * (1 / pow(2 * (1 - dRatio), 2) - 1.0 / 4 + dRatio);
}

double getWLC_Force(double dExt, Domain &Current, double dT)
{
	// Expect extension in nm
	// Return force in pN
	double kBT = c_dkB * dT;
	double dl = Current.N * c_dl_0;
	double dRatio = dExt / dl;

	return (1e12 * kBT / c_dA) * (1 / pow(2 * (1 - dRatio), 2) - 1.0 / 4 + dRatio);
}

double getWLC_Extension(double dF, double dl, double dT)
{
	// Expect force in pN
	// Return extension in nm
	auto ForceFunc = [dl, dT, &dF](double dExt) { return getWLC_Force(dExt, dl, dT) - dF; };

	eps_tolerance<double> tol(10);
	boost::uintmax_t maxtol = 1000;

	std::pair<double, double> Result;
	Result = toms748_solve(ForceFunc, c_dMinExt, c_dMaxExtCoeff * dl, tol, maxtol);

	return (Result.first + Result.second) / 2;
}

double getWLC_Extension(double dF, Domain &Current, double dT)
{
	// Expect force in pN
	// Return extension in nm
	auto ForceFunc = [&Current, dT, &dF](double dExt) { return getWLC_Force(dExt, Current, dT) - dF; };

	eps_tolerance<double> tol(10);
	boost::uintmax_t maxtol = 1000;

	std::pair<double, double> Result;
	Result = toms748_solve(ForceFunc, c_dMinExt, c_dMaxExtCoeff * Current.N * c_dl_0, tol, maxtol);

	return (Result.first + Result.second) / 2;
}

double getTotalExtension(double dF, vector<Domain> &vsDomains, double dT)
{
	double dTotal = 0.0;
	for (Domain &current : vsDomains)
	{
		if (current.folded)
			dTotal += getFJC_Extension(dF, current, dT);
		else
			dTotal += getWLC_Extension(dF, current, dT);
	}

	return dTotal;
}

double getTotalForce(double dExt, vector<Domain> &vsDomains, double dT)
{
	auto ExtFunc = [&vsDomains, dT, &dExt](double dF) { return getTotalExtension(dF, vsDomains, dT) - dExt; };

	//std::cout << ExtFunc(c_dMinForce) << " " << ExtFunc(c_dMaxForce) << std::endl;

	eps_tolerance<double> tol(10);
	boost::uintmax_t maxtol = 1000;

	std::pair<double, double> Result;
	Result = toms748_solve(ExtFunc, c_dMinForce, c_dMaxForce, tol, maxtol);
	
	return (Result.first + Result.second) / 2;
}
