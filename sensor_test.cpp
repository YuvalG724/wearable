#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SDA_PIN 21
#define SCL_PIN 22

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

#define pulsepin 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

volatile int bpm = 0;

uint8_t mpuAddress = 0;
bool mpuAvailable = false;
float accelX = 0;
float accelY = 0;
float accelZ = 0;
float gyroX = 0;
float gyroY = 0;
float gyroZ = 0;

// Step detection
int stepCount = 0;
unsigned long lastStepTime = 0;
float stepThreshold = 3.0;

bool i2cProbe(uint8_t address)
{
    Wire.beginTransmission(address);
    return Wire.endTransmission() == 0;
}

bool mpuWriteRegister(uint8_t address, uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(value);
    return Wire.endTransmission() == 0;
}

bool mpuReadRegisters(uint8_t address, uint8_t reg, uint8_t count, uint8_t *buffer)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0)
        return false;

    if (Wire.requestFrom(address, count) != count)
        return false;

    for (uint8_t i = 0; i < count; i++)
        buffer[i] = Wire.read();

    return true;
}

int16_t read16(const uint8_t *buffer, uint8_t index)
{
    return (int16_t)((buffer[index] << 8) | buffer[index + 1]);
}

void scanI2C()
{
    Serial.println("Scanning I2C bus...");
    for (uint8_t addr = 1; addr < 127; addr++)
    {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0)
        {
            Serial.print("Found I2C device at 0x");
            Serial.println(addr, HEX);
        }
    }
    Serial.println("I2C scan complete.");
}

void initMPU()
{
    if (i2cProbe(0x68))
    {
        Serial.println("MPU6050 detected at 0x68");
        mpuAddress = 0x68;
        mpuAvailable = true;
    }
    else if (i2cProbe(0x69))
    {
        Serial.println("MPU6050 detected at 0x69");
        mpuAddress = 0x69;
        mpuAvailable = true;
    }
    else
    {
        Serial.println("MPU6050 not found at 0x68 or 0x69");
        mpuAvailable = false;
    }

    if (mpuAvailable)
    {
        mpuWriteRegister(mpuAddress, 0x6B, 0x00);
        mpuWriteRegister(mpuAddress, 0x1B, 0x08);
        mpuWriteRegister(mpuAddress, 0x1C, 0x10);
        Serial.println("MPU6050 configured.");
    }
}

void setup()
{
    Serial.begin(115200);

    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);
    delay(1000);
    scanI2C();

    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
    {
        Serial.println("OLED not found");
        while (1)
            ;
    }

    initMPU();
}

void loop()
{
    int sensorValue = analogRead(pulsepin);
    int newbpm = map(sensorValue, 1700, 4096, 60, 180);
    unsigned long now = millis();

    if (mpuAvailable)
    {
        uint8_t buffer[14];
        if (mpuReadRegisters(mpuAddress, 0x3B, 14, buffer))
        {
            int16_t rawAx = read16(buffer, 0);
            int16_t rawAy = read16(buffer, 2);
            int16_t rawAz = read16(buffer, 4);
            int16_t rawGx = read16(buffer, 8);
            int16_t rawGy = read16(buffer, 10);
            int16_t rawGz = read16(buffer, 12);

            accelX = rawAx / 4096.0;
            accelY = rawAy / 4096.0;
            accelZ = rawAz / 4096.0;
            gyroX = rawGx / 65.5;
            gyroY = rawGy / 65.5;
            gyroZ = rawGz / 65.5;

            // Step detection using magnitude
            float magnitude = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);
            if (magnitude > stepThreshold && (now - lastStepTime) > 1000)
            {
                stepCount++;
                lastStepTime = now;
            }
        }
        else
        {
            // Retry instead of permanently disabling
            mpuAvailable = false;
            initMPU();
        }
    }
    else
    {
        static unsigned long lastRetry = 0;
        if ((now - lastRetry) > 2000)
        {
            lastRetry = now;
            initMPU();
        }
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("BPM: ");
    display.println(newbpm);
    display.setCursor(0, 10);
    display.print("Ax:");
    display.print(accelX, 2);
    display.print(" Ay:");
    display.println(accelY, 2);
    display.setCursor(0, 20);
    display.print("Az:");
    display.print(accelZ, 2);
    display.print(" Gx:");
    display.println(gyroX, 1);
    display.setCursor(0, 30);
    display.print("Gy:");
    display.print(gyroY, 1);
    display.print(" Gz:");
    display.println(gyroZ, 1);
    display.setCursor(0, 40);
    display.print("MPU: ");
    display.println(mpuAvailable ? "OK" : "N/A");
    display.setCursor(0, 50);
    display.print("Steps: ");
    display.println(stepCount);
    Serial.println("BPM: ");
    Serial.println(newbpm);
    Serial.println("Ax: ");
    Serial.println(accelX, 2);
    Serial.println("Ay: ");
    Serial.println(accelY, 2);
    Serial.println("accelz: ");
    Serial.println(accelZ);
    display.display();

    delay(100);
}