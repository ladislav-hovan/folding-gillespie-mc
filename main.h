#include <iostream>
#include <fstream>
#include <chrono>

#include "input.h"
#include "gillespie.h"
#include "extension.h"

using std::vector;
using std::pair;

class Timer
{
private:
	std::chrono::time_point <std::chrono::high_resolution_clock> start;

public:
	Timer() : start(std::chrono::high_resolution_clock::now()) {}

	void reset() { start = std::chrono::high_resolution_clock::now(); }

	double elapsed()
	{
		return (std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1> >>
			(std::chrono::high_resolution_clock::now() - start)).count();
	}
};