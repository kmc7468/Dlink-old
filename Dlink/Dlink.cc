#include "Dlink/Dlink.hh"

#include <cstring>
#include <iostream>

namespace Dlink
{
	std::string Dlink::startup_path;
	std::vector<std::string> Dlink::input_files;
	std::string Dlink::output_file;
	int Dlink::optimization_level = 0;

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
			return parse_command_line_default(argc, argv);
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
			return parse_command_line_default(argc, argv);
		}
	}
	bool Dlink::parse_command_line_default(int argc, char* argv[])
	{
		bool _O = false;
		bool _o = false;

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
				else
				{
					std::string front = cmdline.substr(0, 2);

					if (front == "-O")
					{
						if (_O)
						{
							std::cout << "Error: Optimization options can only be used once."
								<< std::endl;
							return false;
						}
						_O = true;
						optimization_level = std::stoi(cmdline.substr(2));
					}
					else if (cmdline == "-o")
					{
						if (_o)
						{
							std::cout << "Error: The output file path option"
								" can only be used once." << std::endl;
							return false;
						}
						_o = true;
						++i;
						if (i == argc)
						{
							std::cout << "Error: There is no argument for -o." << std::endl;
							return false;
						}
						else if (std::string(argv[i]).front() == '-')
						{
							std::cout << "Error: The argument to -o can not be an option."
								<< std::endl;
							return false;
						}
						else
						{
							output_file = argv[i];
						}
					}

					else
					{
						std::cout << "Error: " << cmdline << " is unknown option.\n";
						return false;
					}
				}
			}
			else
			{
				input_files.push_back(cmdline);
			}
		}

		if (input_files.size() == 0 &&
			(_o || _O))
		{
			std::cout << "Error: No input files." << std::endl;
			return false;
		}

		return true;
	}
}
