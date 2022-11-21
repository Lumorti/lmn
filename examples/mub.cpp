#include "../poly.h"

// Standard cpp entry point 
int main(int argc, char ** argv) {

	// Find a lower bound TODO
	//PolynomialProblem<double> testProb(Polynomial<double>(4), {
			//Polynomial<double>(4, "1*{00}+1*{11}-1*{}"),
			//Polynomial<double>(4, "1*{22}+1*{33}-1*{}"),
			//Polynomial<double>(4, "1*{01}-1*{23}-0.9*{}"),
			//Polynomial<double>(4, "1*{03}-1*{12}-0.1*{}"),
		//}, {
		//});
	//std::cout << testProb << std::endl;

	//Polynomial<double> testPoly(testProb.obj.maxVariables);
	//for (int i=0; i<testProb.conZero.size(); i++) {
		//testPoly += testProb.conZero[i]*testProb.conZero[i];
	//}
	//testPoly = testPoly.changeMaxVariables(testPoly.maxVariables+1);
	//auto solVec = testPoly.findRoot(4, 0.5, 1e-10, 10000, 16, false);
	//std::cout << solVec << std::endl;;

	//testProb.proveInfeasible();
	
	//return 0;

	// Get the problem from the args
	int d = 2;
	int n = 2;
	if (argc > 1) {
		d = std::stoi(argv[1]);
	}
	if (argc > 2) {
		n = std::stoi(argv[2]);
	}

	// Useful quantities
	int numVarsNonConj = n*d*d;
	int numVars = 2*numVarsNonConj+100;
	int conjDelta = numVarsNonConj;
	double rt2 = 1.0/std::sqrt(2.0);

	// Different "bases"
	std::vector<std::vector<int>> dLimits;
	if (d == 6 && n == 4) {
		dLimits.push_back({6, 3, 3, 3});
	} else if (d == 5 && n == 4) {
		dLimits.push_back({5, 3, 3, 3});
	} else if (d == 7 && n == 4) {
		dLimits.push_back({7, 3, 3, 3});
	} else if (d == 10 && n == 4) {
		dLimits.push_back({10, 3, 3, 3});
	} else if (d == 2 && n == 3) {
		dLimits.push_back({2, 1, 1});
	} else if (d == 2 && n == 4) {
		dLimits.push_back({2, 1, 1, 1});
	} else {
		dLimits.push_back(std::vector<int>(n, d));
	}

	// For each different restriction
	for (int i2=0; i2<dLimits.size(); i2++) {

		// The list of equations to fill
		std::vector<Polynomial<double>> eqns;

		// Generate equations
		int newVarInd = 2*numVarsNonConj;
		for (int i=1; i<n; i++) {
			for (int j=i; j<n; j++) {
				for (int k=0; k<dLimits[i2][i]; k++) {
					for (int l=0; l<dLimits[i2][j]; l++) {

						// Prevent repeat equations
						if (i == j && l < k) {
							continue;
						}

						// (a+ib)*(c+id)
						Polynomial<std::complex<double>> eqn(numVars);
						for (int m=0; m<d; m++) {
							int var1 = i*d*d + k*d + m;
							int var2 = j*d*d + l*d + m;
							int var3 = var1 + conjDelta;
							int var4 = var2 + conjDelta;
							eqn.addTerm(1, {var1, var2});
							eqn.addTerm(1, {var3, var4});
							eqn.addTerm(1i, {var1, var4});
							eqn.addTerm(-1i, {var2, var3});
						}

						// For the normalisation equations
						if (i == j && k == l) {
							eqn.addTerm(-1, {});
							continue;
						}

						// For the MUB-ness equations
						if (i != j) {

							// Constrain that this should equal a new complex number
							eqn.addTerm(-1, {newVarInd});
							eqn.addTerm(-1i, {newVarInd+1});

							// Constrain that this new complex should have mag 1/d
							Polynomial<double> extraEqn(numVars);
							extraEqn.addTerm(1, {newVarInd,newVarInd});
							extraEqn.addTerm(1, {newVarInd+1,newVarInd+1});
							extraEqn.addTerm(-1.0/d, {});
							eqns.push_back(extraEqn);
							newVarInd += 2;

						}

						// Split into real and imag parts (both should be 0)
						Polynomial<double> eqnReal = eqn.real();
						Polynomial<double> eqnImag = eqn.imag();

						// Add these equations if they're not empty
						if (eqnReal.size() > 0) {
							eqns.push_back(eqnReal);
						}
						if (eqnImag.size() > 0) {
							eqns.push_back(eqnImag);
						}

					}
				}
			}
		}

		// All should have mag 1
		int ogEqns = eqns.size();
		for (int i=d*d; i<numVarsNonConj; i++) {
			for (int j=0; j<ogEqns; j++) {
				if (eqns[j].contains(i)) {
					Polynomial<double> extraEqn(numVars);
					extraEqn.addTerm(1, {i,i});
					extraEqn.addTerm(1, {i+conjDelta,i+conjDelta});
					extraEqn.addTerm(-1.0/d, {});
					eqns.push_back(extraEqn);
					break;
				}
			}
		}

		//std::cout << "before reductions:" << std::endl;
		//for (int i=0; i<eqns.size(); i++) {
			//std::cout << eqns[i] << std::endl;
		//}

		// Can assume the first basis is the computational TODO
		std::vector<int> indsToReplace;
		std::vector<double> valsToReplace;
		for (int i=0; i<d; i++) {
			for (int j=0; j<d; j++) {
				indsToReplace.push_back(i*d+j);
				indsToReplace.push_back(i*d+j+conjDelta);
				if (i == j) {
					valsToReplace.push_back(1);
				} else {
					valsToReplace.push_back(0);
				}
				valsToReplace.push_back(0);
			}
		}

		// Can assume first element of second basis is uniform
		for (int j=0; j<d; j++) {
			indsToReplace.push_back(d*d+j);
			indsToReplace.push_back(d*d+j+conjDelta);
			valsToReplace.push_back(1/std::sqrt(d));
			valsToReplace.push_back(0);
		}

		// Can assume first value of each vector is 1 / sqrt(d)
		//for (int i=d; i<d+1; i++) {
			//indsToReplace.push_back(i*d);
			//indsToReplace.push_back(i*d+conjDelta);
			//valsToReplace.push_back(1/std::sqrt(d));
			//valsToReplace.push_back(0);
		//}

		//for (int i=d; i<2*d; i++) {
			//for (int j=0; j<d; j++) {
				//indsToReplace.push_back(i*d+j);
				//indsToReplace.push_back(i*d+j+conjDelta);
				//if (i == 2*d-1 && j == 1) {
					//valsToReplace.push_back(-1.0/std::sqrt(2));
				//} else {
					//valsToReplace.push_back(1.0/std::sqrt(2));
				//}
				//valsToReplace.push_back(0);
			//}
		//}
		//double overRt2 = 1.0/std::sqrt(2);
		//indsToReplace.push_back(33);
		//valsToReplace.push_back(0);
		//indsToReplace.push_back(32);
		//valsToReplace.push_back(std::sqrt(2));
		//indsToReplace = {0, 1,    2, 3,    4, 5,     6, 7,   8,  
						 //16, 17,  18, 19,  20, 21,    22, 23,  24};
		//valsToReplace = {1, 0,    0, 1,    overRt2, overRt2,     overRt2, -overRt2,    0,
						 //0, 0,    0, 0,    0, 0,                 0, 0};
		//std::cout << indsToReplace << std::endl;
		//std::cout << valsToReplace << std::endl;
		for (int i=0; i<eqns.size(); i++) {
			eqns[i] = eqns[i].replaceWithValue(indsToReplace, valsToReplace);
		}

		// Collapse to the minimum number of vars
		std::unordered_map<int,int> indMap;
		int nextInd = 0;
		for (int i=0; i<numVars; i++) {
			for (int j=0; j<eqns.size(); j++) {
				if (eqns[j].contains(i)) {
					indMap[i] = nextInd;
					nextInd++;
					break;
				}
			}
		}
		numVars = nextInd;
		std::vector<Polynomial<double>> newEqns;
		for (int i=0; i<eqns.size(); i++) {
			eqns[i] = eqns[i].replaceWithVariable(indMap).changeMaxVariables(numVars);
			if (eqns[i].size() > 0) {
				newEqns.push_back(eqns[i]);
			}
		}

		//auto newnewInds = newInds;
		//newnewInds[12] = 0;
		//newnewInds[13] = 6;
		//newnewInds[14] = 2;
		//newnewInds[15] = 8;
		//newnewInds[16] = 4;
		//newnewInds[17] = 10;
		//for (int i=0; i<newEqns.size(); i++) {
			//newEqns[i] = newEqns[i].replaceWithVariable(newInds, newnewInds);
		//}

		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 0}-0.70710678118*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 4}-0*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 1}-0*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 5}-0.70710678118*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 2}-0.70710678118*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 6}-0*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 3}-0*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 7}-0.70710678118*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 8}-0.70710678118*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 9}-0*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 0 0}-0.5*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 1 1}-0*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 2 2}-0.5*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 3 3}-0*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 4 4}-0*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 5 5}-0.5*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 6 6}-0*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 7 7}-0.5*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 8 8}-0.5*{}"));
		//newEqns.push_back(Polynomial<double>(numVars, "1*{ 9 9}-0*{}"));
		
		// Combine these to create a single polynomial
		//std::cout << "reduced equations:" << std::endl;
		Polynomial<double> poly(numVars);
		for (int i=0; i<newEqns.size(); i++) {
			std::cout << newEqns[i] << std::endl;
			poly += newEqns[i]*newEqns[i];
		}

		// Minimize number of equations needed TODO
		std::cout << dLimits[i2] << "    " << newEqns.size() << "   " << numVars << std::endl;

		// Find a lower bound TODO
		Polynomial<double> obj(numVars);
		PolynomialProblem<double> prob(obj, newEqns, {});
		prob.proveInfeasible();

		// Find a root of this using an auxilary variable
		//std::vector<double> x = poly.findRoot(0, 0.5, 1e-10, 10000, 16, false);
		//std::vector<double> x = poly.findRoot(numVars-1, 0.1, 1e-10, 10000, 16, false);
		//std::cout << "   Testing this x = " << poly.eval(x) << std::endl;

	}

	return 0;

}
	
