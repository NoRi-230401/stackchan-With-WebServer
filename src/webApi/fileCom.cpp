// ----------------------------<fileCom.cpp>------------------------------------
// *** Modified by NoRi 2025-03-16 **************
//  ESPAsynch_Server_v1.1 by David Bird 2022
//--------------------------------------------------------------------------------
/*
  This software, the ideas and concepts is Copyright (c) David Bird 2022
  All rights to this software are reserved.
  It is prohibited to redistribute or reproduce of any part or all of the software contents in any form other than the following:
  1. You may print or download to a local hard disk extracts for your personal and non-commercial use only.
  2. You may copy the content to individual third parties for their personal use, but only if you acknowledge the author David Bird as the source of the material.
  3. You may not, except with my express written permission, distribute or commercially exploit the content.
  4. You may not transmit it or store it in any other website or other form of electronic retrieval system for commercial purposes.
  5. You MUST include all of this copyright and permission notice ('as annotated') and this shall be included in all copies or substantial portions of the software
      and where the software use is visible to an end-user.
  6. *** DONT USE THE SOFTWARE IF YOU DONT LIKE THE LICNCE CONDITIONS ***
  THE SOFTWARE IS PROVIDED "AS IS" FOR PRIVATE USE ONLY, IT IS NOT FOR COMMERCIAL USE IN WHOLE OR PART OR CONCEPT.
  FOR PERSONAL USE IT IS SUPPLIED WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR
  A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHOR OR COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OR
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  See more at http://dsbird.org.uk
*/
#include "fileCom.h"
int start, downloadtime = 1, uploadtime = 1, downloadsize, uploadsize, downloadrate, uploadrate, numfiles;

void setupFileComHandler()
{
  // ##################### DIR HANDLER ##############################
  server.on("/dir", HTTP_GET, [](AsyncWebServerRequest *request)
    {Serial.println("File directory..."); Dir(request); request->send(200, "text/html", webpage); });

  // ##################### UPLOAD HANDLERS ###########################
  server.on("/upload", HTTP_GET, [](AsyncWebServerRequest *request)
    {Serial.println("Uploading file..."); UploadFileSelect(); request->send(200, "text/html", webpage); });
  
  server.on("/handleupload", HTTP_POST, [](AsyncWebServerRequest *request) {},
      [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
      { handleFileUpload(request, filename, index, data, len, final); });

  // ##################### DELETE HANDLER ############################
  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request)
    { Serial.println("Deleting file..."); Select_File_For_Function("[DELETE]", "deletehandler");
    request->send(200, "text/html", webpage); });

  // ##################### RENAME HANDLER ############################
  server.on("/rename", HTTP_GET, [](AsyncWebServerRequest *request)
    { Serial.println("Renaming file..."); File_Rename();
    request->send(200, "text/html", webpage); });

  // ##################### HOMEPAGE HANDLER ########################
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    { Serial.println("Home Page..."); Home(); request->send(200, "text/html", webpage); });

    // ##################### NOT FOUND HANDLER #########################
  server.onNotFound(notFound);
}

// #############################################################################################
void Dir(AsyncWebServerRequest *request)
{
  String Fname1, Fname2;
  int index = 0;
  FilesDirList(); // Get a list of the current files and dir List
  webpage = HTML_Header();
  webpage += "<h3>Filing System Content</h3><br>";
  if (numfiles > 0)
  {
    webpage += "<table class='center'>";
    webpage += "<tr><th>Type</th><th>File Name</th><th>File Size</th><th class='sp'></th><th>Type</th><th>File Name</th><th>File Size</th></tr>";
    while (index < numfiles)
    {
      Fname1 = Filenames[index].filename;
      Fname2 = Filenames[index + 1].filename;
      webpage += "<tr>";
      webpage += "<td style = 'width:5%'>" + Filenames[index].ftype + "</td><td style = 'width:25%'>" + Fname1 + "</td><td style = 'width:10%'>" + Filenames[index].fsize + "</td>";
      webpage += "<td class='sp'></td>";
      if (index < numfiles - 1)
      {
        webpage += "<td style = 'width:5%'>" + Filenames[index + 1].ftype + "</td><td style = 'width:25%'>" + Fname2 + "</td><td style = 'width:10%'>" + Filenames[index + 1].fsize + "</td>";
      }
      webpage += "</tr>";
      index = index + 2;
    }
    webpage += "</table>";
    webpage += "<p style='background-color:yellow;'><b>" + MessageLine + "</b></p>";
    MessageLine = "";
  }
  else
  {
    webpage += "<h2>No Files Found</h2>";
  }
  webpage += HTML_Footer();
  request->send(200, "text/html", webpage);
}

// #############################################################################################
void FilesDirList()
{
  numfiles = 0; // Reset number of SPIFFS/SD files counter
  File root;
  root = SPIFFS.open("/", "r");

  if (root)
  {
    root.rewindDirectory();
    File file = root.openNextFile();

    String flname;
    while (file)
    { // Now get all the filenames, file types and sizes
      flname = (String(file.name()).startsWith("/") ? String(file.name()).substring(1) : file.name());
      if (flname == "System Volume Information")
      {
        file = root.openNextFile();
      }
      else
      {
        Filenames[numfiles].filename = flname;
        Filenames[numfiles].ftype = (file.isDirectory() ? "Dir" : "File");
        Filenames[numfiles].fsize = ConvBinUnits(file.size(), 1);
        file = root.openNextFile();
        numfiles++;
      }
    }
    root.close();
  }
}

// #############################################################################################
void FilesList()
{
  numfiles = 0; // Reset number of SPIFFS/SD files counter
  File root;
  root = SPIFFS.open("/", "r");
  
  if (root)
  {
    root.rewindDirectory();
    File file = root.openNextFile();

    while (file)
    { // Now get all the filenames, file types and sizes
      if (!file.isDirectory())
      {
        Filenames[numfiles].filename = (String(file.name()).startsWith("/") ? String(file.name()).substring(1) : file.name());
        Filenames[numfiles].ftype = (file.isDirectory() ? "Dir" : "File");
        Filenames[numfiles].fsize = ConvBinUnits(file.size(), 1);

        file = root.openNextFile();
        numfiles++;
      }
      else
      {
        file = root.openNextFile();
      }
    }
    root.close();
  }
}

// #############################################################################################
void UploadFileSelect()
{
  webpage = HTML_Header();
  webpage += "<h3>Select a File to [UPLOAD] to this device</h3>";
  webpage += "<form method = 'POST' action = '/handleupload' enctype='multipart/form-data'>";
  webpage += "<input type='file' name='filename'><br><br>";
  webpage += "<input type='submit' value='Upload'>";
  webpage += "</form>";
  webpage += HTML_Footer();
}

// #############################################################################################
void handleFileUpload(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{
  if (!index)
  {
    String file = filename;
    if (!filename.startsWith("/"))
        file = "/" + filename;

    request->_tempFile = SPIFFS.open(file, "w");
  }

  if (!request->_tempFile)
    Serial.println("Error creating file for upload...");
  start = millis();

  if (request->_tempFile)
  {
    if (len)
    {
      request->_tempFile.write(data, len); // Chunked data
      Serial.println("Transferred : " + String(len) + " Bytes");
    }
    if (final)
    {
      uploadsize = request->_tempFile.size();
      request->_tempFile.close();
      uploadtime = millis() - start;
      request->redirect("/dir");
    }
  }
}

// #############################################################################################
void Handle_File_Delete(String filename)
{ // Delete the file
  webpage = HTML_Header();
  if (!filename.startsWith("/"))
    filename = "/" + filename;

  File dataFile;
  dataFile = SPIFFS.open(filename, "r");

  if (dataFile)
  { // It does so delete it
    SPIFFS.remove(filename);
    webpage += "<h3>File '" + filename.substring(1) + "' has been deleted</h3>";
    webpage += "<a href='/dir'>[Enter]</a><br><br>";
  }
  else
  {
    webpage += "<h3>File [ " + filename + " ] does not exist</h3>";
    webpage += "<a href='/dir'>[Enter]</a><br><br>";
  }
  webpage += HTML_Footer();
}

// #############################################################################################
void File_Rename()
{ // Rename the file
  FilesList();
  webpage = HTML_Header();
  webpage += "<h3>Select a File to [RENAME] on this device</h3>";
  webpage += "<FORM action='/renamehandler'>";
  webpage += "<table class='center'>";
  webpage += "<tr><th>File name</th><th>New Filename</th><th>Select</th></tr>";
  int index = 0;
  while (index < numfiles)
  {
    webpage += "<tr><td><input type='text' name='oldfile' style='color:blue;' value = '" + Filenames[index].filename + "' readonly></td>";
    webpage += "<td><input type='text' name='newfile'></td><td><input type='radio' name='choice'></tr>";
    index++;
  }
  webpage += "</table><br>";
  webpage += "<input type='submit' value='Enter'>";
  webpage += "</form>";
  webpage += HTML_Footer();
}

// #############################################################################################
void Handle_File_Rename(AsyncWebServerRequest *request, String filename, int Args)
{ // Rename the file
  String newfilename;
  int j = 0;
  webpage = HTML_Header();

  if (Args >= 3)
  {
    for (int i = 2; i < Args; i++)
    {
      if (request->arg(i - 1) != "" && request->arg(i) == "on")
      {
        filename = request->arg(i - 2);
        newfilename = request->arg(i - 1);
        break;
      }
    }
  }

  if (!filename.startsWith("/"))
    filename = "/" + filename;

  if (!newfilename.startsWith("/"))
    newfilename = "/" + newfilename;

  File CurrentFile;
  CurrentFile = SPIFFS.open(filename, "r"); // Now read SPIFFS to see if file exists

  if (CurrentFile && filename != "/" && newfilename != "/" && (filename != newfilename))
  { // It does so rename it, ignore if no entry made, or Newfile name exists already
    if (SPIFFS.rename(filename, newfilename))
    {
      filename = filename.substring(1);
      newfilename = newfilename.substring(1);
      webpage += "<h3>File '" + filename + "' has been renamed to '" + newfilename + "'</h3>";
      webpage += "<a href='/dir'>[Enter]</a><br><br>";
    }
  }
  else
  {
    if (filename == "/" && newfilename == "/")
      webpage += "<h3>File was not renamed</h3>";
    else
      webpage += "<h3>New filename exists, cannot rename</h3>";
    webpage += "<a href='/rename'>[Enter]</a><br><br>";
  }

  CurrentFile.close();
  webpage += HTML_Footer();
}

// #############################################################################################
//  Not found handler is also the handler for 'delete', 'download' functions
void notFound(AsyncWebServerRequest *request)
{ // Process selected file types
  Serial.println("notFund func in ...");
  Serial.println("url -> " + request->url());
  
  String filename;

  if (request->url().startsWith("/deletehandler") || request->url().startsWith("/renamehandler"))
  {
    // Now get the filename and handle the request for 'delete' or 'download' or 'stream' functions
  
    if (!request->url().startsWith("/renamehandler"))
      filename = request->url().substring(request->url().indexOf("~/") + 1);
    
    start = millis();
  
    if (request->url().startsWith("/deletehandler"))
    {
      Serial.println("Delete handler started...");
      Handle_File_Delete(filename);
      request->send(200, "text/html", webpage);
    }

    if (request->url().startsWith("/renamehandler"))
    {
      Handle_File_Rename(request, filename, request->args());
      request->send(200, "text/html", webpage);
    }
  }
  else
  {
    Page_Not_Found();
    request->send(200, "text/html", webpage);
  }
}

// #############################################################################################
String getContentType(String filenametype)
{ // Tell the browser what file type is being sent
  if (filenametype == "download")
  {
    return "application/octet-stream";
  }
  else if (filenametype.endsWith(".txt"))
  {
    return "text/plain";
  }
  else if (filenametype.endsWith(".htm"))
  {
    return "text/html";
  }
  else if (filenametype.endsWith(".html"))
  {
    return "text/html";
  }
  else if (filenametype.endsWith(".css"))
  {
    return "text/css";
  }
  else if (filenametype.endsWith(".js"))
  {
    return "application/javascript";
  }
  else if (filenametype.endsWith(".png"))
  {
    return "image/png";
  }
  else if (filenametype.endsWith(".gif"))
  {
    return "image/gif";
  }
  else if (filenametype.endsWith(".jpg"))
  {
    return "image/jpeg";
  }
  else if (filenametype.endsWith(".ico"))
  {
    return "image/x-icon";
  }
  else if (filenametype.endsWith(".xml"))
  {
    return "text/xml";
  }
  else if (filenametype.endsWith(".pdf"))
  {
    return "application/x-pdf";
  }
  else if (filenametype.endsWith(".zip"))
  {
    return "application/x-zip";
  }
  else if (filenametype.endsWith(".gz"))
  {
    return "application/x-gzip";
  }
  // ----- Add by NoRi 2023-06-26 ---------------------------------
  else if (filenametype.endsWith(".csv"))
  {
    return "text/csv";
  }
  else if (filenametype.endsWith(".json"))
  {
    return "application/json";
  }
  else if (filenametype.endsWith(".mp3"))
  {
    return "audio/mpeg";
  }
  else if (filenametype.endsWith(".mp4"))
  {
    return "video/mp4";
  }

  return "text/plain";
}

// #############################################################################################
void Select_File_For_Function(String title, String function)
{
  String Fname1, Fname2;
  int index = 0;
  FilesList();
  webpage = HTML_Header();
  webpage += "<h3>Select a File to " + title + " from this device</h3>";
  webpage += "<table class='center'>";
  webpage += "<tr><th>File Name</th><th>File Size</th><th class='sp'></th><th>File Name</th><th>File Size</th></tr>";
  while (index < numfiles)
  {
    Fname1 = Filenames[index].filename;
    Fname2 = Filenames[index + 1].filename;
    if (Fname1.startsWith("/"))
      Fname1 = Fname1.substring(1);
    if (Fname2.startsWith("/"))
      Fname2 = Fname2.substring(1);
    webpage += "<tr>";
    webpage += "<td style='width:25%'><button><a href='" + function + "~/" + Fname1 + "'>" + Fname1 + "</a></button></td><td style = 'width:10%'>" + Filenames[index].fsize + "</td>";
    webpage += "<td class='sp'></td>";
    if (index < numfiles - 1)
    {
      webpage += "<td style='width:25%'><button><a href='" + function + "~/" + Fname2 + "'>" + Fname2 + "</a></button></td><td style = 'width:10%'>" + Filenames[index + 1].fsize + "</td>";
    }
    webpage += "</tr>";
    index = index + 2;
  }
  webpage += "</table>";
  webpage += HTML_Footer();

}

// #############################################################################################
int GetFileSize(String filename)
{
  int filesize;
  File CheckFile;
  CheckFile = SPIFFS.open(filename, "r");
  filesize = CheckFile.size();
  CheckFile.close();
  return filesize;
}

// #############################################################################################
void Page_Not_Found()
{
  webpage = HTML_Header();
  webpage += "<div class='notfound'>";
  webpage += "<h1>Sorry</h1>";
  webpage += "<p>Error 404 - Page Not Found</p>";
  webpage += "</div><div class='left'>";
  webpage += "<p>The page you were looking for was not found, it may have been moved or is currently unavailable.</p>";
  webpage += "<p>Please check the address is spelt correctly and try again.</p>";
  webpage += "<p>Or click <b><a href='/'>[Here]</a></b> for the home page.</p></div>";
  webpage += HTML_Footer();
}

// #############################################################################################
// void Display_System_Info()
// {
//   esp_chip_info_t chip_info;
//   esp_chip_info(&chip_info);
//   if (WiFi.scanComplete() == -2)
//     WiFi.scanNetworks(true, false); // Scan parameters are (async, show_hidden) if async = true, don't wait for the result
//   webpage = HTML_Header();
//   webpage += "<h3>System Information</h3>";
//   webpage += "<h4>Transfer Statistics</h4>";
//   webpage += "<table class='center'>";
//   webpage += "<tr><th>Last Upload</th><th>Last Download/Stream</th><th>Units</th></tr>";
//   webpage += "<tr><td>" + ConvBinUnits(uploadsize, 1) + "</td><td>" + ConvBinUnits(downloadsize, 1) + "</td><td>File Size</td></tr> ";
//   webpage += "<tr><td>" + ConvBinUnits((float)uploadsize / uploadtime * 1024.0, 1) + "/Sec</td>";
//   webpage += "<td>" + ConvBinUnits((float)downloadsize / downloadtime * 1024.0, 1) + "/Sec</td><td>Transfer Rate</td></tr>";
//   webpage += "</table>";
//   webpage += "<h4>SPIFFS Filing System</h4>";
//   webpage += "<table class='center'>";
//   webpage += "<tr><th>Total Space</th><th>Used Space</th><th>Free Space</th><th>Number of Files</th></tr>";
//   webpage += "<tr><td>" + ConvBinUnits(SPIFFS.totalBytes(), 1) + "</td>";
//   webpage += "<td>" + ConvBinUnits(SPIFFS.usedBytes(), 1) + "</td>";
//   webpage += "<td>" + ConvBinUnits(SPIFFS.totalBytes() - SPIFFS.usedBytes(), 1) + "</td>";

//   webpage += "<td>" + (numfiles == 0 ? "Pending Dir or Empty" : String(numfiles)) + "</td></tr>";

//   webpage += "</table>";
//   webpage += "<h4>CPU Information</h4>";
//   webpage += "<table class='center'>";
//   webpage += "<tr><th>Parameter</th><th>Value</th></tr>";
//   webpage += "<tr><td>Number of Cores</td><td>" + String(chip_info.cores) + "</td></tr>";
//   webpage += "<tr><td>Chip revision</td><td>" + String(chip_info.revision) + "</td></tr>";
//   webpage += "<tr><td>Internal or External Flash Memory</td><td>" + String(((chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "Embedded" : "External")) + "</td></tr>";
//   webpage += "<tr><td>Flash Memory Size</td><td>" + String((spi_flash_get_chip_size() / (1024 * 1024))) + " MB</td></tr>";
//   // webpage += "<tr><td>Current Free RAM</td><td>" + ConvBinUnits(ESP.getFreeHeap(), 1) + "</td></tr>";
//   webpage += "</table>";
//   webpage += "<h4>Network Information</h4>";
//   webpage += "<table class='center'>";
//   webpage += "<tr><th>Parameter</th><th>Value</th></tr>";
//   webpage += "<tr><td>LAN IP Address</td><td>" + String(WiFi.localIP().toString()) + "</td></tr>";
//   webpage += "<tr><td>Network Adapter MAC Address</td><td>" + String(WiFi.BSSIDstr()) + "</td></tr>";
//   webpage += "<tr><td>WiFi SSID</td><td>" + String(WiFi.SSID()) + "</td></tr>";
//   webpage += "<tr><td>WiFi RSSI</td><td>" + String(WiFi.RSSI()) + " dB</td></tr>";
//   webpage += "<tr><td>WiFi Channel</td><td>" + String(WiFi.channel()) + "</td></tr>";
//   webpage += "<tr><td>WiFi Encryption Type</td><td>" + String(EncryptionType(WiFi.encryptionType(0))) + "</td></tr>";
//   webpage += "</table> ";
//   webpage += HTML_Footer();
// }

// #############################################################################################
String ConvBinUnits(int bytes, int resolution)
{
  if (bytes < 1024)
  {
    return String(bytes) + " B";
  }
  else if (bytes < 1024 * 1024)
  {
    return String((bytes / 1024.0), resolution) + " KB";
  }
  else if (bytes < (1024 * 1024 * 1024))
  {
    return String((bytes / 1024.0 / 1024.0), resolution) + " MB";
  }
  else
    return "";
}

// #############################################################################################
String EncryptionType(wifi_auth_mode_t encryptionType)
{
  switch (encryptionType)
  {
  case (WIFI_AUTH_OPEN):
    return "OPEN";
  case (WIFI_AUTH_WEP):
    return "WEP";
  case (WIFI_AUTH_WPA_PSK):
    return "WPA PSK";
  case (WIFI_AUTH_WPA2_PSK):
    return "WPA2 PSK";
  case (WIFI_AUTH_WPA_WPA2_PSK):
    return "WPA WPA2 PSK";
  case (WIFI_AUTH_WPA2_ENTERPRISE):
    return "WPA2 ENTERPRISE";
  case (WIFI_AUTH_MAX):
    return "WPA2 MAX";
  default:
    return "";
  }
}

