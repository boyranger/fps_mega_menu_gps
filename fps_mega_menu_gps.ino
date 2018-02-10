#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
//#include <Adafruit_PCD8544.h>

#include <TinyGPS.h>
#include "Adafruit_GFX.h"// Hardware-specific library
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <DFMiniMp3.h>
#include <Keypad.h>

uint8_t id;

uint8_t getFingerprintEnroll();

const byte numRows= 4;
const byte numCols= 4;

char keymap[numRows][numCols]= 
{
{'1', '2', '3', 'A'}, 
{'4', '5', '6', 'B'}, 
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

byte rowPins[numRows] = {22,24,26,28}; 
byte colPins[numCols]= {30,32,34,36};

Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

/*-----------------------KEYPAD-----------------------*/
/*--------------------------VARIABLES------------------------*/
String password="2580"; //Variable to store the current password
String tempPassword=""; //Variable to store the input password
int doublecheck;
boolean armed = false;  //Variable for system state (armed:true / unarmed:false)
boolean input_pass;   //Variable for input password (correct:true / wrong:false)
boolean storedPassword = true;
boolean changedPassword = false;
boolean checkPassword = false;
char phone_no[] = "089523170934";

TinyGPS gps;

int getFingerprintIDez();

// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
SoftwareSerial mySerial(10, 11);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// On Leonardo/Micro or others with hardware serial, use those! #0 is green wire, #1 is white
//Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);

float flat, flon;
unsigned long age;
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

int FingerMode = 0 ;
int SkipHead = 0;
int State1=0;
int State2=0;


// implement a notification class,
// its member methods will get called 
//
class Mp3Notify
{
public:
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }

  static void OnPlayFinished(uint16_t globalTrack)
  {
    Serial.println();
    Serial.print("Play finished for #");
    Serial.println(globalTrack);   
  }

  static void OnCardOnline(uint16_t code)
  {
    Serial.println();
    Serial.print("Card online ");
    Serial.println(code);     
  }

  static void OnCardInserted(uint16_t code)
  {
    Serial.println();
    Serial.print("Card inserted ");
    Serial.println(code); 
  }

  static void OnCardRemoved(uint16_t code)
  {
    Serial.println();
    Serial.print("Card removed ");
    Serial.println(code);  
  }
};

// instance a DFMiniMp3 object, 
// defined with the above notification class and the hardware serial class
//
DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial3);

// Some arduino boards only have one hardware serial port, so a software serial port is needed instead.
// comment out the above definition and uncomment these lines
SoftwareSerial secondarySerial(A10, A11); // RX, TX
//DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(&Serial3);

void(* resetFunc) (void) = 0;//declare reset function at address 0
uint8_t readnumber(void) {
  uint8_t num = 0;
  boolean validnum = false; 
  while (1) {
   // while (! Serial.available());
    char    keypressed = myKeypad.getKey();
    char c = keypressed;//Serial.read();
    if (isdigit(c)) {
       num *= 10;
       num += c - '0';
       validnum = true;
    } else if (validnum) {
      return num;
    }
  }
}
void setup()
{
Serial.begin(9600);//GPS
delay(300);
Serial2.begin(9600);//SMS
delay(300);
  

uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(BLACK);

tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 0);
tft.println("SELAMAT DATANG ");

 // set the data rate for the sensor serial port
  finger.begin(57600); //serial
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }


  
}

void loop()
{
  if (armed){
    systemIsArmed();  //Run function to activate the system
  }
  else if (!armed){
    systemIsUnarmed();  //Run fuction to de activate the system
  }
}

/********************************FUNCTIONS************************************/

//While system is unarmed
void systemIsUnarmed(){
  int screenMsg=0;
  //lcd.clear();                  //Clear lcd
  tft.fillScreen(BLACK);
  unsigned long previousMillis = 0;           //To make a delay by using millis() function
  const long interval = 5000;           //delay will be 5 sec. 
                          //every "page"-msg of lcd will change every 5 sec
  while(!armed){                  //While system is unarmed do...
    unsigned long currentMillis = millis();   //Store the current run-time of the system (millis function)
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        if(screenMsg==0){           //First page-message of lcd
//        lcd.setCursor(0,0);
//        lcd.print("SYSTEM ALARM OFF");
//        lcd.setCursor(0,1);
//        lcd.print("----------------");
  
tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 0);
tft.println("SYSTEM OFF");
        screenMsg=1;
        }
        else{                 //Second page-message of lcd
//          lcd.setCursor(0,0);
//          lcd.print("* to arm        ");
//          lcd.setCursor(0,1);
//        lcd.print("# to change pass");

tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 20);
tft.println("A u/ absen ");
tft.setCursor(0, 40);
tft.println("B u/ regis ID  ");
tft.setCursor(0, 60);
tft.println("C u/ delete ID  ");
        screenMsg=0;
        }
      }
      char    keypressed = myKeypad.getKey();       //Read the pressed button

    if (keypressed =='A'){            //If A is pressed, activate the system
//      /NewTone(buzzer,500,200);
tft.setTextColor(BLUE);  tft.setTextSize(2);
    tft.setCursor(0, 60);
tft.println("A di pencet ");
      systemIsArmed();            //by calling the systemIsArmed function
    }
    
    else if (keypressed =='B'){//If B is pressed, change current password
      keypressed = myKeypad.getKey(); 
       Serial.println("Ready to enroll a fingerprint! Please Type in the ID # you want to save this finger as...");
  id = readnumber();
  Serial.print("Enrolling ID #");
  Serial.println(id);
  //char    keypressed = myKeypad.getKey();  1

  while (!  getFingerprintEnroll() ); 

    }
    else if (keypressed =='C'){//If B is pressed, change current password
      keypressed = myKeypad.getKey(); 
       Serial.println("Ready to enroll a fingerprint! Please Type in the ID # you want to save this finger as...");
  id = readnumber();
  Serial.print("Enrolling ID #");
  Serial.println(id);
  //char    keypressed = myKeypad.getKey();  1

deleteFingerprint(id);

    }
  }
}

//While system is armed
void systemIsArmed(){     
  tft.fillScreen(BLACK);          
//  lcd.clear();
  int count=10;               //Count 10sec before activate the system
  unsigned long previousMillis = 0;         
  const long interval = 1000; 
  while(!armed){    
   //While system is unarmed - for 10sed do...
//    lcd.setCursor(0,0);
//    lcd.print(" SYSTEM WILL BE ");      //Print message to lcd with 10 sec timer
//    lcd.setCursor(0,1);
//    lcd.print("   ARMED IN ");
  tft.fillScreen(BLACK);      
tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 0);
tft.println(" SYSTEM WILL BE ");
tft.setCursor(0, 10);
tft.println("   standby IN ");

   unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        //Screen counter 10sec
        if (count>1){
        count--;            //Countdown timer
        }
        else{
          armed=true;           //Activate the system!
        break;
        }
      }
//    lcd.setCursor(12,1);
//    lcd.print(count);           //show the timer at lcd second line 13 possition 
tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 30);
tft.println(count);
 
  }
  while (armed){                //While system is armed do...
//    lcd.setCursor(0,0);
//    lcd.print("SYSTEM IS ARMED!");
//    lcd.setCursor(0,1);
//    lcd.print("----------------");

      
tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 30);
tft.println("Silahkan Absen");

tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 50);
tft.println("Tekan * jika telah absen");
char    keypressed = myKeypad.getKey();       //Read the pressed button
for (unsigned long start = millis(); millis() - start < 1000;) {
    while (Serial.available()) {
      char c = Serial.read();
      //       Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      gps.encode(c); // Did a new valid sentence come in?

    }
  }
gps.f_get_position(&flat, &flon, &age);


  tft.setCursor(0, 100);
  tft.println("LAT :");
  tft.setCursor(50, 100);
  tft.println(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
  tft.setCursor(0, 120);
  tft.println("LONG :");
  tft.setCursor(60, 120);
  tft.println(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);

getFingerprintIDez();
delay(50);
    if (finger.fingerID<=0){ //finger tidak terdaftar
    mp3.playMp3FolderTrack(2);  // sd:/mp3/0001.mp3
    }

tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 150);
tft.println("Tekan B u/ reg jari");
  


if (keypressed=='*'){
  resetFunc(); //call reset 
  }
  
  
}}


uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
   // IF FOUND A MATCH............
  tft.fillScreen(BLACK);
tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 80);
tft.println("Found ID :");
tft.setCursor(120, 80);
tft.println(finger.fingerID);
 mp3.playMp3FolderTrack(1);  // sd:/mp3/0001.mp3
for (unsigned long start = millis(); millis() - start < 1000;) {
    while (Serial.available()) {
      char c = Serial.read();
      //       Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      gps.encode(c); // Did a new valid sentence come in?

    }
  }
  gps.f_get_position(&flat, &flon, &age);
  Serial.print("LAT=");
  Serial.println(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
  Serial.print(" LON=");
  Serial.println(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
  Serial.print(" Speed=");
  Serial.println(gps.f_speed_kmph());

  tft.setCursor(0, 20);
  tft.println("LAT :");
  tft.setCursor(20, 20);
  tft.println(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
  tft.setCursor(0, 30);
  tft.println("LONG :");
  tft.setCursor(30, 30);
  tft.println(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
  //display.display();
  Serial2.println("AT+CMGF=1");
  delay(2000);
  Serial2.print("AT+CMGS=\"");
  Serial2.print(phone_no);
  Serial2.write(0x22);
  Serial2.write(0x0D);  // hex equivalent of Carraige return
  Serial2.write(0x0A);  // hex equivalent of newline
  delay(2000);
  Serial2.println("Absen Berhasil!");
  Serial2.print("https://maps.google.com/maps?q=loc:");
  Serial2.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
  Serial2.print(",");
  Serial2.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
  delay(500);
  Serial2.println (char(26));//the ASCII code of the ctrl+z is 26



  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 


}


uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 170);
tft.println("finger to enroll as #");
  tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 190);
tft.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
   tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 200);
tft.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
     tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 220);
tft.println("ID");
   tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(40, 220);
tft.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
     tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 240);
tft.println("sukses finger");
   tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(100, 240);
tft.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
     tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 240);
tft.println("ID finger");
   tft.setTextColor(BLUE);  tft.setTextSize(2);
tft.setCursor(0, 240);
tft.println(id);

  
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    return p;
  }   
}
