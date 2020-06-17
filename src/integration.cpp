#include "../include/integration.h"
#include "../include/function.h"
#include <vector>
#include "../include/config_parser.h"
#include <cmath>
#include <emmintrin.h>
#include <immintrin.h>
#include <avx2intrin.h>

void integrate_with_threads(int idx, configuration_t config, size_t steps, std::vector<double> &results, std::mutex& mx) {
	double res = 0;
	auto y = config.y1;
	double delta_x = (config.x2 - config.x1) / sqrt(steps);
	double delta_y = (config.y2 - config.y1) / sqrt(steps);
	double size_div = (config.x2 - config.x1) / config.num_threads;
	double starting_x = config.x1 + (size_div * idx);
	double area = delta_x * delta_y;
	double ch_x1 = starting_x;
	double end_x = starting_x + size_div;

	while (y < config.y2) {

		while (ch_x1 < end_x) {
//			res += Function::func_to_integrate(ch_x1, y) * area;
			res += Function::func(ch_x1, y) * area;
			ch_x1 += delta_x;
		}
		y += delta_y;
		ch_x1 = starting_x;
	}
	mx.lock();
	results.push_back(res);
	mx.unlock();
}

double integrate(double x1, const double &x2, double y1, const double &y2, size_t steps) {
//	double res = 0;
//	double starting_x = x1;
//	double delta_x = (x2 - x1) / sqrt(steps);
//	double delta_y = (y2 - y1) / sqrt(steps);
//	double area = delta_x * delta_y;
//	while (y1 < y2) {
//		while (x1 < x2) {
//			res += Function::func_to_integrate(x1, y1) * area;
//			x1 += delta_x;
//		}
//		y1 += delta_y;
//		x1 = starting_x;
//	}
//	return res;

	double res = 0;
	auto steps_x_y = static_cast<size_t>(sqrt(steps));
	size_t new_steps_x_y = steps_x_y & static_cast<size_t>(-4);
	double delta_x = (x2 - x1) / new_steps_x_y;
	double delta_y = (y2 - y1) / sqrt(steps);
	double area = delta_x * delta_y;
	for (size_t i = 0; i < new_steps_x_y; i++) {
		for (size_t j = 0; j < new_steps_x_y; j++) {
			res += Function::func_to_integrate(x1 + delta_x * i, y1 + delta_y * j) * area;

		}
	}
	return res;
}
__m256d func_to_integrate_simd(const __m256d& x, const __m256d& y) {
	__m256d res = _mm256_setzero_pd();
//	return res;
	for (int j =-2; j <= 2; j++) {
		for (int i =-2; i <= 2; i++) {
			__m256d v0 = _mm256_mul_pd(x, y);
			res = _mm256_add_pd(res, v0);
		}
	}
	return res;

}

double integrate_simd(double x1, double x2, double y1, double y2, const configuration_t& config, size_t steps_x, size_t steps_y) {



//	double res = 0;
	size_t new_steps_y = steps_y & static_cast<size_t>(-4);
	double delta_x = (x2 - x1) / steps_x;
	double delta_y = (y2 - y1) / new_steps_y;
//	std::cout << delta_y << " delta_y" << std::endl;
	__m256d vsum = _mm256_setzero_pd();
	__m256d step_y = _mm256_set1_pd(4 * delta_y);
	__m256d step_x = _mm256_set1_pd(delta_x);
	__m256d vx = {x1, x1, x1, x1};
	__m256d vy = {y1, y1 + delta_y, y1 + 2 * delta_y, y1 + 3 * delta_y};
	__m256d vy_init = vy;
	for (size_t i = 0; i < steps_x; i++) {
		for (size_t j = 0; j < new_steps_y; j+=4) {

			__m256d cur_res = func_to_integrate_simd(vx, vy); //double cur_res = Function::func_to_integrate(vx, vy); // cur_res is a register
			vy = _mm256_add_pd(vy, step_y); //change register with y's + 4 * delta_y;
			vsum = _mm256_add_pd(vsum, cur_res); // res += cur_res;
		}
		vx = _mm256_add_pd(vx, step_x); //change register with x's * delta_y;
		vy = vy_init; // reset register y's
	}
	 vsum = _mm256_hadd_pd(vsum, vsum);
	__m128d sum_high = _mm256_extractf128_pd(vsum, 1);
	__m128d result = _mm_add_pd(sum_high, _mm256_castpd256_pd128(vsum));
	double res = _mm_cvtsd_f64(result);
	return res * delta_x * delta_y;
}

