module;

#include <format>
#include <string>

export module error;

using namespace std;

export template <typename Enumeration>
constexpr Enumeration undefined = static_cast<Enumeration>(-1);

export struct Error {
	enum class Type {
		SUCCESS,
		WARNING,
		EXCEPTION,
		MINOR_ERROR,
		MAJOR_ERROR,
		CRITICAL_ERROR
	};

	string location;
	string message;
	Type type;

	template <class... Types>
	Error(Type type, const char* location, const char* format, const Types&... arguments) : location(location), message(std::format(format, arguments...)), type(type) {}

	static constexpr const char* GetTypeName(Type type)
	{
		switch (type) {
		case Type::SUCCESS: return "Success";
		case Type::WARNING: return "Warning";
		case Type::EXCEPTION: return "Exception";
		case Type::MINOR_ERROR: return "Minor Error";
		case Type::MAJOR_ERROR: return "Major Error";
		case Type::CRITICAL_ERROR: return "Critical Error";
		default: return "";
		}
	}
};
