#include "Dlink/Dlink.hh"

#include <iostream>

namespace Dlink
{
	bool Dlink::parse_command_line(int argc, char* argv[]) noexcept
	{
		if (argc == 1)
		{
			std::cout << "Error: No input files.";
			return false;
		}

		switch (argc)
		{
		case 1:
		{
			std::cout << "Error: No input files.";
			return false;
		}

		case 2:
			return parse_command_line_2(argc, argv);
		}
	}

	bool Dlink::parse_command_line_2(int argc, char* argv[]) noexcept
	{
		if (argv[1] == "--version")
		{
			std::cout << "Dlink 1.0.0\n"
						 "(C) 2017. kmc7468, gear, Funniest All rights reserved.";
		}
	}
}