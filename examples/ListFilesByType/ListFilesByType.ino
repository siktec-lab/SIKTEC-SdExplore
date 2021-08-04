/******************************************************************************/
// Created by: Shlomo Hassid.
// Release Version : 1.0.3
// Creation Date: 2021-04-27
// Copyright 2021, SIKTEC.
/******************************************************************************/
/*****************************      Changelog       ****************************
version:
    1.0.3
        -> required sdfat is adafruit fork now since it has better config.
    1.0.2
        -> version bump -> no edits in the example 
        -> listRootDirectory now takes serial as a pointer changed in this example.
    1.0.1
        -> initial release.
        -> Example supports Serial commands to scan and communicate with the
           sd card file system.
        -> Commands available:
            - dir     : changes the working directory.
            - scan    : scans and index files of a given type.
            - file    : prints a specific filename by its index.
            - listall : ls command to show al files and directories 
                        in the working directory.
            - reset   : reset all flags and current command.
*******************************************************************************/
/***********************  Assets / Learning More  *****************************
  1.
*******************************************************************************/
/***************************   Dependencies   *********************************
 * SIKTEC_SdExplore - https://github.com/siktec-lab/SIKTEC-SdExplore
*******************************************************************************/

/******************************************************************************/
/**** REQUIRED / LIBRARIES / INCLUDES *****************************************/
/******************************************************************************/

#include <Arduino.h>
#include <SIKTEC_SdExplore.h>     

/***************************************************************************/
/**** BASE DEFINITIONS *****************************************************/
/***************************************************************************/

#define CONT_SD_CS          9       // PB1, Arduino PIN 9  -> SD card Chip select
#define CONT_SD_LIMIT_MHZ   50      // 50 MHz limit the SPI interface - use at 10 or 50
#define CONT_SERAIL_BAUD    9600

/***************************************************************************/
/**** INTIATE OBJECTS / LIBRARIES / GLOBAL VARIABLES ***********************/
/***************************************************************************/

// The companion lib: 
SIKtec::SdExplore sdexplore(
    20, // Max files to index when scanning
    50 // Max file name length - longer files will be ignored by the scan
); 

// A buffer for commands received via serial: 
String command; 

// Some flags used to indicate which serial command needs to be executed:
enum OpFlags { OP_NONE, OP_SCAN, OP_DIR, OP_FILE };
OpFlags opFlag = OpFlags::OP_NONE;

// A buffer to hold the filename use by the 'file' command:
char* fileNameBuffer = new char[sdexplore.defineMaxFileName];

/***************************************************************************/
/**** Functions ************************************************************/
/***************************************************************************/

/*! printScreens
    @brief  An helper function to print a header.
    @param  name the command name to print.
*/
void printCommandHeader(const char* name) {
    Serial.print(F("-- "));
    Serial.print(name);
    Serial.println(F(" -------------------------------"));
}
/*! printScreens
    @brief  An helper function to print a footer.
*/
void printCommandFooter() {
    Serial.println(F("-----------------------------------------"));
}
/***************************************************************************/
/**** MAIN LOGIC - Setup + Main Loop ***************************************/
/***************************************************************************/

/*! setup
    @brief  run once before the main loop
*/
void setup()   {

    //Initiate serial port:
    Serial.begin(CONT_SERAIL_BAUD);
    #if !defined(ESP32)
        while (!Serial) { ; }
    #endif

    //Initialize sd card:
    if(sdexplore.initialize(CONT_SD_CS, CONT_SD_LIMIT_MHZ)) {
        //Set Folder of SD card:
        sdexplore.setRoot("/");
    } else {
        Serial.println("Error initializing SD Card");
        for(;;); // Halt Sd card is not intialized
    }

}
/*! loop
    @brief  infinite main loop 
*/
void loop() {
    //Check if got Serial data:
    while (Serial.available()) {

        //Get the command string
        command = Serial.readString();

        //Remove trailing spaces
        command.trim();

        //Show what we received:
        printCommandHeader(command.c_str());

        //Proccess the command:
        if (command == "reset") {
            //Reset all flags and command.
            command = "";
            opFlag = OpFlags::OP_NONE;
            Serial.println(F("Reset Done"));
        }
        else if (opFlag != OpFlags::OP_NONE) {
            // An operation flag was set - execute it:
            switch (opFlag) {
                case OpFlags::OP_DIR: {
                    //Change working directory:
                    sdexplore.setRoot(command.c_str());
                } break;
                case OpFlags::OP_SCAN: {
                    //Scan for files that are of type:
                    sdexplore.scanFiles(command.c_str());
                    Serial.print(F("Total files found: "));
                    Serial.println(sdexplore.loadedFiles);
                } break;
                case OpFlags::OP_FILE: {
                    //Print the name of a specific indexed file:
                    sdexplore.getFullFileNameByIndex(fileNameBuffer, atoi(command.c_str()));
                    Serial.print(F("File name is: "));
                    Serial.println(fileNameBuffer);
                } break;
            }
            opFlag = OpFlags::OP_NONE; //Reset flags.
        }
        else if (command == "listall") {
            //List current directory files (all files)
            sdexplore.listRootDirectory(&Serial);
            opFlag = OpFlags::OP_NONE;
        } 
        else if (command == "scan") {
            //Set scan mode:
            Serial.println(F("Wainting for file type..."));
            opFlag = OpFlags::OP_SCAN;
        } 
        else if (command == "file") {
            //Set file mode:
            Serial.println(F("send index to print name..."));
            opFlag = OpFlags::OP_FILE;
        } 
        else if (command == "dir") {
            //Set directory mode:
            Serial.println(F("Wainting for dir path..."));
            opFlag = OpFlags::OP_DIR;
        } else {
            //unknown command - reset:
            opFlag = OpFlags::OP_NONE;
            Serial.println(F("Unknown Command"));
        }
        //Print footer devider:
        printCommandFooter();
    }
}