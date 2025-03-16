// ----------------------------<fileApi.h>------------------------------------
#ifndef _FILE_COM_H
#define _FILE_COM_H
// ---------------------------
#include <ArduinoJson.h>
#include <AsyncTCP.h>          
#include <ESPAsyncWebServer.h> 
#include "esp_system.h"        
#include "esp_spi_flash.h"     
#include "esp_wifi_types.h"    
#include "esp_bt.h"            
#include <ESPmDNS.h>
#include <SD.h>
#include <SPIFFS.h>
#include <M5Unified.h>
#include <WiFi.h>  

extern String SdPath;
extern void Home();
extern String HTML_Header();
extern String HTML_Footer();
extern AsyncWebServer server;
typedef struct
{
  String filename;
  String ftype;
  String fsize;
} fileinfo;
extern fileinfo Filenames[]; // Enough for most purposes!
extern String webpage, MessageLine;

// -------------------------------------------------------
void setupFileComHandler();
void FilesList();
void FilesDirList();
void Dir(AsyncWebServerRequest *request);
void UploadFileSelect();
void handleFileUpload(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final);
void Handle_File_Delete(String filename);
void File_Rename();
void Handle_File_Rename(AsyncWebServerRequest *request, String filename, int Args);
void notFound(AsyncWebServerRequest *request);
void Handle_File_Download();
String getContentType(String filenametype);
void Select_File_For_Function(String title, String function);
int GetFileSize(String filename);
void Page_Not_Found();
void Display_System_Info();
String ConvBinUnits(int bytes, int resolution);
String EncryptionType(wifi_auth_mode_t encryptionType);
// bool StartMDNSservice(const char *Name);

// -------------------------------------------------------

#endif
