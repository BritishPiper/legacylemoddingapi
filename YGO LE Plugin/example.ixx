export module example;

import log;
import manager;
import plugin;

// 00000001407FF695 - GetDuelDeck

/*uint64_t oGetDuelPointer = 0;



DuelData* GetDuelPointer(int arc, int index)
{
	if (arc == 4 && index == 0x25) {
		return GetDuelData(0xE2);
	}
	else {
		FixPeskyBug1();

		return PLH::FnCast(oGetDuelPointer, &GetDuelPointer)(arc, index);
	}
}*/

/*class PassLogger : public PLH::Logger {
	void log(std::string msg, PLH::ErrorLevel level)
	{
		PluginManager::Log(msg);
	}
};*/

export class Example final : public Plugin {
public:
	Example(void)
	{
		//PassLogger* logg = new PassLogger();
		//PLH::Log::registerLogger(std::shared_ptr<PLH::Logger>(logg));
		// This is a temporary workaround for polyhook
	}

	const char* Name(void)
	{
		return "Example";
	}

private:
	template <class... Types>
	static void Log(const char* message, const Types&... arguments) noexcept
	{
		Logger::Log(message, arguments...);
	}
};
