#ifndef _INC_CHILLER_
#define _INC_CHILLER_

#include <Module.hpp>

class Chiller : public Module {
	public:
        Chiller(const char * name, int taskCore = 1);

		void connect(void * data) override;

		void run(void* data) override;
};

#endif
