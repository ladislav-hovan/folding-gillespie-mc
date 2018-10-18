#include "gillespie.h"

inline bool isNearlyEqual(double dX, double dY)
{
	const double dEpsilon = 1e-8;
	return std::abs(dX - dY) <= dEpsilon * std::abs(dX);
}

double getRand()
{
	// PRNG, static so only created once
	static std::random_device Seed;
	static std::mt19937 Mersenne(Seed());
	static std::uniform_real_distribution<double> Rand(0.0, 1.0);

	return Rand(Mersenne);
}

double getDeltaPhi(Domain &Current, double dF, double dT)
{
	auto PhiFunc = [&Current, dT](double dF) { return -(getWLC_Extension(dF, Current.l_f_0, dT) + 
		getFJC_Extension(dF, Current.b * (1.0 - Current.l_f_0 / (1e9 * Current.N * c_dA)), dT) - 
		getWLC_Extension(dF, Current, dT)); };

	return boost::math::quadrature::gauss_kronrod<double, 15>::integrate(PhiFunc, 0.0, dF, 5, 1e-9);
}

vpd precomputeDeltaPhi(Domain &Current, double dT)
{
	vpd Pairs;
	for (int nF = 100; nF >= 1; --nF)
	{
		pair<double, double> Temp(0.01 * nF, getDeltaPhi(Current, 0.01 * nF, dT));
		Pairs.push_back(Temp);
	}

	return Pairs;
}

double getRate(vector<Domain> &vsDomains, int nPos, double dF, double dT)
{
	double kBT = c_dkB * dT;

	if (vsDomains[nPos].folded)
		return vsDomains[nPos].k_u_0 * exp(1e-21 * vsDomains[nPos].Delta * dF / kBT);
	else
	{
		try
		{
			if (isNearlyEqual(dF, 0.0))
				return vsDomains[nPos].k_f_0;
			else
				return vsDomains[nPos].k_f_0 * exp(-1e-21 * getDeltaPhi(vsDomains[nPos], dF, dT) / kBT);
		}
		catch (std::exception e)
		{
			std::cout << dF << "\n";
		}
	}
}

pair<int, double> iterateGillespieDirect(vector<double> &vdCumSums, bool bBinary)
{
	double dFinalSum = vdCumSums[vdCumSums.size() - 1];

	// Check in case no transitions possible
	if (isNearlyEqual(dFinalSum, 0.0))
		return pair<int, double>(-1, 0.0);

	int nWhich = 0;
	// Decide on transition
	if (vdCumSums.size() > 1)
	{
		// Else only one option, so no need to generate random number
		double dProduct = getRand() * dFinalSum;
		if (bBinary)
			// Binary search, logarithmic scaling
			nWhich = std::lower_bound(vdCumSums.begin(), vdCumSums.end(), dProduct) - vdCumSums.begin();
		else
			// Searching in order, linear scaling, efficient for small number of possibilities
			while (vdCumSums[nWhich] < dProduct) ++nWhich;
	}

	// Calculate tau (time increment)
	double dTau = -(1 / dFinalSum) * log(getRand());

	return pair<int, double>(nWhich, dTau);
}

pair<int, double> iterateGillespieFirst(vector<double> &vdRates)
{
	// Calculate tau (time increment) for each possible target, keep smallest
	int nSmallest = -1;
	double dSmallest = std::numeric_limits<double>::infinity();
	for (unsigned int nPos = 0; nPos < vdRates.size(); ++nPos)
	{
		double dRate = vdRates[nPos];
		// A rate of zero can't enter into the next equation, so just ignore
		if (isNearlyEqual(dRate, 0.0))
			continue;
		double dTau = (1 / dRate) * log(1 / getRand());
		if (dTau < dSmallest)
		{
			nSmallest = nPos;
			dSmallest = dTau;
		}
	}
	if (nSmallest == -1)
		return pair<int, double>(-1, 0.0);

	// Go with the smallest tau
	return pair<int, double>(nSmallest, dSmallest);
}

pair<int, double> iterateGillespie(vector<double> &vdRatesOrSums, bool bFirst, bool bBinary)
{
	if (bFirst)
		return iterateGillespieFirst(vdRatesOrSums);
	else
		return iterateGillespieDirect(vdRatesOrSums, bBinary);
}