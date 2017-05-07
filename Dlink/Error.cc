#include "Dlink/Error.hh"

namespace Dlink
{
	Error::Error(const std::string& error_message, const std::size_t& line)
		: Error(error_message, line, false)
	{}
	Error::Error(const std::string& error_message, const std::size_t& line,
				 bool is_warning)
		: error_message_(error_message), line_(line), is_warning_(is_warning)
	{}
	Error::Error(const Error& error)
		: error_message_(error.error_message_), line_(error.line_),
		is_warning_(error.is_warning_)
	{}

	void Error::print(std::ostream& stream)
	{
		stream << (is_warning_ ? "Warning:" : "Error:") << " Line " << line_ << \
			" -> " << error_message_.c_str() << std::endl;
	}
}