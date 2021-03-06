#include<stdio.h>
#include <string.h>
#include <RFduinoGZLL.h>

device_t role = HOST;

const int led = 2;										//Anzeige LED
const int AnRe1 = 3;									//Antriebe
const int AnRe2 = 4;
const int AnLi1 = 5;
const int AnLi2 = 6;

const int angularFactor = 5.0;							//Faktor welcher mit dem Winkelmultipliziert wird

boolean Init = false;									//Initalisierungs-Variable
boolean busy = false;									//Merker fuer Bearbeitungskennzeichnung

//String incoming;
char incoming[20];										//Einzulesender String
char wastebin[10];										//Zu vernachlaessigendes Array

volatile int inco1 = 0, inco2 = 0, inco3 = 0;			// eingelesener Strings (Geschwindigkeit, Winkel, Zeit)
int gzll_inco1 = 0, gzll_inco2 = 0, gzll_inco3 = 0;


int rate = 0;											//Geschwindigkeit in %
int angular = 0;										//Winkel in °
int period = 0;											//Dauer in ms

bool data_available;

// custom packet max size 20 bytes.
struct myCustomPacket_t
{
    int rate = 0;
    int angular = 0;
    int period = 0;  
};

struct myCustomPacket_t packet;

void setup()
{
	pinMode(led, OUTPUT);								//Definition der AusgÃƒÂ¤nge
	pinMode(AnRe1, OUTPUT);
	pinMode(AnRe2, OUTPUT);
	pinMode(AnLi1, OUTPUT);
	pinMode(AnLi2, OUTPUT);

	data_available = false;
	RFduinoGZLL.begin(role);

	Serial.begin(9600);									//Aktivieren der Seriellen Kommunikation
	delay(100);											//kurze Wartezeit
}

	//******************************************
	//Initalisierung
	//******************************************
void initialize()
{
	Serial.print("Initialzing");
	digitalWrite(led, LOW);
	digitalWrite(AnRe1, LOW);
	digitalWrite(AnRe2, LOW);
	digitalWrite(AnLi1, LOW);
	digitalWrite(AnLi2, LOW);

	//Blink to show initialize
	for (int t1 = 0; t1 < 15; t1++)
	{
		digitalWrite(led, HIGH);

		delay(150);
		digitalWrite(led, LOW);
		delay(150);
		Serial.print(".");
	}

	// Turn off all LED + Motors
	digitalWrite(led, LOW);
	digitalWrite(AnRe1, LOW);
	digitalWrite(AnRe2, LOW);
	digitalWrite(AnLi1, LOW);
	digitalWrite(AnLi2, LOW);

	Init = true;										//Initialisierung Abgeschlossen
	Serial.println("Done!\n");
	Serial.println("rate [%]; angular [°]; time [ms]\n");
}

void loop()
{
	//******************************************
	//Initalisierungsphase einmalig zu Beginn
	//******************************************
	if (Init == false)
	{
		initialize();
	}else	/*Beginn des eigentlichen Programms*/
	{
		if(data_available)
		{
			/*Serial.println("Starting distributor...");
			Serial.print("Recieved Data ");
			Serial.print(inco1);
			Serial.print(" ");
			Serial.print(inco2);
			Serial.print(" ");
			Serial.println(inco3);*/
			distributor(inco1, inco2, inco3);
			data_available = false;         
		}else //USB?
		{
		/*	int nrOfBytes = Serial.available();					//Erfassung des belegten Buffers		
			if (nrOfBytes < 5 && busy == false)					//Sollte der Buffer nur zum Teil befuellt worden sein wird dieser hier geloescht
			{
				//falls zu wenig Daten, check ob nach 100 ms immer noch der fall, dann leeren
				int tmp = nrOfBytes;
				delay(100);
				nrOfBytes = Serial.available();
				if(tmp >= nrOfBytes)
					emptyBuffer(nrOfBytes);
			}
			else												// wenn String vollstaendig vorhanden ist wird die Bearbeitung gestartet.
			{
				busy = true;
				Serial.print("Anzahl Bytes: ");
				Serial.println(nrOfBytes);
				for (int i = 0; i < nrOfBytes; i++)
				{
					incoming[i] = Serial.read();				//Auslesen des Buffers und schreiben
				}

				Serial.print("I received: ");					//gibt den empfangen String aus
				Serial.println(incoming);

				inco1 = inco2 = inco3 = 0;
																//aufloesen des Strings in:
				inco1 = atoi(strtok(incoming, ";"));			//Geschwindigkeit
				inco2 = atoi(strtok(NULL, ";"));					//Winkel
				inco3 = atoi(strtok(NULL, ";"));					//Dauer

				/*Serial.print("Speed: ");
				Serial.print(inco1);
				Serial.print(" %, Angle: ");
				Serial.print(inco2);
				Serial.print("°, Duration: ");
				Serial.print(inco3);
				Serial.println("s");

				memset(incoming, 0, sizeof(incoming));   //leeren des Einlese-Strings

				distributor(inco1, inco2, inco3);
			}*/
		}
	}
}

/*	Event! Wird ausgefuehrt, wenn ein Datenpacket ueber Bluetooth empfangen wird
	Datenpacket muss enthalten:
		>> RATE (%); ANGULAR (°); PERIOD (ms) <<
*/
void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len)
{
	if (device == DEVICE0)
	{
		//Serial.println("from GZLL: ");

		memcpy( &packet, &data[0], len);

		/*Serial.println("Test parameters:");
		Serial.print("Speed: ");
		Serial.print(packet.rate);
		Serial.print(" %, Angle: ");
		Serial.print(packet.angular);
		Serial.print("°, Duration: ");
		Serial.print(packet.period);
		Serial.println("s");*/
		 
		inco1 = packet.rate;
		inco2 = packet.angular;
		inco3 = packet.period;

		/*Serial.println("Test copied variables: ");
		Serial.print("Speed: ");
		Serial.print(inco1);
		Serial.print(" %, Angle: ");
		Serial.print(inco2);
		Serial.print("°, Duration: ");
		Serial.print(inco3);
		Serial.println("s");*/

		data_available = true;
	}
}

void emptyBuffer(int nrBytes)
{
	for (int i = 0; i < nrBytes; i++) {
		wastebin[i] = Serial.read();
	}
}

