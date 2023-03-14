#include <Keypad.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);  

Servo myservo; 

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
char password[3] = {'1','2','3'};
char resetPass[4] = {'A', 'B', 'C', 'D'};
int unlocked = 0; //0 = door is locked, 1 = door is unlocked
int i = 0;
int k = 0;
int incorect_pass_count = 0; //contor pentru a numara de cate ori s-a introdus parola gresit
int Green = 12;
int Red = 13;
const int buzzerPin = 11;
char pressedKey;
int wrong = 0; // 1 = parola a fost introdusa gresit
int wrongReset = 0; // 1 = codul de resetare a fost introdus gresit
char Incoming_value = 0; // variabile pentru citirea semnalului bluetooth
int flag = 0;
int flag2 = 0;

void setup(){
  Serial.begin(9600);
  myservo.attach(10);
  myservo.write(15);
  lcd.backlight();
  lcd.init(); 
  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("Door is locked");
  lcd.setCursor(0,1);
  lcd.print("Enter password");
  pinMode(Green, OUTPUT);
  pinMode(Red, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}
  
void loop(){
  digitalWrite(Red, LOW);
  digitalWrite(Green, LOW);
  pressedKey = customKeypad.getKey();
  //door is locked
  if(unlocked == 0 && incorect_pass_count < 3){
    if (pressedKey != NO_KEY && i<3) {
        if(pressedKey != password[i])
          wrong = 1; // wrong password
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Password loading");
        lcd.setCursor(4, 1);
        for(int j=0; j<=i; j++) 
          {lcd.print("*");}
        delay(1000);
        i=i+1;
     }    
    if(i == 3){
      if(wrong == 0){
        unlocked = 1; //door is unlocked
        flag = 1;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Corect password");
        lcd.setCursor(0,1);
        lcd.print("Door unlocked");
        digitalWrite(Green, HIGH);
        delay(3000);
        myservo.write(50); 
        i = 0;
       }
       else if (wrong == 1 && incorect_pass_count == 0){
        incorect_pass_count = incorect_pass_count+1;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Wrong password");
        lcd.setCursor(0,1);
        lcd.print("2 attempts left");
        digitalWrite(Red, HIGH);
        delay(3000);
        wrong = 0;
        i = 0;
       }
       else if (wrong == 1 && incorect_pass_count == 1){
        incorect_pass_count = incorect_pass_count+1;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Wrong password");
        lcd.setCursor(0,1);
        lcd.print("1 attempt left");
        digitalWrite(Red, HIGH);
        delay(3000);
        wrong = 0;
        i = 0;
       }
       else if (wrong == 1 && incorect_pass_count == 2){
        incorect_pass_count = incorect_pass_count+1;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Wrong password");
        lcd.setCursor(0,1);
        lcd.print("Alarm ON");
        tone(buzzerPin, 2000);
        digitalWrite(Red, HIGH);
        delay(3000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Enter reset code");
        delay(3000);
        flag2 = 1;
       }
    }
    if(Serial.available()>0){
      Incoming_value = Serial.read();
      Serial.print("\n");
      if(Incoming_value == '0'){
        unlocked = 1; //door is unlocked
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Corect password");
        lcd.setCursor(0,1);
        lcd.print("Door unlocked");
        digitalWrite(Green, HIGH);
        delay(3000);
        myservo.write(50); 
        i = 0;
      }
    }
  }
  //door is locked and password was incorrect 3 times
  if(incorect_pass_count == 3 && flag2 == 0){
     if (pressedKey != NO_KEY && k<4) {
        if(pressedKey != resetPass[k])
          wrongReset = 1;
        lcd.setCursor(4, 1);
        for(int j=0; j<=k; j++) 
          lcd.print("*");
        delay(1000);
      k++;
    }
    if(Serial.available()>0){
      Incoming_value = Serial.read();
      Serial.print("\n");
      if(Incoming_value == '2'){
        lcd.clear();
        lcd.setCursor(0,0);
        noTone(buzzerPin);
        lcd.print("Alarm OFF");
        delay(3000);
        lcd.clear();
        lcd.setCursor(0, 0); 
        lcd.print("Door is locked");
        lcd.setCursor(0,1);
        lcd.print("Enter password");
        i = 0;
        wrong = 0;
        k = 0;
        incorect_pass_count = 0;
      }
    }
    if(k==4 && wrongReset == 0)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        noTone(buzzerPin);
        lcd.print("Alarm OFF");
        delay(3000);
        lcd.clear();
        lcd.setCursor(0, 0); 
        lcd.print("Door is locked");
        lcd.setCursor(0,1);
        lcd.print("Enter password");
        i = 0;
        wrong = 0;
        k = 0;
        incorect_pass_count = 0;
       } 
       else if(k==4 && wrongReset == 1){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Wrong reset code");
        digitalWrite(Red, HIGH);
        delay(3000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Enter reset code");
        k = 0;
        wrongReset = 0;
       }
  }
  //door is unlocked
  if(unlocked == 1){ 
    if (pressedKey != NO_KEY && i<3 && flag == 0) {
      if(pressedKey != password[i])
          wrong = 1; // wrong password
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Password loading");
      lcd.setCursor(4, 1);
      for(int j=0; j<=i; j++) 
        lcd.print("*");
      i=i+1;
      delay(1000);
    }
    if(i == 3){
      if(wrong == 0){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Corect password");
        lcd.setCursor(0,1);
        lcd.print("Door locked");
        digitalWrite(Green, HIGH);
        delay(3000);
        myservo.write(15); 
        i = 0;
        unlocked = 0; //door is locked
        incorect_pass_count = 0;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Door is locked");
        lcd.setCursor(0,1);
        lcd.print("Enter password");
       }
       else if (wrong == 1){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Wrong password");
        lcd.setCursor(0,1);
        lcd.print("Door unlocked");
        digitalWrite(Red, HIGH);
        delay(3000);
        wrong = 0;
        i = 0;
       } 
    }
    if(Serial.available()>0){
      Incoming_value = Serial.read();
      Serial.print("\n");
      if(Incoming_value == '1'){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Corect password");
        lcd.setCursor(0,1);
        lcd.print("Door locked");
        digitalWrite(Green, HIGH);
        delay(3000);
        myservo.write(15); 
        i = 0;
        unlocked = 0; //door is locked
        incorect_pass_count = 0;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Door is locked");
        lcd.setCursor(0,1);
        lcd.print("Enter password");
      }
    }
  }
  flag = 0;
  flag2 = 0;
}
