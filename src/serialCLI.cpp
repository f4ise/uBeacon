//
// Created by Wizado F4ISE on 14/08/2026.
//
#include <Arduino.h>
#include <EEPROM.h>

#include <ErriezSerialTerminal.h>

#include "config.h"
#include "serialCLI.h"


// Newline character '\r' or '\n'
char newlineChar = '\r';
// Separator character between commands and arguments
char delimiterChar = ' ';

// Create serial terminal object
SerialTerminal term(newlineChar, delimiterChar);

void terminalInit() {
    Serial.println(F("\nSerial terminal example."));
    Serial.println(F("Type 'help' to display usage."));
    printConsoleChar();

    // Set default handler for unknown commands
    term.setDefaultHandler(unknownCommand);

    // Add command callback handlers
    term.addCommand("?", cmdHelp);
    term.addCommand("help", cmdHelp);
    term.addCommand("get", cmdGetConfig);
    term.addCommand("set", cmdSetConfig);
    term.addCommand("save", cmdSaveConfig);
    term.addCommand("erase", cmdEraseEEPROM);
    term.addCommand("read", cmdReadEEPROM);

    //Enable Char Echoing
    term.setSerialEcho(true);
    //Set Post Command Handler
    term.setPostCommandHandler(printConsoleChar);
}

void receiveCLI() {
    // Read from serial port and handle command callbacks
    term.readSerial();
}

void printConsoleChar()
{
    Serial.print(F("# "));
}

void unknownCommand(const char *command)
{
    // Print unknown command
    Serial.print(F("Unknown command: "));
    Serial.println(command);
}

void cmdHelp()
{
    // Print usage
    Serial.println(F("Serial terminal usage:"));
    Serial.println(F("  help or ?           Print this command help"));
    Serial.println(F("  get <type>          Get Config."));
    Serial.println(F("  set <type> <arg1> <arg2> Set Config."));
    Serial.println(F("  save                Save Config. to EEPROM"));
}

void cmdPrintHello()
{
    char *arg;
    char *name = NULL;
    int age = 0;

    // Get first argument
    arg = term.getNext();
    if (arg != NULL) {
        // Try to convert argument to int
        if (atoi(arg)) {
            // Name is not specified
            // Convert age string to int
            age = atoi(arg);
        } else {
            // Store first name argument
            name = arg;

            // Get second age argument
            arg = term.getNext();

            // Convert age string to int
            age = atoi(arg);
        }
    }

    // Print name
    if (name) {
        Serial.print(F("Hello "));
        Serial.print(name);
        Serial.println(F("!"));
    } else {
        Serial.println(F("Hello. You did not specify your name."));
    }

    // Print age
    if (age) {
        Serial.print(F("You are "));
        Serial.print(age, DEC);
        Serial.println(F(" years old."));
    } else {
        Serial.println(F("You did not specify your age."));
    }
}

void cmdPrintIntArgument()
{
    int val;
    char *arg;

    // Get argument
    arg = term.getNext();
    if (arg == NULL) {
        Serial.println(F("Decimal or hex value not specified."));
        return;
    }

    // Try to convert decimal or hex argument character string to an int
    if (strncmp(arg, "0x", 2) == 0) {
        if (sscanf(arg, "0x%x", &val) != 1) {
            Serial.println(F("Cannot convert hex value."));
            return;
        }
    } else {
        if (sscanf(arg, "%d", &val) != 1) {
            Serial.println(F("Cannot convert decimal value."));
            return;
        }
    }

    // Print argument as string and value in hex / decimal
    Serial.print(F("String: "));
    Serial.print(arg);
    Serial.print(F("; "));

    Serial.print(F("Hex: 0x"));
    if (val < 0x10) {
        Serial.print(F("0"));
    }
    Serial.print(val, HEX);
    Serial.print(F("; "));

    Serial.print(F("Dec: "));
    Serial.println(val, DEC);
}

void cmdPrintStringArguments()
{
    int argIndex;
    char *arg;
    char *remaining;

    // Print arguments
    Serial.println(F("Arguments [1..4]: "));
    for (argIndex = 0; argIndex < 4; argIndex++) {
        arg = term.getNext();
        if (arg != NULL) {
            Serial.print(F("  "));
            Serial.print(argIndex);
            Serial.print(F(": "));
            Serial.println(arg);
        } else {
            break;
        }
    }

    if (argIndex == 0) {
        // No arguments specified
        Serial.println(F("  None"));
    } else {
        // Print remaining string
        remaining = term.getRemaining();

        if (remaining) {
            Serial.print(F("Remaining: "));
            Serial.println(remaining);
        }
    }
}

void cmdGetConfig() {
    //
}

void cmdSetConfig() {
    //
}

void cmdSaveConfig() {
    //
}

void cmdEraseEEPROM() {
    for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0x00);
    }
    if (EEPROM.commit()) {
        Serial.println(F("EEPROM successfully committed"));
    } else {
        Serial.println(F("ERROR! EEPROM commit failed"));
    }
}

void cmdReadEEPROM() {
    byte value = 0;
    for (int i = 0 ; i < EEPROM.length() ; i++) {
        value = EEPROM.read(i);
        Serial.print(i);
        Serial.print("\t");
        Serial.print(value, DEC);
        Serial.println();
    }
}