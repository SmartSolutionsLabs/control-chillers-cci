#include "SmartBoard.hpp"
#include "Control.hpp"
#include "Keypad.hpp"

void SmartBoard::processMessage(unsigned char * message, size_t length, bool printable) { // se define que hayq ue procesar
	
}

void SmartBoard::initializeModulesPointerArray(unsigned int quantity) {
	this->modulesPointerQuantity = quantity;

	this->modulesPointer = new Module*[quantity];

	this->modulesPointer[0] = new Control("ctl");
	this->modulesPointer[0]->connect(nullptr);
	this->modulesPointer[0]->start();

	this->modulesPointer[1] = new Keypad("keypad");
	this->modulesPointer[1]->connect(&Wire);
	static_cast<Keypad*>(this->modulesPointer[1])->setControl(static_cast<Control*>(this->modulesPointer[0])); //cochina zoo
	this->modulesPointer[1]->start();

	this->modulesPointer[2] = new GraphicLCD("lcd");
	static_cast<Control*>(this->modulesPointer[0])->setGraphicLCD(static_cast<GraphicLCD*>(this->modulesPointer[2])); //cochina zoo
	this->modulesPointer[2]->connect(nullptr);
	this->modulesPointer[2]->start();

	this->modulesPointer[3] = new Chiller("chiller1");
	this->modulesPointer[3]->connect(nullptr);
	this->modulesPointer[3]->start();

	this->modulesPointer[4] = new Chiller("chiller2");
	this->modulesPointer[4]->connect(nullptr);
	this->modulesPointer[4]->start();

	this->modulesPointer[5] = new Pump("pump1");
	this->modulesPointer[5]->connect(nullptr);
	this->modulesPointer[5]->start();

	this->modulesPointer[6] = new Pump("pump2");
	this->modulesPointer[6]->connect(nullptr);
	this->modulesPointer[6]->start();

}

SmartBoard * smartboard;

void setup() {
	Serial.begin(115200);
	Wire.begin();
	smartboard = new SmartBoard();
	//smartboard->beginSerialPort(Serial2);
	smartboard->initializeModulesPointerArray(7);
}

void loop() {
}
