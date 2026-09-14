//
// Created by Wizado F4ISE on 14/08/2026.
//

#ifndef SERIALCLI_H
#define SERIALCLI_H

void terminalInit(void);
void receiveCLI(void);

// Function prototypes
void unknownCommand(const char *command);
void cmdHelp();
void cmdLedOn();
void cmdLedOff();
void cmdPrintHello();
void cmdPrintIntArgument();
void cmdPrintStringArguments();
void printConsoleChar();


#endif // SERIALCLI_H
