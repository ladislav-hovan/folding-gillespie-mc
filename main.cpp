#include "main.h"

int main()
{
	Timer global;

	// Obtain data about possible transitions and initial conditions
	Input sInput;
	vector<Domain> vsDomains = loadInput("input.dat", sInput);
	if (vsDomains.empty())
	{
		std::cout << "No valid domains were entered" << std::endl;
		exit(0);
	}

	double dT = 296.15;  // 23C

	for (int nRep = 0; nRep < sInput.repetitions; ++nRep)
	{
		// Some initial fixed extension
		double dExt = sInput.initial;

		// Reset domains to folded
		for (auto &Domain : vsDomains)
			Domain.folded = true;

		//vpd Pairs = precomputeDeltaPhi(vsDomains[0], dT);
		//for (auto &pair : Pairs)
		//	std::cout << pair.first << " " << pair.second << "\n";

		// Estimate initial force
		double dF = getTotalForce(dExt, vsDomains, dT);

		// Vectors of reaction rates and their cumulative sums
		vector<double> vdRates;
		for (int nPos = 0; nPos < vsDomains.size(); ++nPos)
			vdRates.push_back(getRate(vsDomains, nPos, dF, dT));
		vector<double> vdCumSums = vdRates;
		if (!sInput.first)
		{
			for (int nPos = 1; nPos < vdCumSums.size(); ++nPos)
				vdCumSums[nPos] += vdCumSums[nPos - 1];
		}
		vector<double> &vdRatesOrSums = (sInput.first ? vdRates : vdCumSums);

		// Output file, but with prefix if repetitions
		std::ofstream Output;
		if (sInput.outfile != "")
		{
			if (sInput.repetitions == 1)
				Output.open(sInput.outfile);
			else
				Output.open("rep" + std::to_string(nRep) + "_" + sInput.outfile);
		}

		// Algorithm loop
		double dTime = 0.0;
		double dRemaining = sInput.timestep;
		bool bChange = true;
		Output << "# Time Ext Force\n";
		Output << dTime << " " << dExt << " " << dF << "\n";
		std::cout << std::endl;

		//while (dExt < sInput.final)
		while (dTime < (sInput.final - sInput.initial) / sInput.rate + sInput.constant)
		{
			bChange = true;
			auto Pair = iterateGillespie(vdRatesOrSums, sInput.first, sInput.binary);
			//std::cout << dExt << " " << Pair.first << " " << Pair.second << std::endl;

			if (Pair.first != -1 && Pair.second < dRemaining)
			{
				dTime += Pair.second;
				dRemaining -= Pair.second;
				vsDomains[Pair.first].folded = !vsDomains[Pair.first].folded;

				Output << dTime << " " << dExt << " " << dF << "\n";
				dF = getTotalForce(dExt, vsDomains, dT);
				Output << "#" << dTime << " " << Pair.first << "\n";
				Output << dTime << " " << dExt << " " << dF << "\n";
				std::cout << dTime << " " << dExt << " " << Pair.first << std::endl;
			}
			else
			{
				dTime += dRemaining;
				dRemaining = sInput.timestep;
				if (dExt < sInput.final)
				{
					dExt += sInput.rate * sInput.timestep;
					dF = getTotalForce(dExt, vsDomains, dT);
				}
				else
					bChange = false;

				Output << dTime << " " << dExt << " " << dF << "\n";
			}

			// Very expensive, so don't recompute unless necessary
			if (bChange)
			{
				for (int nPos = 0; nPos < vsDomains.size(); ++nPos)
					vdRates[nPos] = getRate(vsDomains, nPos, dF, dT);
				vdCumSums = vdRates;
				if (!sInput.first)
				{
					for (int nPos = 1; nPos < vdCumSums.size(); ++nPos)
						vdCumSums[nPos] += vdCumSums[nPos - 1];
				}
				vdRatesOrSums = (sInput.first ? vdRates : vdCumSums);
			}
		}
		if (Output)
			Output.close();
	}

	std::cout << "Total time: " << global.elapsed() << "s" << std::endl;

	return 0;
}