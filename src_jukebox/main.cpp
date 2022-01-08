/*
  SD card test for esp32

  This example shows how use the utility libraries

  The circuit:
    SD card attached to SPI bus as follows:
        SS    = 5;
        MOSI  = 23;
        MISO  = 19;
        SCK   = 18;

   by Mischianti Renzo <https://www.mischianti.org>

   https://www.mischianti.org
*/
// include the SD library:
#include <SD.h>
#include <SPI.h>

// WeMos D1 esp8266: D8 as standard
const int chipSelect = SS;

void printDirectory(File dir, int numTabs);

// WebDAV stuff
#include <ESPWebDAV.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include <SPIFFS.h>
#include <WiFi.h>

#define HOSTNAME "ESPWebDAV"

#ifndef STASSID
#define STASSID "Djurgarden"
#define STAPSK "palmepalme666"
#endif

FS &gfs = SD;
// #define FILESYSTEM SD

// FS &gfs = LITTLEFS;
// #define FILESYSTEM LITTLEFS
// // #define FILESYSTEM SPIFFS

// WiFiServerSecure tcp(443);
WiFiServer tcp(80);

ESPWebDAV dav;

void setup() {
  /**********************************
   * Init SD
   *********************************/
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("\nInitializing SD card...");

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!SD.begin(SS)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println(
        "* did you change the chipSelect pin to match your shield or module?");
    while (1)
      ;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial.println();
  Serial.print("Card type:         ");
  switch (SD.cardType()) {
  case CARD_NONE:
    Serial.println("NONE");
    break;
  case CARD_MMC:
    Serial.println("MMC");
    break;
  case CARD_SD:
    Serial.println("SD");
    break;
  case CARD_SDHC:
    Serial.println("SDHC");
    break;
  default:
    Serial.println("Unknown");
  }

  // print the type and size of the first FAT-type volume
  //  uint32_t volumesize;
  //  Serial.print("Volume type is:    FAT");
  //  Serial.println(SDFS.usefatType(), DEC);

  Serial.print("Card size:  ");
  Serial.println((float)SD.cardSize() / 1000);

  //   Serial.print("Total bytes: ");
  //   Serial.println(SD.totalBytes());

  //   Serial.print("Used bytes: ");
  //   Serial.println(SD.usedBytes());

  File dir = SD.open("/");
  //   printDirectory(dir, 0);
  Serial.println("SD part is ready");

  /**********************************
   * Init WebDAV
   *********************************/
  // ------------------------
  Serial.println("Start WiFi");
  WiFi.persistent(false);
  WiFi.setHostname(HOSTNAME);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  Serial.println("Connecting to " STASSID " ...");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(STASSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());

  MDNS.begin(HOSTNAME);

  //   FILESYSTEM.begin();
  tcp.begin();
  dav.begin(&tcp, &gfs);
  dav.setTransferStatusCallback([](const char *name, int percent,
                                   bool receive) {
    Serial.printf("%s: '%s': %d%%\n", receive ? "recv" : "send", name, percent);
  });

  Serial.println("WebDAV server started");
}

void loop(void) { dav.handleClient(); }

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry = dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.print(entry.size(), DEC);
      time_t lw = entry.getLastWrite();
      struct tm *tmstruct = localtime(&lw);
      Serial.printf("\tLAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",
                    (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1,
                    tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min,
                    tmstruct->tm_sec);
    }
    entry.close();
  }
}

// // Original code

// #include <ESPmDNS.h>
// #include <WiFi.h>
// //#include <SPIFFS.h>
// #include <ESPWebDAV.h>
// #include <LittleFS.h>
// #include <SD.h>
// #include <SD_MMC.h>

// #define HOSTNAME "ESPWebDAV"

// #ifndef STASSID
// #define STASSID "Djurgarden"
// #define STAPSK "palmepalme666"
// #endif

// FS &gfs = SD;
// #define FILESYSTEM SD

// // WiFiServerSecure tcp(443);
// WiFiServer tcp(80);

// ESPWebDAV dav;

// // ------------------------
// void setup() {
//   // ------------------------
//   WiFi.persistent(false);
//   WiFi.setHostname(HOSTNAME);
//   WiFi.mode(WIFI_STA);
//   Serial.begin(115200);
//   WiFi.begin(STASSID, STAPSK);
//   Serial.println("Connecting to " STASSID " ...");

//   // Wait for connection
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("");
//   Serial.print("Connected to ");
//   Serial.println(STASSID);
//   Serial.print("IP address: ");
//   Serial.println(WiFi.localIP());
//   Serial.print("RSSI: ");
//   Serial.println(WiFi.RSSI());

//   MDNS.begin(HOSTNAME);

//   FILESYSTEM.begin();
//   tcp.begin();
//   dav.begin(&tcp, &gfs);
//   dav.setTransferStatusCallback([](const char *name, int percent,
//                                    bool receive) {
//     Serial.printf("%s: '%s': %d%%\n", receive ? "recv" : "send", name,
//     percent);
//   });

//   Serial.println("WebDAV server started");
// }

// void help() {
//   Serial.printf("interactive: F/ormat D/ir C/reateFile\n");

//   Serial.printf("Heap stats: free heap: %u\n", ESP.getHeapSize());
// }

// // ------------------------
// void loop() {
//   dav.handleClient();

//   int c = Serial.read();
//   if (c > 0) {
//     if (c == 'F') {
//       Serial.println("formatting...");
//       //   if (FILESYSTEM.format())
//       //     Serial.println("Success");
//       //   else
//       //     Serial.println("Failure");
//     } else if (c == 'D') {
//       Serial.printf(">>>>>>>> dir /\n");
//       dav.dir("/", &Serial);
//       Serial.printf("<<<<<<<< dir\n");
//     } else if (c == 'C') {
//       auto f = gfs.open("/readme.md", "w");
//       f.printf("hello\n");
//       f.close();
//     } else
//       help();
//   }

//   // ------------------------
// }
