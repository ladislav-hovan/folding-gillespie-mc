#pragma once

#include <fstream>
#include <sstream>
#include <cctype>
#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include <algorithm>

#include "domain.h"

using std::vector;
using std::pair;
using std::string;

struct Input
{
	bool first = false;
	bool binary = false;
	double initial = 0.01;
	double final = 100.0;
	double rate = 1.0;
	double timestep = 0.05;
	double constant = 0.0;
	string outfile = "";
	int repetitions = 1;
};

void registerDomain(Domain &sDomain, vector<string> &vsWords, Input &sInput);
Domain analyseLine(string strLine, Input &sInput);
vector<Domain> loadInput(string strFilename, Input &sInput);