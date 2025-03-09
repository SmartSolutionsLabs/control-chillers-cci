#ifndef _INC_SCB_
#define _INC_SCB_

#include <Application.hpp>

class SmartBoard : public Application {
	public:
		void processMessage(unsigned char * message, size_t length, bool printable) override;
		void initializeModulesPointerArray(unsigned int quantity) override;
};

#endif
