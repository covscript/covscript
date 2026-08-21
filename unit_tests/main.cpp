#include "covariant_test.hpp"
#include <cstring>

static void print_usage(const char *prog)
{
	std::cout << "Usage: " << prog << " [options]\n"
	          << "Options:\n"
	          << "  --filter=<pattern>  Run only tests whose name contains <pattern>\n"
	          << "  --exclude=<pattern> Skip tests whose name contains <pattern>\n"
	          << "  --repeat=N          Repeat all tests N times\n"
	          << "  --shuffle           Randomize test execution order\n"
	          << "  --timeout=N         Warn if a test exceeds N milliseconds\n"
	          << "  --xml=<path>        Write JUnit XML report to <path>\n"
	          << "  --list              List all registered tests without running\n"
	          << "  --help              Show this help message\n";
}

int main(int argc, char *argv[])
{
	cs_test::run_options opts;
	for (int i = 1; i < argc; ++i)
	{
		const char *arg = argv[i];
		if (std::strncmp(arg, "--filter=", 9) == 0)
			opts.filter = arg + 9;
		else if (std::strncmp(arg, "--exclude=", 10) == 0)
			opts.exclude = arg + 10;
		else if (std::strncmp(arg, "--repeat=", 9) == 0)
			opts.repeat = std::atoi(arg + 9);
		else if (std::strcmp(arg, "--shuffle") == 0)
			opts.shuffle = true;
		else if (std::strncmp(arg, "--timeout=", 10) == 0)
			opts.timeout_ms = std::atoi(arg + 10);
		else if (std::strncmp(arg, "--xml=", 6) == 0)
			opts.xml_path = arg + 6;
		else if (std::strcmp(arg, "--list") == 0)
			opts.list_only = true;
		else if (std::strcmp(arg, "--help") == 0)
		{
			print_usage(argv[0]);
			return 0;
		}
		else
		{
			std::cerr << "Unknown option: " << arg << std::endl;
			print_usage(argv[0]);
			return 1;
		}
	}
	if (opts.repeat < 1)
		opts.repeat = 1;
	return cs_test::run_all(opts);
}
