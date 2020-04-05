void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(9, HIGH);  
  digitalWrite(22, HIGH);
}

void loop() {
  
//  Serial.print("Main Pot ->");  
//  Serial.println(analogRead(A2));
//  Serial.print("Octave Pot ->");
//  Serial.println(analogRead(A3));
//  Serial.print("Strum Pot ->");  
//  Serial.println(analogRead(A10));
//  Serial.print("Neck Pot ->");
//  Serial.println(analogRead(A11));
//  Serial.print("Joy X Pot ->");  
//  Serial.println(analogRead(A6));
//  Serial.print("Joy Y Pot ->");
//  Serial.println(analogRead(A7));
//   Serial.print("Joystick Button ->");
//   Serial.println(digitalRead(9));
  Serial.print("Main Knob Value ->");
  Serial.println(analogRead(A4));
  Serial.print("Knob 1 Value ->");
  Serial.println(analogRead(A14));
  Serial.print("Knob 2 Value ->");
  Serial.println(analogRead(A15));
//  Serial.println(digitalRead(6));
//  Serial.println(digitalRead(7));
//  Serial.println(digitalRead(8)) ;

  delay(300);
  // put your main code here, to run repeatedly:

}
