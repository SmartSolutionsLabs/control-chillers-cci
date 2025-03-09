#ifndef _INC_PUMP_
#define _INC_PUMP_

#include <Module.hpp>

class Pump : public Module {
	public:
        Pump(const char * name, int taskCore = 1);

		void connect(void * data) override;

		void run(void* data) override;
};

#endif
