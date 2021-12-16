module;

#include <fstream>
#include <mutex>
#include <vector>

#include "../globals.h"
#include "error.h"

export module log;

using namespace std;

export class Logger {
public:
	__declspec(dllexport) static void Log(const char* message) noexcept
	{
		static mutex mutex;
		mutex.lock();

		for (string& str : delayed) {
			LogInternal(str.c_str());
		}

		LogInternal(message);

		mutex.unlock();

		delayed.clear();
	}

	static void Log(const string& message) noexcept
	{
		Log(message.c_str());
	}

	template <typename Message, class... Arguments>
	static void Log(Message message, const Arguments&... arguments) noexcept
	{
		Log(format(message, arguments...).c_str());
	}

	template <typename Message>
	__declspec(dllexport) static void LogDelayed(const Message& message) noexcept
	{
		delayed.push_back(message);
	}

	template <typename Message, class... Arguments>
	static void LogDelayed(Message message, const Arguments&... arguments) noexcept
	{
		LogDelayed(format(message, arguments...));
	}

	static void Setup(void)
	{
		string name = string(log_folder) + Now() + " " + log_file;
		log = ofstream(name);

		if (!log) {
			throw CriticalError("Failed to open log file {}.", name.c_str());
		}

		Log("Log file created.");
	}

	static void Show(const Error& error) noexcept
	{
		Log("{} - {}", Error::GetTypeName(error.type), error.message);
	}

private:
	static vector<string> delayed;
	static ofstream log;

	static void LogInternal(const char* message) noexcept
	{
		string formatted = Now() + " | " + message;

		log << formatted << std::endl;
		log.flush();
	}

	static string Now(void)
	{
		time_t long_now = time(nullptr);
		tm now;
		localtime_s(&now, &long_now);

		stringstream stream;
		stream << put_time(&now, "%F %Hh %Mmin %Ss");

		return stream.str();
	}
};

vector<string> Logger::delayed;
ofstream Logger::log;
