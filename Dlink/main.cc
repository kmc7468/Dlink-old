#include "Dlink/Dlink.hh"

namespace dl = Dlink;

int main(int argc, char* argv[])
{
	if (!dl::Dlink::parse_command_line(argc, argv))
	{
		return 0;
	}
	
	return 0;
}
