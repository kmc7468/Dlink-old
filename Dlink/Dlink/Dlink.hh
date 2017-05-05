#pragma once

namespace Dlink
{
	class Dlink
	{
	public:
		Dlink() = delete;
		Dlink(const Dlink& dlink) = delete;
		Dlink(Dlink&& dlink) noexcept = delete;
		~Dlink() = delete;

	public:
		Dlink& operator=(const Dlink& dlink) = delete;
		Dlink& operator=(Dlink&& dlink) noexcept = delete;
		bool operator==(const Dlink& dlink) const noexcept = delete;
		bool operator!=(const Dlink& dlink) const noexcept = delete;

	public:
		static void parse_command_line(int argc, char* argv[]) noexcept;
	};
}