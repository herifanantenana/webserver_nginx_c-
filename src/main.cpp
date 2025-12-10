#include "main.hpp"

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		LOG_ERROR("Usage: %s <config_file>", argv[0]);
		return EXIT_FAILURE;
	}
	try
	{
		config::ParserConfig parserConfig(argv[1]);
		parserConfig.parseConfigFile();
		parserConfig.setup();
		parserConfig.printConfig();
	}
	catch (const std::exception &e)
	{
		HANDLE_EXCEPTION(e);
	}

	return EXIT_SUCCESS;
}
