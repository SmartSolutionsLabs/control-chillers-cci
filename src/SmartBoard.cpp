#include "SmartBoard.hpp"
#include "Control.hpp"
#include "Keypad.hpp"
#include "Network.hpp"

String ssid = "SmartLabs";
String password = "20120415H";

Control * globalControl; // to use in lambdas

void SmartBoard::processMessage(unsigned char * message, size_t length, bool printable) { // se define que hayq ue procesar

}

void SmartBoard::initializeModulesPointerArray(unsigned int quantity) {
	this->modulesPointerQuantity = quantity;

	this->modulesPointer = new Module*[quantity];

	this->modulesPointer[0] = new Control("ctl");
	globalControl = static_cast<Control*>(this->modulesPointer[0]);
	this->modulesPointer[0]->connect(nullptr);


	this->modulesPointer[1] = new Keypad("keypad");
	this->modulesPointer[1]->connect(&Wire);
	static_cast<Keypad*>(this->modulesPointer[1])->setControl(static_cast<Control*>(this->modulesPointer[0])); //cochina zoo
	this->modulesPointer[1]->start();

	this->modulesPointer[2] = new GraphicLCD("lcd");
	static_cast<Control*>(this->modulesPointer[0])->setGraphicLCD(static_cast<GraphicLCD*>(this->modulesPointer[2])); //cochina zoo
	this->modulesPointer[2]->connect(nullptr);
	this->modulesPointer[2]->start();

	this->modulesPointer[3] = new Chiller("chiller1");
	static_cast<Control*>(this->modulesPointer[0])->setChiller(0,static_cast<Chiller*>(this->modulesPointer[3])); //cochina zoo
	this->modulesPointer[3]->connect(&Wire);
	//this->modulesPointer[3]->start();

	this->modulesPointer[4] = new Chiller("chiller2");
	static_cast<Control*>(this->modulesPointer[0])->setChiller(1,static_cast<Chiller*>(this->modulesPointer[4])); //cochina zoo
	this->modulesPointer[4]->connect(&Wire);
	//this->modulesPointer[4]->start();

	this->modulesPointer[5] = new Pump("pump1");
	static_cast<Control*>(this->modulesPointer[0])->setPump(0,static_cast<Pump*>(this->modulesPointer[5]));
	this->modulesPointer[5]->connect(&Wire);
	//this->modulesPointer[5]->start();

	this->modulesPointer[6] = new Pump("pump2");
	static_cast<Control*>(this->modulesPointer[0])->setPump(1,static_cast<Pump*>(this->modulesPointer[5]));
	this->modulesPointer[6]->connect(&Wire);
	//this->modulesPointer[6]->start();

	this->modulesPointer[0]->start();

	// WiFi events
	WiFi.onEvent(
		[](WiFiEvent_t event, WiFiEventInfo_t info) {
			Serial.print("WiFi connected.\n");
			globalControl->setWifiStatus(true);
		},
		WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED
	);
	WiFi.onEvent(
		[](WiFiEvent_t event, WiFiEventInfo_t info) {
			Serial.print("Addressed with IP \n");
			Serial.print(WiFi.localIP());
			// Ejecutar el método del objeto que controla todo
			globalControl->setWifiIP(WiFi.localIP());
		},
		WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP
	);
	WiFi.onEvent(
		[](WiFiEvent_t event, WiFiEventInfo_t info) {
			Serial.print("Diconnected.\n");
			IPAddress disconectedIP(0, 0, 0, 0);
			globalControl->setWifiIP(disconectedIP);
			globalControl->setWifiStatus(false);
		},
		WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED
	);

	WiFi.onEvent(
		[](WiFiEvent_t event, WiFiEventInfo_t info) {
			Serial.print("WiFi lost IP\n");
			IPAddress disconectedIP(0, 0, 0, 0);
			globalControl->setWifiIP(disconectedIP);
		},
		WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_LOST_IP
	);
}

SmartBoard * smartboard;

void setup() {
	Serial.begin(115200);
	Network::SSID = ssid;
	Network::PASSWORD = password;
	Network::getInstance()->begin("SC-RAIDI8_CCI_CHILLERS",true);
	Network::getInstance()->connect();
	Wire.begin(5,4);
	smartboard = new SmartBoard();
	//smartboard->beginSerialPort(Serial2);
	smartboard->initializeModulesPointerArray(7);
}

void loop() {
}
