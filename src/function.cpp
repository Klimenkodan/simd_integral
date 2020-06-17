#include "../include/function.h"
#include <cmath>

double Function::func_to_integrate(const double &x1, const double &x2) {
	double result = 0;
	double added;
	result += 0.002;
	for (int j =-2; j <= 2; j++) {
		for (int i =-2; i <= 2; i++) {
			added = 1 / (5 * (i + 2) + j + 3 + pow((x1 - 16 * j), 6) + pow((x2 - 16 * i), 6));
			result += added;
		}
	}
	result = 1 / result;
	return result;
}

double Function::func(const double &x1, const double &x2) {
	double result = 0;
	for (int j =-2; j <= 2; j++) {
		for (int i =-2; i <= 2; i++) {
			result += x1 * x2;
		}
	}
	return result;

}
