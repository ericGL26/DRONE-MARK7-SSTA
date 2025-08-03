#include <SPI.h>
#include <nRF24L01.h> //DEFINICOES DE REGISTROS INTERNOS DO NRF
#include <RF24.h> //FORNECE A INTERFACE, COMANDOS MAIS IMPORTANTES PARA USAR O MODULO
#include <pins.h>

SPIClass *vspi = NULL;
RF24 radio(CE_PIN, CNS_PIN);

const byte address[6] = "DRONE"; //canal de recepcao
char receivedCommand[32];

void setup(){
  Serial.begin(9600);
  vspi = new SPIClass(VSPI);
  vspi->begin(SCK_PIN, MISO_PIN, MOSI_PIN, CSN_PIN);
  if(!radio.begin(vspi, CE_PIN, CNS_PIN)){
    Serial.println("Falha ao iniciar rádio");
    while(1);
  }

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_1MBPS);
  radio.startListening();
  Serial.print("Receptor do drone pronto");
}

void loop(){
  if(radio.available()){
    radio.read(&receivedCommand, sizeof(receivedCommand));
    Serial.print("comando recebido: ");
    serial.print(receivedCommand);

    radio.stopListening();
    delay(5);

    const char ackMsg[] = "COMANDO RECEBIDO";
    radio.write(&ackMsg, sizeof(ackMsg));

    radio.startListening();
  }
}