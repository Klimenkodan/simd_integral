#include <iostream>
#include <vector>
#include "config_parser.h"
#include "mutex"

#ifndef SAMPLEINT_INTEGRATION_H
#define SAMPLEINT_INTEGRATION_H

double integrate(double x1, const double &x2, double y1, const double &y2, size_t steps);
void integrate_with_threads(int idx, configuration_t config, size_t steps, std::vector<double>& results, std::mutex& mx);
double integrate_simd(double x1, double x2, double y1, double y2, const configuration_t& config, size_t steps_x, size_t steps_y);
#endif //SAMPLEINT_INTEGRATION_H
