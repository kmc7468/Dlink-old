#include "Dlink/Dlink.hh"

#include <cstring>
#include <iostream>

namespace Dlink
{
	std::string Dlink::startup_path;
	std::vector<std::string> Dlink::input_files;

	bool Dlink::parse_command_line(int argc, char* argv[])
	{
		startup_path = argv[0];

		switch (argc)
		{
		case 1:
		{
			std::cout << "Error: No input files.\n";
			return false;
		}

		case 2:
			return parse_command_line_2(argc, argv);

		case 3:
			return parse_command_line_3(argc, argv);
		
		default:
			return parse_command_line_default(argc, argv);
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
						 "--version         View the version of Dlink.\n";
			return false;
		}
		else
		{
			std::cout << "Error: " << argv[1] << " is unknown option.\n";
			return false;
		}
	}
	bool Dlink::parse_command_line_3(int argc, char* argv[])
	{
		if (std::strcmp(argv[1], "--help") == 0)
		{
			if (std::strcmp(argv[2], "--version") == 0)
			{
				std::cout << "Usage: Dlink --version\n"
							 "View the version of Dlink.\n";
				return false;
			}
			
			std::cout << "Error: " << argv[2] << " is invalid option argument.";
			return false;
		}
		else
		{
			std::cout << "Error: " << argv[1] << " cannot have arguments.\n";
			return false;
		}
	}
	bool Dlink::parse_command_line_default(int argc, char* argv[])
	{
		for (int i = 1; i < argc; ++i)
		{
			std::string cmdline = argv[i];

			if (cmdline == "--help" && i == 1)
			{
				std::cout << "Error: --help can only have a maximum of one argument.\n";
				return false;
			}
			else if (cmdline == "--version" && i == 1)
			{
				std::cout << "Error: " << argv[1] << " cannot have arguments.\n";
				return false;
			}
			else if (cmdline.front() == '-')
			{
				if (cmdline == "--help" || cmdline == "--version")
				{
					std::cout << "Error: Can not use " << cmdline << " here.\n";
					return false;
				}

				// Compile Options
				
				else
				{
					std::cout << "Error: " << cmdline << " is unknown option.\n";
					return false;
				}
			}
			else
			{
				input_files.push_back(cmdline);
			}
		}

		return true;
	}
}
