#include "Chiller.hpp"

Chiller::Chiller(const char * name, int taskCore) : Module(name, taskCore) {
}

void Chiller::connect(void * data) {
}

void Chiller::run(void* data) {
    this->iterationDelay = 1 / portTICK_PERIOD_MS;

	while (1) {
		vTaskDelay(this->iterationDelay);
	}
}
