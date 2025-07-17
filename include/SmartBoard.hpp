#ifndef _INC_SCB_
#define _INC_SCB_

#ifndef USING_EMULATOR

#include <Application.hpp>

class SmartBoard : public Application {
	public:
		void processMessage(unsigned char * message, size_t length, bool printable) override;
		void initializeModulesPointerArray(unsigned int quantity) override;
};

#endif

#endif // ifndef USING_EMULATOR
