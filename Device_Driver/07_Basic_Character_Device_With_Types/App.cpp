#include <fstream>
#include <iostream>

class Basic_Character_Devices
{
    private:
        const std::string PATH {"/dev/Basic_Device2"};
        std::fstream fileDescriptor;

    public:
        void WriteFile (std::string message);
        std::string ReadFile (void);
};

void Basic_Character_Devices::WriteFile(std::string message)
{
    fileDescriptor.open(PATH, std::ios::out);

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
    std::string input;

    Basic_Character_Devices Basic_Device2;

    std::cout << "The Application is running..." << "\n";

    std::cout << "Please enter value: ";

    std::getline(std::cin, input);

    Basic_Device2.WriteFile(input);

    std::cout << Basic_Device2.ReadFile() << "\n";
}