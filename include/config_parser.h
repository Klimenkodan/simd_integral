
#ifndef SAMPLEINT_CONFIG_PARSER_H
#define SAMPLEINT_CONFIG_PARSER_H
struct configuration_t {
	double rel_err, abs_err;
	double num_threads;
	double x1, x2;
	double y1, y2;
	size_t initial_steps;
	size_t max_steps;
};

configuration_t read_conf(std::istream& cf);

#endif //SAMPLEINT_CONFIG_PARSER_H
