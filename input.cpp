#include "input.h"

void registerDomain(Domain &sDomain, vector<string> &vsWords, Input &sInput)
{
	for (auto &strWord : vsWords)
	{
		if (strWord[0] == 'N')
		{
			string strVal = strWord.substr(2, strWord.size() - 2);
			sDomain.N = std::stoi(strVal);
		}
		else if (strWord[0] == 'b')
		{
			string strVal = strWord.substr(2, strWord.size() - 2);
			sDomain.b = std::stof(strVal);
		}
		else if (!strWord.compare(0, 5, "k_u_0"))
		{
			string strVal = strWord.substr(6, strWord.size() - 6);
			sDomain.k_u_0 = std::stof(strVal);
		}
		else if (!strWord.compare(0, 5, "Delta"))
		{
			string strVal = strWord.substr(6, strWord.size() - 6);
			sDomain.Delta = std::stof(strVal);
		}
		else if (!strWord.compare(0, 5, "l_f_0"))
		{
			string strVal = strWord.substr(6, strWord.size() - 6);
			sDomain.l_f_0 = std::stof(strVal);
		}
		else if (!strWord.compare(0, 5, "k_f_0"))
		{
			string strVal = strWord.substr(6, strWord.size() - 6);
			sDomain.k_f_0 = std::stof(strVal);
		}
		else if (strWord == "DOMAIN")
			continue;
		else
			std::cout << "Ignoring unrecognised sequence in domain definition: " << strWord << std::endl;
	}
}

Domain analyseLine(string strLine, Input &sInput)
{
	Domain sDomain;

	// Remove comments
	for (unsigned int nChar = 0; nChar < strLine.size(); ++nChar)
	{
		if (strLine[nChar] == '#')
		{
			strLine = strLine.substr(0, nChar);
			break;
		}
	}

	// Split into words
	std::stringstream SStream;
	vector<string> vsWords;
	SStream << strLine;
	while (!SStream.eof())
	{
		string strWord;
		SStream >> strWord;
		// Only non-empty words
		if (!std::all_of(strWord.begin(), strWord.end(), 
			[](const auto &v) { return std::isspace(v); }))  // Workaround for g++ compatibility
			vsWords.push_back(strWord);
	}

	if (vsWords.empty())
		// Default Domain will have N = 0 and won't be included
		return sDomain;

	// Handle keywords
	if (vsWords[0] == "INITIAL")
	{
		sInput.initial = std::stof(vsWords[1]);
		std::cout << "Initial extension is " << vsWords[1] << "nm" << std::endl;
	}
	else if (vsWords[0] == "DIRECT")
	{
		sInput.first = false;
		std::cout << "Using direct version of the algorithm" << std::endl;
	}
	else if (vsWords[0] == "FIRST")
	{
		sInput.first = true;
		std::cout << "Using first reaction version of the algorithm" << std::endl;
	}
	else if (vsWords[0] == "BINARY")
	{
		sInput.binary = true;
		std::cout << "Using binary search when deciding on a reaction" << std::endl;
	}
	else if (vsWords[0] == "RATE")
	{
		sInput.rate = std::stof(vsWords[1]);
		std::cout << "Rate of extension is " << vsWords[1] << "nm/s" << std::endl;
	}
	else if (vsWords[0] == "FINAL")
	{
		sInput.final = std::stof(vsWords[1]);
		std::cout << "Final extension is " << vsWords[1] << "nm" << std::endl;
	}
	else if (vsWords[0] == "STEP")
	{
		sInput.timestep = std::stof(vsWords[1]);
		std::cout << "Maximal timestep is " << vsWords[1] << "s" << std::endl;
	}
	else if (vsWords[0] == "OUTFILE")
	{
		sInput.outfile = vsWords[1];
		std::cout << "Saving data to file \"" << vsWords[1] << "\"" << std::endl;
	}
	else if (vsWords[0] == "REPEAT")
	{
		sInput.repetitions = std::stoi(vsWords[1]);
		std::cout << "Doing " << vsWords[1] << " repetitions" << std::endl;
	}
	else if (vsWords[0] == "CONSTANT")
	{
		sInput.constant = std::stof(vsWords[1]);
		std::cout << "Constant extension time is " << vsWords[1] << " seconds" << std::endl;
	}
	else if (vsWords[0] == "DOMAIN")
		registerDomain(sDomain, vsWords, sInput);
	else
		std::cout << "Ignoring unrecognised sequence: " << strLine << std::endl;

	return sDomain;
}

vector<Domain> loadInput(string strFilename, Input &sInput)
{
	vector<Domain> vsDomains;

	std::ifstream InputFile(strFilename);
	string strLine;
	while (std::getline(InputFile, strLine))
	{
		Domain sDomain = analyseLine(strLine, sInput);
		if (sDomain.N > 0)
			vsDomains.push_back(sDomain);
	}

	return vsDomains;
}