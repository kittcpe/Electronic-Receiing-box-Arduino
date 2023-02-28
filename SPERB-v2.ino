// PROJECT TITLE: SOLAR POWERED ELECTRONIC RECIEVING BOX

//LIBRARIES
#include <DFRobot_HX711.h> //weight
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>


//Definition
  //Weight Sensor Door1
  #define WeightSensorA A2
  #define WeightSensorB A3
  //Weight Sensor Door2
  #define WeightSensorC A4
  #define WeightSensorD A5
  
  //Door 1
  #define Relay1 22
  #define Relay2 24

  //Door 2
  #define Relay3 30
  #define Relay4 32

  //Cashbox 1
  #define Relay5 26
  #define Relay6 28

  //Cashbox 2
  #define Relay7 34
  #define Relay8 36
  
//Configurations
LiquidCrystal_I2C lcd(0x21,16,2); // address (0x21), 16cols, 2 rows
//Weight Sensor 1
DFRobot_HX711 MyScale1(WeightSensorA, WeightSensorB);
//Weight Sensor 2
DFRobot_HX711 MyScale2(WeightSensorC, WeightSensorD);
SoftwareSerial mySerial(9, 10);
//Keypad
const byte row = 4;
const byte col = 4;
  //keypad config
char keys[row][col] = {  {'1','2','3','A'},
                         {'4','5','6','B'},
                         {'7','8','9','C'},
                         {'*','0','#','D'} };

byte RPins[row] = { 10,9,8,7 };
byte CPins[col] = { 6,5,4,3 }; 

Keypad kpd = Keypad( makeKeymap(keys),
                     RPins,
                     CPins,
                     row,
                     col );  

//Decisional Variables
bool Box1_isNotOccupied;
bool Box2_isNotOccupied;
bool AllBoxOccupied;
bool password_iscorrect;
String Setpassword = "123456";
String Inputpassword;
String val;
int PswdCount;
int i;
void setup() {
  //PinModes
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
  pinMode(Relay5, OUTPUT);
  pinMode(Relay6, OUTPUT);
  pinMode(Relay7, OUTPUT);
  pinMode(Relay8, OUTPUT);

  //Periphirals
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

   // Set the calibration +
  MyScale1.setCalibration(1900);
  MyScale2.setCalibration(1900);   
  Box1_isNotOccupied = true;
  Box2_isNotOccupied = true; 
  AllBoxOccupied = false;

  //Program Introduction
  lcd.setCursor(0,0);
  lcd.print("Solar Powered Electronic");
  lcd.setCursor(0, 1);
  lcd.print("Electronic Receiving Box");
  delay(5000);
  lcd.clear();

  //password configuration 
  // Setpassword = "";
  Inputpassword = "";  
  PswdCount = 6;
  password_iscorrect = false;
  lcd.clear();
  //Rest State
    //Doors
  Door1_Close();
  Door2_Close();
  Cashbox1_Close();
  Cashbox2_Close();


  CheckBoxes();
}

void loop() {
  if(!AllBoxOccupied)
  {
  while(!password_iscorrect)     
  {
    SendMessage();
    InputPassword();
  }
  if(password_iscorrect)
  {
    if(Box1_isNotOccupied)
    {
      Box1_Recieve_Item();      
    }
    else
    {
      if(Box2_isNotOccupied)
      {
      Box2_Recieve_Item(); 
      }  
      else
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("FULL");
      }      
    }
  }
  }
  else
  {
         lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("ALL BOXES");
          lcd.setCursor(1, 0);
          lcd.print("ARE FULL");
  }       
      
}
void CheckBoxes(){
  //Check if box is occupied
  CheckWeigh1();
  CheckWeigh2(); 
    
}
 void SendMessage()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+9816653039\"\r"); // Replace x with mobile number
  delay(1000);
  mySerial.println(String("Password:") + Setpassword);// The SMS text you want to send
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);  
}

void CheckWeigh1(){
  int weight1 = MyScale1.readWeight();
  Serial.println(String("W1:") + weight1);
  if(weight1 >30){Box1_isNotOccupied = false;}
  else{Box1_isNotOccupied = true;}
}
void CheckWeigh2(){
  int weight2 = MyScale2.readWeight();
  Serial.println(String("W2:") +weight2);
  if(weight2>30){Box2_isNotOccupied = false;}
  else{Box2_isNotOccupied = true;}
}
void InputPassword(){

  lcd.setCursor(0, 0);
  lcd.print("Input Password");
  char key = kpd.getKey();
  if(key != NO_KEY)
  {
  Inputpassword += key;
  i++;  
    if(i== (PswdCount))
    {
      if(Inputpassword == Setpassword)
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Password Correct");
            password_iscorrect = true;           
        }
        else
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Password Incorrect");
            password_iscorrect = false;
            delay(3000);
            lcd.clear();Inputpassword ="";i=0;            
          
        }              
    }
    if(key == '#')
    {lcd.clear();Inputpassword ="";i=0;}
  lcd.setCursor(1, i);
  Serial.println(Inputpassword);
  lcd.print(Inputpassword);  

  }
    
  }
  
void Box1_Recieve_Item(){

    lcd.clear();    
    lcd.setCursor(0, 0);
    lcd.print("Door 1 Opening");
    Door1_Open();
    CheckWeigh1();
    delay(8000);
if(!Box1_isNotOccupied)
  {
    delay(3000);
    lcd.clear();    
    lcd.setCursor(0, 0);
    lcd.print("Door 1 Closing");
    Door1_Close();
    lcd.clear();    
    lcd.setCursor(0, 0);
    delay(3000);
    lcd.print("Cashbox 1 Opening");
    Cashbox1_Open();
    delay(8000);       
    lcd.clear();    
    lcd.setCursor(0, 0);
    lcd.print("Cashbox 1 Closing");  
    Cashbox1_Close();
    delay(2000);
    lcd.clear();   
    password_iscorrect = false;
    Inputpassword ="";i=0;    
  } 
}
void Box2_Recieve_Item(){
 
    lcd.clear();    
    lcd.setCursor(0, 0);
    lcd.print("Door 2 Opening");
    Door2_Open();
    CheckWeigh2();
    delay(8000);
if(!Box2_isNotOccupied)
  {
    delay(3000);
    lcd.clear();    
    lcd.setCursor(0, 0);
    lcd.print("Door 2 Closing");
    Door2_Close();
    lcd.clear();    
    lcd.setCursor(0, 0);
    delay(3000);
    lcd.print("Cashbox 2 Opening");
    Cashbox2_Open();
    delay(8000);       
    lcd.clear();    
    lcd.setCursor(0, 0);
    lcd.print("Cashbox 2 Closing");  
    Cashbox2_Close();
    delay(2000);
    lcd.clear();   
    password_iscorrect = false;
    Inputpassword ="";i=0;    
  }  
}
//Door and Cashbox Open and Close

//Doors Open
void Door1_Open(){
      digitalWrite(Relay1, LOW);
       digitalWrite(Relay2, HIGH);
}
void Door2_Open(){
      digitalWrite(Relay3, LOW);
       digitalWrite(Relay4, HIGH);
}
//Doors Close
void Door1_Close(){
      digitalWrite(Relay1, HIGH);
       digitalWrite(Relay2, LOW);
}
void Door2_Close(){
      digitalWrite(Relay3, HIGH);
       digitalWrite(Relay4, LOW);
}

//Cashbox Open
void Cashbox1_Open(){
      digitalWrite(Relay5, HIGH);
       digitalWrite(Relay6, LOW);
}
void Cashbox2_Open(){
      digitalWrite(Relay7, HIGH);
       digitalWrite(Relay8, LOW);
}
//Cashbox Close
void Cashbox1_Close(){
      digitalWrite(Relay5, LOW);
       digitalWrite(Relay6, HIGH);
}
void Cashbox2_Close(){
      digitalWrite(Relay7, LOW);
       digitalWrite(Relay8, HIGH);
}

