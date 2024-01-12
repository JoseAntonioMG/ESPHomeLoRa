#include <SPI.h>
#include <LoRa.h>

using namespace esphome;

#define LOR_RST  36
#define LOR_IRQ  37
#define LOR_MOSI 23
#define LOR_MISO 19
#define LOR_CLK  18
#define LOR_SS   38
#define LOR_Band 868E6
struct xPaquete
{
  char Cabecera[2] = { 'T','0' };
  float Temperatura=0.00;
  float Presion=0;
  float Humedad=0;
  float temperaturaPS=0.00;
  float Voltaje=0.00;
};
xPaquete Paquete;
String RSSI="";
bool PaqueteRecibido=false;

void onReceive(int packetSize)
{
  ESP_LOGD("custom", "Recibiendo paquete ...");
  LoRa.readBytes((char*)&Paquete,sizeof(Paquete));
  RSSI="";
  RSSI=String(LoRa.packetRssi());
  ESP_LOGD("custom", "RSSI %s", RSSI);
  PaqueteRecibido=true;
}

class LoRaSensors : public PollingComponent
{
 public:
  Sensor *Temperatura = new Sensor();
  Sensor *Presion = new Sensor();
  Sensor *Humedad = new Sensor();
  Sensor *Voltaje = new Sensor();
  Sensor *RSSI = new Sensor();
  
  LoRaSensors() : PollingComponent(4000) {}

  void setup() override
  {
	  LoRa.setPins(LOR_SS, LOR_RST, LOR_IRQ);
	  if (!LoRa.begin(LOR_Band))
	  {
		  ESP_LOGD("custom", "La iniciación de LoRa ha fallado.");
		  while (true);
	  }
	  ESP_LOGD("custom", "LoRa inicializado.");
	  LoRa.onReceive(onReceive);
	  LoRa.receive();
  }

  void update() override
  {
	  if (PaqueteRecibido==true)
	  {
      if (Paquete.Cabecera[0]=='T' && Paquete.Cabecera[1]=='0')
      {
        Temperatura->publish_state(Paquete.Temperatura);
        Humedad->publish_state(Paquete.Humedad);
        Presion->publish_state(Paqurete.Presion);
        Voltaje->publish_state(Paquete.Voltaje);
        Sensor->publish_state(RSSI);
        PaqueteRecibido=false;
      }
	  }
  }
}