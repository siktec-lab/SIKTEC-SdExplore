
/******************************************************************************/
// Created by: Shlomo Hassid.
// Release Version : 1.0.2
// Creation Date: 2021-04-27
// Copyright 2021, SIKTEC.
/******************************************************************************/

#include "SIKTEC_SdExplore.h"

namespace SIKtec {
    
    /*
    * SD: static instance that exposes all the SdFat methods
    */
    SdFat SdExplore::SD;

    /*
    * Constructor: initiate the Class with some required definitions
    */
    SdExplore::SdExplore(
        //const bool useSerial, 
        const uint16_t maxFiles,
        const uint16_t maxFileName
    ) {
        //this->debugSerial = useSerial;
        this->defineMaxFiles = maxFiles;
        this->defineMaxFileName = maxFileName;
        this->files = new uint16_t[maxFiles];
        memset(this->files, 0, sizeof(maxFiles));
    }

    /*
    * initialize: initiate sd card
    */
    bool SdExplore::initialize(uint8_t sd_cs_pin, uint8_t limitSpiMHz) {
        #ifdef SDEXPLORE_USE_SERIAL
            Serial.print(F("Initializing filesystem..."));
        #endif
        if(!SdExplore::SD.begin(sd_cs_pin, SD_SCK_MHZ(limitSpiMHz))) {
            #ifdef SDEXPLORE_USE_SERIAL
                Serial.println(F("SD initialization failed."));
            #endif
            return false;
        }
        #ifdef SDEXPLORE_USE_SERIAL
            Serial.println(F(" Filesystem OK!"));
        #endif
        return true;
    }

    /*
     * class destructor - release the memory allocated for files.
     */
    SdExplore::~SdExplore(void) {
        delete[] this->files;
    }

    /*
    * setRoot: sets a working directory which the filesystem will address as root.
    */
    bool SdExplore::setRoot(const char* path) {
        if (
            SdExplore::SD.chdir((path ? path : "/"))
        ) {
            this->mainDirectory = SdExplore::SD.vwd();
            #ifdef SDEXPLORE_USE_SERIAL
                Serial.println(F("Loaded directory."));
            #endif
            return true;
        }
        #ifdef SDEXPLORE_USE_SERIAL
            Serial.println(F("Failed to load directory."));
        #endif
        return false;
    }

    /*
    * scanFiles: scans file on the current working directory for given file types
    */
    uint16_t SdExplore::scanFiles(const char* ext) {
        this->loadedFiles = 0;
        if (!this->mainDirectory->isOpen()) {                
            #ifdef SDEXPLORE_USE_SERIAL
                Serial.println(F("Current directory not open."));
            #endif
        } else {
            FatFile candid;
            char nameBuff[this->defineMaxFileName];
            memset(this->files, 0, this->defineMaxFiles);
            memset(nameBuff, ' ', this->defineMaxFileName);
            this->mainDirectory->rewind();
            while (candid.openNext(this->mainDirectory, O_RDONLY) && this->loadedFiles < this->defineMaxFiles) {
                if (candid.isFile() && !candid.isHidden()) {
                    candid.getName(nameBuff, this->defineMaxFileName);
                    if (this->fileExtensionIs(nameBuff, ext)) {
                        this->files[this->loadedFiles++] = candid.dirIndex();
                        #ifdef SDEXPLORE_USE_SERIAL
                            Serial.print(nameBuff);
                            Serial.print(F(" - Folder Index: "));
                            Serial.print(candid.dirIndex());
                            Serial.print(F(" - System Index: "));
                            Serial.println(this->loadedFiles - 1);
                        #endif
                    }
                }
                candid.close();
            }
        }
        return this->loadedFiles;
    }

    /*
    * fileExtensionIs: checks if the a char array ends with a substring
    */
    bool SdExplore::fileExtensionIs(const char* filename, const char* ext) {
        uint8_t name_len = strlen(filename);
        uint8_t ext_len = strlen(ext);
        return (name_len >= ext_len) && (0 == strcmp(filename + (name_len - ext_len), ext));
    }
    
    void SdExplore::getFullFileNameByIndex(char* nameBuff , const int16_t index) {
        FatFile candid;
        if (index < this->loadedFiles && index >= 0) {
            if (candid.open(this->mainDirectory, this->files[index], O_RDONLY)) {
                candid.getName(nameBuff, this->defineMaxFileName);
                candid.close();
            } else {
                nameBuff[0] = '\0';
            }
        }
    }

    void SdExplore::getFilesAroundCurrent(uint8_t index, char* menunames, size_t bufferSize, size_t maxlength, size_t shift) {
        if (this->loadedFiles < 1) return;
        uint8_t rows = shift * 2 + 1;
        int8_t loop_index = index - shift;
        FatFile candid;
        char nameBuff[maxlength] = {' '};
             nameBuff[maxlength - 1] = '\0';
        for (uint8_t i = 0; i < rows; i++, loop_index++) {
            if (loop_index < this->loadedFiles && loop_index >= 0) {
                if (candid.open(this->mainDirectory, this->files[loop_index], O_RDONLY)) {
                    candid.getName(nameBuff, maxlength);
                    candid.close();
                } else {
                    nameBuff[0] = '\0';
                }
            } else {
                nameBuff[0] = '\0';
            }
            memcpy((menunames + bufferSize * i), nameBuff, sizeof(nameBuff));
            nameBuff[maxlength-1] = '\0';
        }
    }
    /*
    * listRootDirectory:
    */
    void SdExplore::listRootDirectory(print_t* pr) {
        SdExplore::SD.ls(pr);
    }
}
