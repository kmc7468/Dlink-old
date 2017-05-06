#pragma once

namespace Dlink
{
	class Error
	{
		class Error
		{
			private:
				const std::string error_message_;
				const std::size_t line_;

				const bool is_warning_ = false;
			public:
				Error(const std::string error_message, std::size_t line, bool is_warning = false) 
					: m_error_message(error_message), m_line(line), m_is_warning(is_warning) {}
				void print(std::ostream& os)
				{
					os << (m_is_warning ? "[Warning]" : "[Error]") << " Line " << m_line << " : " << m_error_message << std::endl;
				}
		};

		using ErrorList = std::vector<Error>;
	}
}
