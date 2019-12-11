#include<Servo.h>
Servo base;
Servo shld;
Servo elbw;
const int ledPin = 13; // the pin that the LED is attached to
const byte buffSize = 40;
unsigned int inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;
byte coordinates[3];
int zerobase = 90, zeroshld = 80, zeroelbw = 10;
int angbase = 75, angshld = 60, angeblw = 50;
int sqrbase = 170, tribase = 10, drpshld = 75, drpelbw = 40;

void setup() {
  // put your setup code here, to run once:
  //=====operational instructions=====
  //light the holy incense
  //speak the operational incantations
  //give praise unto the omnissiah
  //===operational instructions end===
  Serial.begin (115200);
  pinMode(ledPin, OUTPUT);
  base.attach (2); //vertical base rotating link - 55 high position, 140 low position
  shld.attach (3); //vertical intermediate rotating link - 170 fully extended, 50 fully retracted
  elbw.attach (4); //right eject 35, left eject 165,
  base.write(zerobase);
  shld.write(zeroshld);
  elbw.write(zeroelbw);
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  getDataFromPC();
  
  if(newDataFromPC){
    sendSuspendCmd();
    digitalWrite(ledPin, HIGH);
    delay(5000);
    digitalWrite(ledPin, LOW);
    sendEnableCmd();
    sendCoordinatesToPC();
    //calculate the required angles (the hard part?)
    
    //for-loops used to slow the arm's movement
    //begin moving arm to location (for-loop moving the angle from neutral to the pick-up)
    if(zerobase < angbase){
    for(int i = zerobase; i < angbase; i++){//move base first, to prevent the arm hitting things
      base.write(i);
      delay(100);
    }
    }
    else{
    for(int i = zerobase; i > angbase; i--){
      base.write(i);
      delay(100);
    }
    }
    int angelbw = 50;//no idea why this is here...
    if(zeroelbw < angelbw){
    for(int i = zeroelbw; i < angelbw; i++){//then elbow, to prevent scraping the ground
      elbw.write(i);
      delay(100);
    }
    }
    else{
    for(int i = zeroelbw; i > angelbw; i--){//then elbow, to prevent scraping the ground
      elbw.write(i);
      delay(100);
    }
    }
    if(zeroshld < angshld){
    for(int i = zeroshld; i < angshld; i++){//and finally shoulder, to lower it down
      shld.write(i);
      delay(100);
    }
    }
    else{
    for(int i = zeroshld; i > angshld; i--){//and finally shoulder, to lower it down
      shld.write(i);
      delay(100);
    }
    }
    //initiate pickup (enable magnet and lower, then pray it hits the shape...)
    for (int i = angshld; i > angshld - 5; i--){
      shld.write(i);
      delay(100);
    }
    digitalWrite(ledPin, HIGH);
    delay(2000); //====================="pick-up"=====================
    digitalWrite(ledPin, LOW);
    for (int i = angshld - 5; i < angshld; i++){
      shld.write(i);
      delay(100);
    }
    //initiate drop off (simple for loop starting at pick-up angle, and ending at the drop-off)
    if(angshld < drpshld){
    for(int i = angshld; i < drpshld; i++){//and finally shoulder, to lower it down
      shld.write(i);
      delay(100);
    }
    }
    else{
    for(int i = angshld; i > drpshld; i--){//and finally shoulder, to lower it down
      shld.write(i);
      delay(100);
    }
    }
    if(angelbw < drpelbw){
    for(int i = angelbw; i < drpelbw; i++){//then elbow, to prevent scraping the ground
      elbw.write(i);
      delay(100);
    }
    }
    else{
    for(int i = angelbw; i > drpelbw; i--){//then elbow, to prevent scraping the ground
      elbw.write(i);
      delay(100);
    }
    }
    if (coordinates[3] == 1){//square ********************remember to input shape data********************
      if(angbase < sqrbase){
      for (int i = angbase; i < sqrbase; i++){
        shld.write(i);
        delay(100);
      }
      }
      else{
      for (int i = angbase; i > sqrbase; i--){
        shld.write(i);
        delay(100);
      }
      }
    }
    else{//triangle
      if(angbase < tribase){
      for (int i = angbase; i < tribase; i++){
        shld.write(i);
        delay(100);
      }
      }
      else{
      for (int i = angbase; i > tribase; i--){
        shld.write(i);
        delay(100);
      }
      }
    }
    //assuming drop off successful, return to standard position
    
    if (coordinates[3] == 1){//square
      if(sqrbase < zerobase){
      for (int i = sqrbase; i < zerobase; i++){
        shld.write(i);
        delay(100);
      }
      }
      else{
      for (int i = sqrbase; i > zerobase; i--){
        shld.write(i);
        delay(100);
      }
      }
    }
    else{//triangle
      if(tribase < zerobase){
      for (int i = tribase; i < zerobase; i++){
        shld.write(i);
        delay(100);
      }
      }
      else{
      for (int i = tribase; i > zerobase; i--){
        shld.write(i);
        delay(100);
      }
      }
    }
    newDataFromPC = false;
  }
  /*
  base.write(170);
  delay(1000);
  elbw.write(45);
  delay(1000);
  shld.write(45);
  delay(3000);
  shld.write(80);
  delay(1000);
  elbw.write(10);
  delay(1000);
  base.write(90);
  delay(3000);
 if (Serial.available() > 0) {
    incomingByte = Serial.read();
    Serial.println(incomingByte);
  }
  */
}

void sendSuspendCmd(){
  // send the suspend-true command
  Serial.println("<S1>");
}

void sendEnableCmd(){
  // send the suspend-false command
  Serial.println("<S0>");
}

void sendCoordinatesToPC(){
  // send the point data to the PC
  Serial.print("<P");
  Serial.print(coordinates[0]);
  Serial.print(",");
  Serial.print(coordinates[1]);
  Serial.println(">");
}

// alternative to the readBytes function:
void getDataFromPC() {
  // receive data from PC and save it into inputBuffer
  if(Serial.available() > 0) {
    char x = Serial.read();
    // the order of these IF clauses is significant
    if (x == endMarker) {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      coordinates[0] = inputBuffer[0];
      coordinates[1] = inputBuffer[1];
    }
    if(readInProgress) {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }
    if (x == startMarker) {
    bytesRecvd = 0;
    readInProgress = true;
    }
  }
}
