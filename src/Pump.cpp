#include "Pump.hpp"

Pump::Pump(const char * name, int taskCore) : Module(name, taskCore) {
}

void Pump::connect(void * data) {
}

void Pump::run(void* data) {
    this->iterationDelay = 1 / portTICK_PERIOD_MS;

	while (1) {
		vTaskDelay(this->iterationDelay);
	}
}

