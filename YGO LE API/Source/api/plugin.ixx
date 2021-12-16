export module plugin;

export class Plugin {
public:
	virtual ~Plugin(void) {}

	virtual const char* Name(void) = 0;
};
