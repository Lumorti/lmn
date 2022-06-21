#ifndef _poly
#define _poly

#include <iostream>
#include <vector>
#include <complex>
#include <random>
#include <chrono>
#include <math.h>
#include <unordered_map>

// Use Eigen for matrix/vector ops
#include <Eigen/Dense>

// Class allowing manipulation of polynomials
class Polynomial {
public:

	// General poly properties
	int numVars = 1;
	int digitsPerInd = 1;
	std::unordered_map<std::string, std::complex<double>> coeffs;

	// Used for fast eval
	bool fastEvalReady = false;
	std::vector<std::vector<int>> inds;
	std::vector<double> vals;

	// Default constructor
	Polynomial(int numVars_) {
		numVars = numVars_;
		digitsPerInd = std::ceil(std::log10(numVars));
	}

	// Add a term given a coefficient and an index list
	void addTerm(std::complex<double> coeff, std::vector<int> list) {

		// Convert the vector to a string
		std::string asString = "";
		for (int i=0; i<list.size(); i++) {
			std::string padded = std::to_string(list[i]);
			padded.insert(0, digitsPerInd-padded.size(), ' ');
			asString += padded;
		}

		// Add it, combining it if the term already exists
		if (coeffs.find(asString) != coeffs.end()) {
			coeffs[asString] += coeff;
			if (std::abs(coeffs[asString]) < 1e-10) {
				coeffs.erase(asString);
			}
		} else {
			coeffs[asString] = coeff;
		}

	}

	// Substitute a variable for a value TODO
	Polynomial substitute(int ind, std::complex<double> toReplace) {

		// Cache the ind to replace as a string
		std::string indString = std::to_string(ind);
		indString.insert(0, digitsPerInd-indString.size(), ' ');

		// For each element in this polynomial
		Polynomial newPoly(numVars);
		for (auto const &pair: coeffs) {

			// Remove any instances of this index
			std::string newKey = "";
			std::complex<double> newVal = pair.second;
			for (int i=0; i<pair.first.size(); i+=digitsPerInd) {
				if (pair.first.substr(i, digitsPerInd) == indString) {
					newVal *= toReplace;
				} else {
					newKey += pair.first.substr(i,digitsPerInd); 
				}
			}

			// Then add or create this new term
			if (newPoly.coeffs.find(newKey) != newPoly.coeffs.end()) {
				newPoly.coeffs[newKey] += newVal;
				if (std::abs(newPoly.coeffs[newKey]) < 1e-10) {
					newPoly.coeffs.erase(newKey);
				}
			} else {
				newPoly.coeffs[newKey] = newVal;
			}

		}

		return newPoly;

	}	

	// Substitute several variables for several values 
	Polynomial substitute(std::vector<int> ind, std::vector<std::complex<double>> toReplace) {
		Polynomial newPoly = substitute(ind[0], toReplace[0]);
		for (int i=1; i<ind.size(); i++) {
			newPoly = newPoly.substitute(ind[i], toReplace[i]);
		}
		return newPoly.prune();
	}

	// Remove zeros from a polynomial
	Polynomial prune() {
		Polynomial newPoly(numVars);
		for (auto const &pair: coeffs) {
			if (std::abs(pair.second) > 1e-8) {
				newPoly.coeffs[pair.first] = pair.second;
			}
		}
		return newPoly;
	}

	// Differentiate a polynomial with respect to a variable index
	Polynomial differentiate(int ind) {

		// Convert to a string and cache
		std::string indString = std::to_string(ind);
		indString.insert(0, digitsPerInd-indString.size(), ' ');

		// Process each term
		Polynomial result(numVars);
		for (auto const &pair: coeffs) {

			// Count the number of times the variable appears here
			int indFound = -1;
			double degree = 0.0;
			for (int i=0; i<pair.first.size(); i+=digitsPerInd) {
				if (pair.first.substr(i,digitsPerInd) == indString) {
					degree += 1.0;
					indFound = i;
				}
			}

			// Only do something if there is something with this ind
			if (degree > 0.0) {

				// Remove the latest term of this ind
				std::string newInd = pair.first;
				newInd.erase(newInd.begin()+indFound, newInd.begin()+indFound+digitsPerInd);

				// Add to the result
				if (result.coeffs.find(newInd) != result.coeffs.end()) {
					result.coeffs[newInd] += pair.second * degree;
				} else {
					result.coeffs[newInd] = pair.second * degree;
				}

			}

		}

		// Remove any zero terms and return
		return result.prune();

	}

	// Integrate a polynomial with respect to a variable index
	Polynomial integrate(int ind) {

		// Convert to a string and cache
		std::string indString = std::to_string(ind);
		indString.insert(0, digitsPerInd-indString.size(), ' ');

		// Process each term
		Polynomial result(numVars);
		for (auto const &pair: coeffs) {

			// Count the number of times the variable appears here
			double degree = 0.0;
			for (int i=0; i<pair.first.size(); i+=digitsPerInd) {
				if (pair.first.substr(i,digitsPerInd) == indString) {
					degree += 1.0;
				}
			}

			// Add an extra term at the right place
			std::string newInd = pair.first;
			bool hasAdded = false;
			for (int i=0; i<newInd.size(); i+=digitsPerInd) {
				if (std::stoi(newInd.substr(i,digitsPerInd)) > ind) {
					newInd.insert(i, indString);
					hasAdded = true;
					break;
				}
			}

			// If the index is larger than everything
			if (!hasAdded) {
				newInd += indString;
			}

			// Add to the result
			if (result.coeffs.find(newInd) != result.coeffs.end()) {
				result.coeffs[newInd] += pair.second / (degree+1);
			} else {
				result.coeffs[newInd] = pair.second / (degree+1);
			}

		}

		// Remove any zero terms and return
		return result.prune();

	}

	// Overload the addition operator
	Polynomial operator+(const Polynomial& other) {

		// Start with one equation
		Polynomial result = other;

		// For each term of the other
		for (auto const &pair: coeffs) {

			// If it's new add it, otherwise combine with the existing
			if (result.coeffs.find(pair.first) != result.coeffs.end()) {
				result.coeffs[pair.first] += pair.second;
				if (std::abs(result.coeffs[pair.first]) < 1e-10) {
					result.coeffs.erase(pair.first);
				}
			} else {
				result.coeffs[pair.first] = pair.second;
			}

		}

		// Remove any zeros and return
		return result;

	}

	// Overload for in-place addition (+=)
	Polynomial& operator+=(const Polynomial& other){

		// For each term of the other
		for (auto const &pair: other.coeffs) {

			// If it's new add it, otherwise combine with the existing
			if (coeffs.find(pair.first) != coeffs.end()) {
				coeffs[pair.first] += pair.second;
				if (std::abs(coeffs[pair.first]) < 1e-10) {
					coeffs.erase(pair.first);
				}
			} else {
				coeffs[pair.first] = pair.second;
			}

		}

		return *this;
	}

	// Overload the multiplication operator
	Polynomial operator*(const Polynomial& other) {

		// For each term of both equations
		Polynomial result(numVars);
		for (auto const &pair1: coeffs) {
			for (auto const &pair2: other.coeffs) {

				// Combine the term list
				std::string combined = "";
				int ind1 = 0;
				int ind2 = 0;
				while (ind1 < pair1.first.size() && ind2 < pair2.first.size()) {
					if (std::stoi(pair1.first.substr(ind1,digitsPerInd)) < std::stoi(pair2.first.substr(ind2,digitsPerInd))) {
						combined += pair1.first.substr(ind1,digitsPerInd);
						ind1 += digitsPerInd;
					} else {
						combined += pair2.first.substr(ind2,digitsPerInd);
						ind2 += digitsPerInd;
					}
				}
				combined += pair1.first.substr(ind1) + pair2.first.substr(ind2);

				// If it's new add it, otherwise combine with the existing
				if (result.coeffs.find(combined) != result.coeffs.end()) {
					result.coeffs[combined] += pair1.second*pair2.second;
				} else {
					result.coeffs[combined] = pair1.second*pair2.second;
				}

			}
		}

		// Remove any zeros and return
		return result.prune();

	}

	// Get the conjugate of the polynomial
	Polynomial conjugate() {
		Polynomial con(numVars);
		for (auto const &pair: coeffs) {
			con.coeffs[pair.first] = std::conj(pair.second);
		}
		return con;
	}

	// Evaluate a polynomial with x values
	template <typename type>
	std::complex<double> eval(type x) {

		// For each term being added
		std::complex<double> soFar = 0;
		for (auto const &pair: coeffs) {

			// Multiply all the values
			std::complex<double> sub = 1;
			for (int j=0; j<pair.first.size(); j+=digitsPerInd) {
				sub *= x[std::stoi(pair.first.substr(j, digitsPerInd))];
			}

			// Add to the total
			soFar += sub*pair.second;

		}

		return soFar;

	}

	// Prepares this equation for rapid eval by caching things
	void prepareEvalFast() {

		// For each coefficient
		for (auto const &pair: coeffs) {

			// Get the indices as a std::vector
			std::vector<int> indVec;
			for (int j=0; j<pair.first.size(); j+=digitsPerInd) {
				indVec.push_back(std::stoi(pair.first.substr(j, digitsPerInd)));
			}
			inds.push_back(indVec);

			// Convert the coefficient to real
			vals.push_back(std::real(pair.second));

		}

		// Free some space
		coeffs.clear();

		// Flag that ready for fast eval now
		fastEvalReady = true;

	}

	// Evaluate a polynomial with x values
	template <typename type>
	double evalFast(type x) {

		// For each term being added
		double soFar = 0;
		for (int i=0; i<vals.size(); i++) {

			// Multiply all the values
			double sub = 1;
			for (int j=0; j<inds[i].size(); j++) {
				sub *= x[inds[i][j]];
			}

			// Add to the total
			soFar += sub*vals[i];

		}

		return soFar;

	}

	// When doing std::cout << Polynomial
	friend std::ostream &operator<<(std::ostream &output, const Polynomial &other) {

		// For each term
		int numSoFar = 0;
		for (auto const &pair: other.coeffs) {

			// First the coeff
			output << pair.second << "*{";

			// Then the indices
			output << pair.first << "}";

			// Output an addition on everything but the last
			numSoFar += 1;
			if (numSoFar < other.coeffs.size()) {
				output << " + ";
			}

		}

		return output;

	}

	// Checking equality between two polynomials
	bool operator==(const Polynomial &other) {
		for (auto const &pair: other.coeffs) {
			if (coeffs[pair.first] != pair.second) {
				return false;
			}
		}
		return true;
	}

	// Checking inequality between two polynomials
	bool operator!=(const Polynomial &other) {
		for (auto const &pair: other.coeffs) {
			if (coeffs[pair.first] == pair.second) {
				return false;
			}
		}
		return true;
	}

	// Use a gradient-descent method to find a local minimum
	std::vector<double> findLocalMinimum(double alpha=0.1, double tolerance=1e-10, int maxIters=10000000) {

		// Get the gradient TODO openmp
		std::cout << "Differentiating..." << std::endl;
		std::vector<Polynomial> gradient(numVars, Polynomial(numVars));
		for (int i=0; i<numVars; i++) {
			gradient[i] = differentiate(i);
		}

		// Get the Hessian TODO openmp
		std::vector<std::vector<Polynomial>> hessian(numVars, std::vector<Polynomial>(numVars, Polynomial(numVars)));
		for (int i=0; i<numVars; i++) {
			for (int j=i; j<numVars; j++) {
				hessian[i][j] = gradient[i].differentiate(j);
			}
		}

		// Pre-cache things to allow for much faster evals
		std::cout << "Optimizing polynomials for fast eval..." << std::endl;
		prepareEvalFast();
		for (int i=0; i<numVars; i++) {
			gradient[i].prepareEvalFast();
			for (int j=i; j<numVars; j++) {
				hessian[i][j].prepareEvalFast();
			}
		}

		// Random starting x
		//std::srand(unsigned(std::time(nullptr)));
		std::srand(0);
		Eigen::VectorXd x = Eigen::VectorXd::Random(numVars);

		// Perform gradient descent using this info
		std::cout << "Minimizing..." << std::endl;
		Eigen::MatrixXd inv(numVars, numVars);
		Eigen::VectorXd p(numVars);
		Eigen::MatrixXd H(numVars, numVars);
		Eigen::VectorXd g(numVars);
		double maxX = 0;
		int iter = 0;
		double norm = 1;
		for (iter=0; iter<maxIters; iter++) {

			// Calculate the gradient TODO openmp
			for (int i=0; i<numVars; i++) {
				g(i) = gradient[i].evalFast(x);
			}

			// Calculate the Hessian TODO openmp
			for (int i=0; i<numVars; i++) {
				for (int j=i; j<numVars; j++) {
					H(i,j) = hessian[i][j].evalFast(x);
					H(j,i) = H(i,j);
				}
			}

			// Determine the direction
			//p = H.inverse()*g;
			//p = H.partialPivLu().solve(g);
			p = H.householderQr().solve(g);

			for (int i=0; i<numVars; i++) {
				if (isnan(p(i))) {
					p(i) = 0;
					x(i) = 0;
				}
			}

			//std::cout << p.transpose() << std::endl;
			//return std::vector<double>(1);

			//alpha = 0.1;
			//for (int i=0; i<100; i++) {
				//if (gradient[0].evalFast(x-alpha*p) - g(0) < alpha) {
					//break;
				//}
				//alpha *= 0.9;
			//}
			
			// Perform a line search
			//alpha = 0.1;
			//double c = 0.0;
			//double tau = 0.9;
			//double m = g.dot(p);
			//double t = -c*m;
			//for (int i=0; i<10; i++) {

				//// Keep going until the Wolfe conditions hold
				//if (std::real(eval(this, x)) - std::real(eval(this, x+alpha*p)) >= t*alpha) {
					//break;
				//}

				//// Reduce alpha
				//alpha *= tau;

			//}

			// Perform the update
			x -= alpha*p;

			// Per-iteration output
			norm = std::abs(g.norm());
			std::cout << iter << " " << norm << " " << alpha << "          \r" << std::flush;

			// Convergence criteria
			if (norm < tolerance) {
				break;
			}

		}

		// Final output
		std::cout << "Finished in " << iter << " iterations" << std::endl;
		std::cout << "Final x = ";
		std::cout << "{ ";
		for (int i=0; i<numVars; i++) {
			std::cout << x(i);
			if (i < x.size()-1) {
				std::cout << ", ";
			}
		}
		std::cout << "}" << std::endl;
		std::cout << "Final gradient = " << g(0) << std::endl;

		// Convert the eigen vec into a normal vec
		std::vector<double> toReturn(numVars);
		for (int i=0; i<numVars; i++) {
			toReturn[i] = x(i);
		}
		return toReturn;

	}

};

#endif
	
