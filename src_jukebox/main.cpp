// Ohne WebDAV 84,9%
// SD + Audio + WebDAV = 87,7%

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

// My stuff
#include <algorithm>
// #include <iostream>
#include <string>
#include <vector>

// #include <ArduinoIostream.hpp>
// #include <prettyprint.hpp>

void getSortedMusicFiles(const std::string dirPath,
                         std::vector<std::string> &files);

// include the SD library:
#include <SD.h>
#include <SPI.h>

// WeMos D1 esp8266: D8 as standard
const int chipSelect = SS;

void printDirectory(File dir, int numTabs);
void help();

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

// Audio Stuff
#include <Audio.h>

#define SD_CS 5
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 18
#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26

Audio audio;

auto musicDir = "/Foo Fighters - Medicine At Midnight (2021)";
std::vector<std::string> musicFiles;
auto fileIter = musicFiles.begin();
bool isPaused = false;

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

  File dir = SD.open(musicDir);
  //   printDirectory(dir, 0);

  getSortedMusicFiles(musicDir, musicFiles);
  for (auto f : musicFiles) {
    Serial.println(f.c_str());
  }
  fileIter = musicFiles.begin();
  isPaused = false;
  //   std::cout << "musicFiles: " << musicFiles << std::endl;

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

  //******* Init Audio *****/
  Serial.println("Init audio");
  //   pinMode(SD_CS, OUTPUT);      digitalWrite(SD_CS, HIGH);
  // SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  // Serial.begin(115200);
  // SD.begin(SD_CS);
  // WiFi.disconnect();
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid.c_str(), password.c_str());
  // while (WiFi.status() != WL_CONNECTED) delay(1500);
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(21); // 0...21

  Serial.println("Play music");
  //   audio.connecttoFS(SD, "/01 - Making A Fire.mp3");
  //    audio.connecttohost("http://www.wdr.de/wdrlive/media/einslive.m3u");
  //    audio.connecttohost("http://macslons-irish-pub-radio.com/media.asx");
  audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.aac"); //
  //    128k aac
  //   audio.connecttohost(
  //   "http://mp3.ffh.de/radioffh/hqlivestream.mp3"); //  128k mp3
  //    audio.connecttohost("https://github.com/schreibfaul1/ESP32-audioI2S/raw/master/additional_info/Testfiles/sample1.m4a");
  //    // m4a
  //    audio.connecttohost("https://github.com/schreibfaul1/ESP32-audioI2S/raw/master/additional_info/Testfiles/test_16bit_stereo.wav");
  //    // wav audio.connecttospeech("Wenn die Hunde schlafen, kann der Wolf gut
  //    Schafe stehlen.", "de");
  Serial.println("Music started");
}

bool playFiles = false;
std::string nextFile() {
  ++fileIter;
  if (fileIter < musicFiles.end()) {
    return *fileIter;
  } else {
    playFiles = false;
    return "";
  }
}

void loop(void) {
  dav.handleClient();
  audio.loop();

  if (playFiles && !audio.isRunning() && !isPaused) {
    Serial.print("Play next file ");
    auto file = nextFile();
    if (file != "") {
      Serial.println(file.c_str());
      audio.connecttoFS(SD, file.c_str());
    } else {
      Serial.println("No more files");
      playFiles = false;
    }
  }

  int c = Serial.read();
  if (c != -1) {
    Serial.print("Read: ");
    Serial.println(c);
  }
  if (c > 0) {
    if (c == 'F') {
      Serial.println("formatting...");
      //   if (FILESYSTEM.format())
      //     Serial.println("Success");
      //   else
      //     Serial.println("Failure");
    } else if (c == 'D') {
      Serial.printf(">>>>>>>> dir /\n");
      dav.dir("/", &Serial);
      Serial.printf("<<<<<<<< dir\n");
    } else if (c == 'C') {
      Serial.println("Create file");
      auto f = gfs.open("/readme.md", "w");
      f.printf("hello\n");
      f.close();
    } else if (c == 'P') {
      Serial.println("Pause");
      audio.pauseResume();
      isPaused = !isPaused;
    } else if (c == 'W') {
      Serial.print("Play web radio ");
      playFiles = false;
      isPaused = false;
      audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.aac"); //
    } else if (c == 'M') {
      Serial.println("Play MP3 files ");
      fileIter = musicFiles.begin();
      playFiles = true;
      isPaused = false;
      audio.connecttoFS(SD, (*fileIter).c_str());
    } else if (c == 'N') {
      Serial.print("Play next file ");
      auto file = nextFile();
      if (file != "") {
        Serial.println(file.c_str());
        playFiles = true;
        audio.connecttoFS(SD, file.c_str());
      } else {
        Serial.println("No more files");
        playFiles = false;
      }
    } else {
      help();
    }
  }
}

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

bool hasEnding(std::string const &fullString, std::string const &ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare(fullString.length() - ending.length(),
                                    ending.length(), ending));
  } else {
    return false;
  }
}

void getSortedMusicFiles(const std::string dirPath,
                         std::vector<std::string> &files) {
  //   auto files = std::vector<std::string>{};

  auto dir = SD.open(dirPath.c_str());

  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }

    if (entry.isDirectory()) {
      // Do nothing
    } else {
      std::string curFile = entry.name();
      if (hasEnding(curFile, "mp3") || hasEnding(curFile, "aac") ||
          hasEnding(curFile, "wav")) {
        files.push_back(curFile);
      }
    }
    entry.close();
  }

  std::sort(files.begin(), files.end());
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

void help() {
  Serial.printf("interactive: F/ormat D/ir C/reateFile\n");

  Serial.printf("Heap stats: free heap: %u\n", ESP.getHeapSize());
}

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
