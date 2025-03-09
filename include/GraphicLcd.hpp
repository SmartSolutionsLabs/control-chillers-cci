//george lo sugirio
#ifndef _INC_GRAPHICLCD_
#define _INC_GRAPHICLCD_
#include <Module.hpp>

class GraphicLCD : public Module {
	private:
        
	public:
        GraphicLCD(const char * name, int taskCore = 1);

		void connect(void * data) override;

		void run(void* data) override;

};

#endif