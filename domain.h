#pragma once

struct Domain
{
	// Contains domain status and relevant information
	bool folded = true;
	int N = 0;             // Number of residues
	double b = 0.0;        // Folded length (FJC)
	double k_u_0 = -1.0;   // Unfolding rate at zero force (Bells)
	double Delta = 0.0;    // Transition distance (Bells)
	double l_f_0 = 0.0;    // Contour length of the TS peptide chain fraction (Arrhenius)
	double k_f_0 = -1.0;   // Refolding rate at zero force (Arrhenius)
};