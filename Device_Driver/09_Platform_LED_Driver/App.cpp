#include <fstream>
#include <iostream>

#define LOW  "0"
#define HIGH "1"

class Basic_Character_Devices
{
    private:
        const std::string PATH;
        std::fstream fileDescriptor;

    public:
        void WriteFile (std::string message);
        std::string ReadFile (void);
        Basic_Character_Devices(std::string devicePath);
};

Basic_Character_Devices::Basic_Character_Devices(std::string devicePath):PATH(devicePath){}

void Basic_Character_Devices::WriteFile(std::string message)
{
    fileDescriptor.open(PATH, std::ios::out);

    message = message + "\n";

    fileDescriptor.write(message.c_str(), message.size());

    fileDescriptor.close();
}

std::string Basic_Character_Devices::ReadFile(void)
{
    std::string result;

    fileDescriptor.open(PATH);

    std::getline (fileDescriptor, result);

    fileDescriptor.close();

    return result;
}

int main ()
{
    // 1- make an object for each LED
    Basic_Character_Devices LED_0("/dev/LED_0"); //RDWR
    Basic_Character_Devices LED_1("/dev/LED_1"); //RDWR
    Basic_Character_Devices LED_2("/dev/LED_2"); //RDWR

    // 2- Initialize all LEDs to be LOW
    LED_0.WriteFile(LOW);
    LED_1.WriteFile(LOW);
    LED_2.WriteFile(LOW);

    // 3- control the LEDs based on the intensity level and the current level
    int intensityLevel;

    std::string currentLevel;

    int travelFlag = 0;
    for (intensityLevel = 1; intensityLevel <= 3; intensityLevel++)
    {
        if (travelFlag == 0)
        {
            if (intensityLevel == 1)
            {
                LED_0.WriteFile(HIGH);
            }
            else if (intensityLevel == 2)
            {
                LED_1.WriteFile(HIGH);
            }
            else if (intensityLevel == 3)
            {
                LED_2.WriteFile(HIGH);
                travelFlag = 1;
            } 
        }
    }
    for (intensityLevel = 2; intensityLevel >= 0; intensityLevel--)
    {
        if (travelFlag == 1)
        {
            if (intensityLevel == 2)
            {
                LED_2.WriteFile(LOW);
            }
            else if (intensityLevel == 1)
            {
                LED_1.WriteFile(LOW);
            }
            else if (intensityLevel == 0)
            {
                LED_0.WriteFile(LOW);
                travelFlag = 0;
            }
        }
    }
}