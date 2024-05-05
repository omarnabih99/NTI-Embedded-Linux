#include <fstream>
#include <iostream>

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
    Basic_Character_Devices Basic_Device0("/dev/Basic_Device0"); //RDONLY
    Basic_Character_Devices Basic_Device1("/dev/Basic_Device1"); //WRONLY
    Basic_Character_Devices Basic_Device2("/dev/Basic_Device2"); //RDWR
    Basic_Character_Devices Basic_Device3("/dev/Basic_Device3"); //RDWR

    std::cout << "The Application is running..." << "\n";

    std::string input;

    std::cout << "Please enter the desired device: ";
    int desiredDevice;
    std::cin >> desiredDevice;
    switch (desiredDevice)
    {
        case 0:
                std::cout << "Warning: This device is read-only" << "\n";
                std::cout << Basic_Device0.ReadFile() << "\n";
                break;

        case 1: 
                std::cout << "Warning: This device is write-only" << "\n";
                //std::cout << "Please enter value: ";
                //std::getline(std::cin, input);
                //Basic_Device1.WriteFile(input);
                break;

        case 2:
                std::cout << "Warning: This device is read-write" << "\n";
                //std::cout << "Please enter value: ";
                //std::getline(std::cin, input);
                input = "This is device 2";
                Basic_Device2.WriteFile(input);
                std::cout << Basic_Device2.ReadFile() << "\n";
                break;

        case 3:
                std::cout << "Warning: This device is read-write" << "\n";
                //std::cout << "Please enter value: ";
                //std::getline(std::cin, input);
                input = "This is device 3";
                Basic_Device3.WriteFile(input);
                std::cout << Basic_Device3.ReadFile() << "\n";
                break;
    }

}