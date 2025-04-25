/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9  // Configurable, see typical pin layout above
#define SS_PIN 10  // Configurable, see typical pin layout above

#define TRIGGERPIN 5  // this is the pin that sends out the pulse.         servo
#define ECHOPIN 2     // this is the pin that reads the distance

#include <Servo.h> 
#define MICHAEL_PIN 7
//make a servo object

Servo  michael;

int const RX_PIN = 4;  //this is the rx pin this recieves the bluetooth
int const TX_PIN = 3;  // this tx pin transmits the bluetooth;
#include <SoftwareSerial.h>
SoftwareSerial tooth(TX_PIN, RX_PIN);  // make a bluetooth object DSD TECH 09c70333
// set tx and rx pins
// tx goes first then rx
char davis;

#define RED_LED 8  // where the red led is connected to arduino board


#define ENABLE 5  // pin that the motor to turn on with insensity DC MOTOR
#define DIRECTIONA 7
#define DIRECTIONB 6
bool lock = false;

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  Serial.begin(9600);  // Initialize serial communications with the PC
  while (!Serial)
    ;                                 // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();                        // Init SPI bus
  mfrc522.PCD_Init();                 // Init MFRC522
  delay(4);                           // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  pinMode(TRIGGERPIN, OUTPUT);  // send pulse
  pinMode(ECHOPIN, INPUT);      // input because reading pulsing that is coming in

  pinMode(ENABLE, OUTPUT);
  pinMode(DIRECTIONA, OUTPUT);
  pinMode(DIRECTIONB, OUTPUT);

  pinMode(RED_LED, OUTPUT);
    tooth.begin(9600);

    michael.attach(MICHAEL_PIN); // connecting the serve object to the pin
  michael.write(90); // set the start of propellor to 90 degrees

}

void loop() {
  //Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  digitalWrite(TRIGGERPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGERPIN, HIGH);
  delayMicroseconds(10);

  float duration = pulseIn(ECHOPIN, HIGH);  // read duration from sensor
  // distance=speed*time
  float speed = 0.034;                    // cm per micro seconds
  float distance = speed * duration / 2;  // distnace between sensor hitting the way and coming back
  Serial.println(distance);

  if (distance > 10) {
    Serial.println("green go");
    digitalWrite(RED_LED, LOW);
  }
  if (distance < 10 && distance > 5) {
    Serial.println("yellow slow down");
    digitalWrite(RED_LED, LOW);
    lock=false;
  }
  if (distance <= 5) {
    Serial.println("stop red");
    digitalWrite(RED_LED, HIGH);
    lock=true;
  }

// Serial.println("bluetooth");
delay(200);

  // put your main code here, to run repeatedly:
  if (tooth.available() > 0) {
// Serial.println("bluetooth");

    davis = tooth.read();
    tooth.println("reading new imput:");
    tooth.print(davis);
  }
  if (davis == 'd') {
    Serial.println("GO");
    digitalWrite(RED_LED, HIGH);
    delay(100);
    digitalWrite(RED_LED, LOW);
    delay(100);
    digitalWrite(RED_LED, HIGH);
    delay(100);
    digitalWrite(RED_LED, LOW);
    delay(100);
    digitalWrite(RED_LED, HIGH);
    delay(100);
    digitalWrite(RED_LED, LOW);
    delay(100);
    lock=true;
  }

  delay(100);
if(lock==true){
   michael.write(0); // set the start of propellor to 90 degrees
  delay(2000); 
  michael.write(180); 
  delay(2000);
}
  //to 100  



  delay(100);
Serial.println("rfid");


  if (!mfrc522.PICC_IsNewCardPresent()) {
    Serial.println("new card?");

    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {

        Serial.println("read card?");

    return;
  }


  	// mfrc522.PICC_DumpToSerial(&(mfrc522.uid));


  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);



  Serial.print(F("RFID Tag UID:"));

  printHex(mfrc522.uid.uidByte, mfrc522.uid.size);

  Serial.println("");



  mfrc522.PICC_HaltA();  // Halt PICC
}

void printHex(byte *buffer, byte bufferSize) {



  //Serial.begin("reading?");

  for (byte i = 0; i < bufferSize; i++) {

    Serial.print(buffer[i] < 0x10 ? " 0" : " ");

    Serial.print(buffer[i], HEX);
  }
}
