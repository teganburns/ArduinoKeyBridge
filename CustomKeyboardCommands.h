#ifndef CUSTOM_KEYBOARD_COMMANDS_H
#define CUSTOM_KEYBOARD_COMMANDS_H

#include <stdint.h>

// Define debugging flag
#ifndef CUSTOM_KEYBOARD_COMMANDS_DEBUG
#define CUSTOM_KEYBOARD_COMMANDS_DEBUG true
#endif

class CustomKeyboardCommands {
public:

    static CustomKeyboardCommands& getInstance(); // Singleton accessor

    static bool COMMAND_MODE; // Tracks whether command mode is active

    // Constructor and Destructor
    //CustomKeyboardCommands();
    //~CustomKeyboardCommands();

    // Method to initialize the command listener
    void init();

    // Method to process a keypress
    void processKey(uint8_t keycode);

private:
    // Helper methods for specific key handling
    void handleF13();
    void handleF14();
    void handleF15();
    void handleF16();
    void handleF17();
    void handleF18();

    // Debug helper method
    void debugPrint(const char* message);

    CustomKeyboardCommands() = default;                  // Private constructor
    ~CustomKeyboardCommands() = default;                 // Private destructor
    CustomKeyboardCommands(const CustomKeyboardCommands&) = delete; // Prevent copy
    CustomKeyboardCommands& operator=(const CustomKeyboardCommands&) = delete;
};

#endif // CUSTOM_KEYBOARD_COMMANDS_H