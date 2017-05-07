#pragma once

#include <ostream>
#include <vector>

namespace Dlink
{
	class Error final
	{
	private:
		const bool is_warning_;

		std::string error_message_;
		std::size_t line_;

	public:
		Error(const std::string& error_message, const std::size_t& line);
		Error(const std::string& error_message, const std::size_t& line, bool is_warning);
		Error(const Error& error);
		~Error() = default;

	public:
		Error& operator=(const Error& error) = delete;
		Error& operator=(Error&& error) noexcept = delete;

	public:
		void print(std::ostream& os);
	};

	using ErrorList = std::vector<Error>;
}
