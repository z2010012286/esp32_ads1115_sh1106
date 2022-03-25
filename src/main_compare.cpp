#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include <SH1106Wire.h>

Adafruit_ADS1115 ads; /* Use this for the 16-bit version */
// Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */

SH1106Wire display(0x3c, SDA, SCL); // ADDRESS, SDA, SCL
int counter = 1;

// moving filter init
int i = 0;
const int N = 10;
float ADC0[N];
float ADC1[N];
float ADC2[N];
float ADC3[N];

void displayOLED(int id, float value)
{
  switch (id)
  {
  case (1):
    display.drawString(0, 0, "v");
    display.drawString(5, 0, String(id));
    display.drawString(10, 0, ":");
    display.drawString(15, 0, String(value, 3));
    break;

  case (2):
    display.drawString(66, 0, "v");
    display.drawString(71, 0, String(id));
    display.drawString(76, 0, ":");
    display.drawString(81, 0, String(value, 3));
    break;

  case (3):
    display.drawString(0, 10, "v");
    display.drawString(5, 10, String(id));
    display.drawString(10, 10, ":");
    display.drawString(15, 10, String(value, 3));
    break;

  case (4):
    display.drawString(66, 10, "v");
    display.drawString(71, 10, String(id));
    display.drawString(75, 10, ":");
    display.drawString(81, 10, String(value, 3));
    break;

  default:
    break;
  }
}

float mean(float p[], int len)
{
  int j;
  float sum = 0, mean1 = 0;
  for (j = 0; j < len; j++)
  {
    sum = sum + p[j];
  }
  mean1 = sum / len;
  Serial.println(len);
  Serial.println(mean1);
  return mean1;
}

void setup(void)
{
  Serial.begin(115200);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  Serial.println("Hello! Tianci, Get starting...");

  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.setGain(GAIN_ONE); // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  if (!ads.begin())
  {
    Serial.println("Failed to initialize ADS1115.");
    while (1)
      ;
  }
}

void loop(void)
{

  int16_t adc0, adc1, adc2, adc3;
  float volts0, volts1, volts2, volts3;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);

  volts0 = ads.computeVolts(adc0);
  volts1 = ads.computeVolts(adc1);
  volts2 = ads.computeVolts(adc2);
  volts3 = ads.computeVolts(adc3);

  // moving filter
  ADC0[i] = volts0;
  ADC1[i] = volts1;
  ADC2[i] = volts2;
  ADC3[i] = volts3;

  float v0 = mean(ADC0, N);
  float v1 = mean(ADC1, N);
  float v2 = mean(ADC2, N);
  float v3 = mean(ADC3, N);

  // clear the display

  display.clear();
  // draw the current demo method
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  displayOLED(1, v0);
  displayOLED(2, v1);
  displayOLED(3, v2);
  displayOLED(4, v3);

  display.drawString(0, 30, "v0:");
  display.drawString(15, 30, String(volts0, 3));

  // display.setFont(ArialMT_Plain_10);
  // display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 48, "By ZTC");
  display.drawString(0, 38, "with moving filter:");
  display.drawString(85, 38, String(N));

  // display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 48, String(millis()));
  // write the buffer to the display
  display.display();

  counter++;
  i++;
  if (i >= N)
    i = 0;
  delay(10);
}
