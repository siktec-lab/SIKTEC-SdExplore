/******************************************************************************/
// Created by: Shlomo Hassid.
// Release Version : 1.0.3
// Creation Date: 2021-04-27
// Copyright 2021, SIKTEC.
/******************************************************************************/
/*****************************      About       ********************************
 * SdExplore is a light-weight companion library to easily scan for specific files
 * on an sd card attached to your project - The aim is to perform memory heavy 
 * operations in the smallest memory consumption - Those are handy for creating 
 * menus, navigating the sd card and searching for specific files or file type 
 * on the sd card.
 * All of the SdFat methods are accessible from the SD static instance  - SIKtec::SD
*******************************************************************************/
/*****************************      Changelog       ****************************
version:
    1.0.3
        -> required sdfat is adafruit fork now since it has better config.
    1.0.2
        -> prepare for arduino library 
        -> added macros to remove serial.
        -> improved listRootDirectory now takes serial as a pointer.
    1.0.1
        -> initial release - scan files, index them, handy methods.
        -> Example to showcase the usage.
        -> Added expressive comments for the methods.
*******************************************************************************/
/***********************  Assets / Learning More  *****************************
  1. 
*******************************************************************************/
/***************************   Dependencies   *********************************
 * SdFat - https://github.com/greiman/SdFat
*******************************************************************************/

#ifndef _SIKTECSDEXPLORE_h
#define _SIKTECSDEXPLORE_h

#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>                // SD card & FAT filesystem library


namespace SIKtec {

    /***************************************************************************/
    /**** Some Defintions - ****************************************************/
    /***************************************************************************/
    // Don't edit - Most of those can be set by the constructor.
    #define SDEXPLORE_LOAD_MAX_FILES  50 
    #define SDEXPLORE_MAX_FILES_NAME  50 
    //For specific compilation commenting this will remove serial printing and all 
    //The code related.
    #define SDEXPLORE_USE_SERIAL           

    /***************************************************************************/
    /**** SdExplore CLASS *********************************************************/
    /***************************************************************************/
    class SdExplore {
        
    private:

        //Whether we want to show procedures as Serial Output:
        //bool debugSerial;

        //Loaded directory:
        FatFile* mainDirectory;

        //Loaded files indexes:
        uint16_t* files;

    public:

        //SD card filesystem: static instance that exposes all the SdFat methods
        static SdFat SD; 

        //definition of how many files to index when using scan:
        uint16_t defineMaxFiles;

        //definition of the max filename length we will use:
        uint16_t defineMaxFileName;

        // Stores how many files were found by the scan method:
        uint16_t loadedFiles = 0;

        /*! 
        * Constructor
        * ----------------------------
        * @brief initiate the Class with some required definitions
        * @return SdExplore 
        */
        SdExplore(
            //const bool useSerial = false, 
            const uint16_t maxFiles = SDEXPLORE_LOAD_MAX_FILES,
            const uint16_t maxFileName = SDEXPLORE_MAX_FILES_NAME
        );

        /*! 
        * initialize
        * ----------------------------
        * @brief initiate sd card.
        * @param sd_cs_pin the sd card pin number
        * @return true on success false on failure
        */
        bool initialize(uint8_t sd_cs_pin, uint8_t limitSpiMHz = 10);

        /*!
        *  ~SdExplore
        * ----------------------------
        * @brief class destructor - release the memory allocated for files.
        * @return void
        */
        ~SdExplore();

        /*!
        * setRoot
        * ----------------------------
        * @brief sets a working directory which the filesystem will address as root.
        * @param path the director path to load if NULL '/' will be used. 
        * @return bool
        */
        bool setRoot(const char* path = NULL);

        /*!
        * scanFiles
        * ----------------------------
        * @brief scans file on the current working directory for given file types
        *        will index the on 'files' array with the corresponding directory file index.
        * @param ext the file extenssion to search for - basically can be any ending substring. 
        * @return the number of file found and indexed.
        * @note scan will stop if upper limit is reached - set by this->defineMaxFiles set in the constructor
        * @note an empty string will index all files.
        */
        uint16_t scanFiles(const char* ext);

        /*!
        * fileExtensionIs
        * ----------------------------
        * @brief checks if the a char array ends with a substring
        * @param filename the file name to check. 
        * @param ext the file extenssion to search for - Basically can be any ending substring. 
        * @return bool
        * @note scan will stop if upper limit is reached - set by this->defineMaxFiles set in the constructor
        */
        bool fileExtensionIs(const char* filename, const char* ext);

        /*!
        * getFullFileNameByIndex
        * ----------------------------
        * @brief get a full file name by its index - Not the internal sd card index, The scan index internally 
        *        by this class.
        * @note  the file name will always be of max length = this->defineMaxFileName
        * @param nameBuff an external buffer to store the requested file name. 
        * @param index the index of the requested file - will always be in range of [0 - this->defineMaxFiles]. 
        * @return void
        */
        void getFullFileNameByIndex(char* nameBuff , const int16_t index);

        /*!
        * getFilesAroundCurrent
        * ----------------------------
        * @brief return filename "around" a requested file (by its index). This is handy and saves
        *        allot of memory and hassle when creating menus and navigation like procedures.
        * @note if there are no files before or after an empty string '\0' will be stored.  
        * 
        * @param index the index of the requested file - will always be in range of [0 - this->defineMaxFiles]. 
        * @param menunames poiter to 2D char array that will stores the file names - make sure its of the correct size. 
        * @param bufferSize number of rows. 
        * @param maxlength the length of each row (filename) - max is this->defineMaxFileName. 
        * @param shift how many rows we want before and after - > shift 1 is [[-1],[current],[+1]]. 
        * @return void
        */
        void getFilesAroundCurrent(uint8_t index, char* menunames, size_t bufferSize, size_t maxlength, size_t shift = 1);

        /*!
        * listRootDirectory
        * ----------------------------
        * @brief the equivalent to the 'ls' function - will print to Serial all files and directories 
        *        in the current working directory.
        * @param pr a poiter to Serial
        * @return void
        */
        void listRootDirectory(print_t* pr);

    };

}

#endif
