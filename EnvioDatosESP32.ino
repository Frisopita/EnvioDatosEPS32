/*
 * Código que funciona para leer los datos de un arreglo
 * Este código que me esta funcionando para realizar pruebas
 */
 // Thread  
#include <Thread.h>
#include <ThreadController.h>
#include <stdint.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERIAL_FRAME_SYNC1 0
#define SERIAL_FRAME_SYNC2 1
#define SERIAL_FRAME_ID 2
#define SERIAL_FRAME_VALUE 3
#define SERIAL_FRAME_CRC 4
#define SERIAL_FRAME_LENGHT 5

#define ID_HEART_RATE 1
#define ID_TEMP 2
#define ID_SPO2 3
#define ID_SYSPRE 4
#define ID_DIASPRE 5
#define ID_FR 6
#define ID_CO2 7
#define ID_TIMER 8
#define ID_STATE 9

#define MAX_DATA_SIZE 5
uint8_t dataArray[MAX_DATA_SIZE];
uint8_t dataSize = 0;
uint8_t txArray[SERIAL_FRAME_LENGHT];

uint8_t receivedData[MAX_DATA_SIZE];
uint8_t receivedSize = 0;
char x = 0;

// ThreadController that will controll all threads
ThreadController controll = ThreadController();
Thread* btThread = new Thread();

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_S1_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_S2_UUID "8bdf0a1a-a48e-4dc3-8bab-ad0c1f7ed218"
#define CHARACTERISTIC_S3_UUID "411fcc1c-e7a5-4a61-82fe-0004993dd1f4"
#define CHARACTERISTIC_S4_UUID "c608f523-aa19-40d1-8359-ad43409c34d7"
#define CHARACTERISTIC_S5_UUID "52294b4d-d66e-4d68-9782-1e5bb8f7ba14"
#define CHARACTERISTIC_S6_UUID "7533653f-6f0e-41fa-8fa6-9892a1904db1"
#define CHARACTERISTIC_S7_UUID "607a2edc-007d-4d51-a3a6-58fad0db3c37"
#define CHARACTERISTIC_S8_UUID "aea7aac8-5a97-488e-bd01-4166d22ec81e"
#define CHARACTERISTIC_S9_UUID "2a1c9f0b-78c4-4e0f-adc1-5dea4a248344"

const uint8_t crc8Table[] = {
0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 
0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d, 
0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65, 
0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d, 
0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5, 
0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd, 
0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 
0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd, 
0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2, 
0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea, 
0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2, 
0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a, 
0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 
0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a, 
0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42, 
0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a, 
0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c, 
0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4, 
0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 
0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4, 
0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c, 
0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44, 
0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c, 
0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34, 
0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 
0x76, 0x71, 0x78, 0x7f, 0x6a, 0x6d, 0x64, 0x63, 
0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b, 
0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13, 
0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb, 
0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83, 
0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 
0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3 
};

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristicS1 = NULL;
BLECharacteristic* pCharacteristicS2 = NULL;
BLECharacteristic* pCharacteristicS3 = NULL;
BLECharacteristic* pCharacteristicS4 = NULL;
BLECharacteristic* pCharacteristicS5 = NULL;
BLECharacteristic* pCharacteristicS6 = NULL;
BLECharacteristic* pCharacteristicS7 = NULL;
BLECharacteristic* pCharacteristicS8 = NULL;
BLECharacteristic* pCharacteristicS9 = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

int S1 = 0;
int S2 = 100;
int S3 = 40;
int S4 = 25;
int S5 = 35;
int S6 = 10;
int S7 = 7;
int S8 = 0;

static BLERemoteCharacteristic* pRemoteCharacteristicRea;

// Función para calcular el CRC-8 de un conjunto de datos
uint8_t calculateCRC8(const uint8_t* data, uint8_t length) {
    uint8_t crc = 0x00; // Valor inicial del CRC

    for (uint8_t i = 0; i < length; i++) {
        crc = crc8Table[crc ^ data[i]]; // Aplicar la tabla de búsqueda
    }

    return crc;
}

//Funcion para codificar el mensaje con crc8
void encodeMessage(int id, int value){
  txArray[SERIAL_FRAME_SYNC1] = 0xA5;
  txArray[SERIAL_FRAME_SYNC2] = 0x5A;
  txArray[SERIAL_FRAME_ID] = id;
  txArray[SERIAL_FRAME_VALUE] = value;
  txArray[SERIAL_FRAME_CRC] = calculateCRC8(txArray, 4);
}
//Funcion para enviar mensaje por serial
void sendMessage()
{
  Serial.write(txArray, SERIAL_FRAME_LENGHT);
}

//Callbacks que reciben datos de bluetooth
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};
class MyCallbacks: public BLECharacteristicCallbacks{
  void onWrite(BLECharacteristic *pCharacteristic){
    std::string rxValue = pCharacteristic->getValue();
    BLEUUID RxUuid = pCharacteristic->getUUID();
    const char* stringArray = RxUuid.toString().c_str();
     if (rxValue.length() > 0) {
    // Limpiar el arreglo antes de copiar los nuevos datos
    memset(dataArray, 0, sizeof(dataArray));
    // Copiar los datos recibidos en el arreglo
    dataSize = rxValue.length();
    for (int i = 0; i < dataSize; i++) {
      dataArray[i] = rxValue[i];
    }
   
    if(memcmp_P(stringArray, CHARACTERISTIC_S1_UUID, 32) == 0)
    {
      encodeMessage(ID_HEART_RATE, dataArray[0]);
      sendMessage();
    }
    else if(memcmp_P(stringArray, CHARACTERISTIC_S2_UUID, 32) == 0)
    {
      encodeMessage(ID_TEMP, dataArray[0]);
      sendMessage();
    }
    else if(memcmp_P(stringArray, CHARACTERISTIC_S3_UUID, 32) == 0)
    {
      encodeMessage(ID_SPO2, dataArray[0]);
      sendMessage();
    }
    else if(memcmp_P(stringArray, CHARACTERISTIC_S4_UUID, 32) == 0)
    {
      encodeMessage(ID_SYSPRE, dataArray[0]);
      sendMessage();
    }
    else if(memcmp_P(stringArray, CHARACTERISTIC_S5_UUID, 32) == 0)
    {
      encodeMessage(ID_DIASPRE, dataArray[0]);
      sendMessage();
    }
    else if(memcmp_P(stringArray, CHARACTERISTIC_S6_UUID, 32) == 0)
    {
      encodeMessage(ID_FR, dataArray[0]);
      sendMessage();
    }
    else if(memcmp_P(stringArray, CHARACTERISTIC_S7_UUID, 32) == 0)
    {
      encodeMessage(ID_CO2, dataArray[0]);
      sendMessage();
    }
    else if(memcmp_P(stringArray, CHARACTERISTIC_S8_UUID, 32) == 0)
    {
      encodeMessage(ID_TIMER, dataArray[0]);
      sendMessage();
    }
    else if(memcmp_P(stringArray, CHARACTERISTIC_S9_UUID, 32) == 0)
    {
      encodeMessage(ID_STATE, dataArray[0]);
      sendMessage();
    }
    else
    {
     // Serial.println("No");
    }
  }

  }
};

//Inicializar 
void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  initBT();
  btThread->onRun(btCallback);
  btThread->setInterval(100);
  controll.add(btThread);
  
}
//Loop
void loop() {
  controll.run();

}
//Flutter btcallback
void btCallback(){
    if (deviceConnected) { 

        // Verificar si hay datos disponibles en el búfer de recepción serial
  if (Serial2.available() > 0) {

    

    Serial2.readBytes(receivedData, 5);
    String receivedString = "";
    for(int i = 0; i < 5; i++) {
      Serial.print(receivedData[i],HEX);
      receivedString += String(receivedData[i], HEX);
    }   
    Serial.println("\n");
    Serial.println(receivedString);

    pCharacteristicS8->setValue((char*)receivedString.c_str());
    pCharacteristicS8->notify();
    pCharacteristicS9->setValue((char*)receivedString.c_str());
    pCharacteristicS9->notify();

    // Verificar si se ha recibido suficiente cantidad de datos
    if (receivedSize >= SERIAL_FRAME_LENGHT) {
      // Verificar la integridad de los datos mediante el cálculo del CRC-8
      uint8_t calculatedCRC = calculateCRC8(receivedData, receivedSize - 1);
      uint8_t receivedCRC = receivedData[receivedSize - 1];

      // Comparar el CRC-8 calculado con el CRC-8 recibido
      if (calculatedCRC == receivedCRC) {
        // El CRC-8 es válido, los datos están completos y sin alteraciones

        // Decodificar los datos
        uint8_t sync1 = receivedData[SERIAL_FRAME_SYNC1];
        uint8_t sync2 = receivedData[SERIAL_FRAME_SYNC2];
        uint8_t id = receivedData[SERIAL_FRAME_ID];
        uint8_t value = receivedData[SERIAL_FRAME_VALUE];

        std::string dataValue(reinterpret_cast<char*>(receivedData), receivedSize);
        pCharacteristicS8->setValue(dataValue);
        pCharacteristicS8->notify();
        pCharacteristicS9->setValue(dataValue);
        pCharacteristicS9->notify();

      } else {
        // El CRC-8 no coincide, los datos pueden estar corruptos o incompletos
        // Manejar el error según sea necesario
        // Serial.println("Nada");
      }
      // Reiniciar el contador de tamaño y prepararse para recibir nuevos datos
      receivedSize = 0;
    }
  }  
    
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        //delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        //Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
void printDataArray() {
 //Serial.print("Data Array: ");
  for (int i = 0; i < dataSize; i++) {
    //Serial.print(dataArray[i]);
    //Serial.print(" ");
  }
  //Serial.println();
}


void initBT(){
  // Create the BLE Device
  BLEDevice::init("ESP32 Sopita");
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  // Create the BLE Service
  BLEService *pService = pServer->createService(BLEUUID(SERVICE_UUID), 50, 0);
  // Create a BLE Characteristic
  pCharacteristicS1 = pService->createCharacteristic(
                      CHARACTERISTIC_S1_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  
  pCharacteristicS2 = pService->createCharacteristic(
                      CHARACTERISTIC_S2_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  
  pCharacteristicS3 = pService->createCharacteristic(
                      CHARACTERISTIC_S3_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  
  pCharacteristicS4 = pService->createCharacteristic(
                      CHARACTERISTIC_S4_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  
  pCharacteristicS5 = pService->createCharacteristic(
                      CHARACTERISTIC_S5_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  
  pCharacteristicS6 = pService->createCharacteristic(
                      CHARACTERISTIC_S6_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  
  pCharacteristicS7 = pService->createCharacteristic(
                      CHARACTERISTIC_S7_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pCharacteristicS8 = pService->createCharacteristic(
                      CHARACTERISTIC_S8_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pCharacteristicS9 = pService->createCharacteristic(
                      CHARACTERISTIC_S9_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
                      
  pCharacteristicS1->setCallbacks(new MyCallbacks());
  pCharacteristicS2->setCallbacks(new MyCallbacks());
  pCharacteristicS3->setCallbacks(new MyCallbacks());
  pCharacteristicS4->setCallbacks(new MyCallbacks());
  pCharacteristicS5->setCallbacks(new MyCallbacks());
  pCharacteristicS6->setCallbacks(new MyCallbacks());
  pCharacteristicS7->setCallbacks(new MyCallbacks());
  pCharacteristicS8->setCallbacks(new MyCallbacks());
  pCharacteristicS9->setCallbacks(new MyCallbacks());
  // Create a BLE Descriptorn
  pCharacteristicS1->addDescriptor(new BLE2902());
  pCharacteristicS2->addDescriptor(new BLE2902());
  pCharacteristicS3->addDescriptor(new BLE2902());
  pCharacteristicS4->addDescriptor(new BLE2902());
  pCharacteristicS5->addDescriptor(new BLE2902());
  pCharacteristicS6->addDescriptor(new BLE2902());
  pCharacteristicS7->addDescriptor(new BLE2902());
  pCharacteristicS8->addDescriptor(new BLE2902());
  pCharacteristicS9->addDescriptor(new BLE2902());
  // Start the service
  pService->start();
  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
  
}