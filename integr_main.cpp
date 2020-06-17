#include <iostream>
#include <fstream>
#include <chrono>
#include <cassert>
#include <atomic>
#include <cmath>
#include <exception>
#include <string>
#include <vector>
#include <thread>
#include <emmintrin.h>
#include "include/config_parser.h"
#include "include/integration.h"
#include "mutex"

std::mutex mx;



inline std::chrono::steady_clock::time_point get_current_time_fenced() {
	assert(std::chrono::steady_clock::is_steady &&
			       "Timer should be steady (monotonic).");
	std::atomic_thread_fence(std::memory_order_seq_cst);
	auto res_time = std::chrono::steady_clock::now();
	std::atomic_thread_fence(std::memory_order_seq_cst);
	return res_time;
}

template<class D>
inline long long to_us(const D &d) {
	return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

using namespace std;

double integr_threads(configuration_t config, size_t steps) {
	vector<thread> threads;
	vector<double> res_local;
	double cur_res = 0;


	for (int i = 0; i < config.num_threads; ++i) {
		threads.emplace_back(thread{integrate_with_threads, i, config,
				       steps, ref(res_local), std::ref(mx)});
	}
	for (auto &thread: threads) {
		thread.join();
	}

	for (auto &res: res_local) {
		cur_res += res;
	}
	
	threads.clear();
	res_local.clear();
	return cur_res;
}





int main(int argc, char *argv[]) {
	string filename("conf.txt");
	if (argc == 2)
		filename = argv[1];
	if (argc > 2) {
		cerr << "Too many arguments. Usage: \n"
		        "<program>\n"
		        "or\n"
		        "<program> <config-filename>\n" << endl;
		return 1;
	}
	ifstream config_stream(filename);
	if (!config_stream.is_open()) {
		cerr << "Failed to open configuration file " << filename << endl;
		return 2;
	}

	configuration_t config;
	try {
		config = read_conf(config_stream);
	} catch (std::exception &ex) {
		cerr << "Error: " << ex.what() << endl;
		return 3;
	}

	auto before = get_current_time_fenced();
	size_t steps = config.initial_steps;
	double cur_res;

	if (config.num_threads == 1) {
		cur_res = integrate(config.x1, config.x2, config.y1, config.y2, steps);
	}
	else {
//		cur_res = integr_threads(config, steps);
		cur_res = integrate_simd(config.x1, config.x2, config.y1, config.y2, config, static_cast<size_t>(sqrt(steps)), static_cast<size_t>(sqrt(steps)));
	}
	double prev_res;
	bool to_continue = true;
	double abs_err = -1; // Just guard value
	double rel_err = -1; // Just guard value
// #define PRINT_INTERMEDIATE_STEPS
	while (to_continue) {
#ifdef PRINT_INTERMEDIATE_STEPS
		cout << cur_res << " : " << steps << " steps" << endl;
#endif
		prev_res = cur_res;
		steps *= 2;

		if (config.num_threads == 1) {
			cur_res = integrate(config.x1, config.x2, config.y1, config.y2, steps);
		}
		else {
//			cur_res = integr_threads(config, steps);
			cur_res = integrate_simd(config.x1, config.x2, config.y1, config.y2, config, static_cast<size_t>(sqrt(steps)), static_cast<size_t>(sqrt(steps)));
		}

		abs_err = fabs(cur_res - prev_res);
		rel_err = fabs((cur_res - prev_res) / cur_res);
#ifdef PRINT_INTERMEDIATE_STEPS
		cout << '\t' << "Abs err : rel err " << abs_err << " : " << rel_err << endl;
#endif
		to_continue = (abs_err > config.abs_err);
		to_continue = to_continue && (rel_err > config.rel_err);
		to_continue = to_continue && (steps < config.max_steps);
	}
	auto time_to_calculate = get_current_time_fenced() - before;
	cout << "Result: " << cur_res << endl;
	cout << "Abs err: " << abs_err << endl << "Rel err: " << rel_err << endl;
	cout << "Time: " << to_us(time_to_calculate);

	return 0;
}

