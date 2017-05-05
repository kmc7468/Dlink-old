#include "Dlink/Dlink.hh"

#include <cstring>
#include <iostream>

namespace Dlink
{
	bool Dlink::parse_command_line(int argc, char* argv[])
	{
		switch (argc)
		{
		case 1:
		{
			std::cout << "Error: No input files.\n";
			return false;
		}

		case 2:
			return parse_command_line_2(argc, argv);
		}
	}

	bool Dlink::parse_command_line_2(int argc, char* argv[])
	{
		if (std::strcmp(argv[1], "--version") == 0)
		{
			std::cout << "Dlink 1.0.0\n"
						 "(C) 2017. kmc7468, gear, Funniest All rights reserved.\n";
			return false;
		}
		else if (std::strcmp(argv[1], "--help") == 0)
		{
			std::cout << "Usage: Dlink [options...]\n"
				"Allowed options:\n\n";

			std::cout << "--help [option]   View all commands, or view detailed\n"
						 "                  descriptions of commands.\n"
						 "--version         View the version of Dlink.\n"
						 "";
		}
	}
}