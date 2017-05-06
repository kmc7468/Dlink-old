#pragma once

#include <ostream>

namespace Dlink
{
	class Error
	{
		private:
			std::string error_message_;
			std::size_t line_;

			const bool is_warning_ = false;
		public:
			Error(const std::string error_message, std::size_t line, bool is_warning = false) 
				: error_message_(error_message), line_(line), is_warning_(is_warning) {}
			void print(std::ostream& os)
			{
				os << (is_warning_ ? "[Warning]" : "[Error]") << " Line " << line_ << " : " << error_message_ << std::endl;
			}
	};

	using ErrorList = std::vector<Error>;
}
