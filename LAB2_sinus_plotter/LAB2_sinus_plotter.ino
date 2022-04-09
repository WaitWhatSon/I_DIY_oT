float x = 0;
float delta = 0.0873; // około 5 radianów

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  Serial.print("sin(x): "); 
  Serial.print(sin(x)); 
  Serial.print(",");
  Serial.println();
  x += delta;
  delay(100);
}
