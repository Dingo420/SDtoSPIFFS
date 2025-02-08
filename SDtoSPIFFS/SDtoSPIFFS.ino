#include "FS.h"
#include "SD.h"
#include "SPIFFS.h"

#define SD_CS_PIN 5  // Change this to your SD card's CS pin if different

void listSPIFFS() {
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    Serial.println("\nSPIFFS File List:");
    while (file) {
        Serial.printf("File: %s, Size: %d bytes\n", file.name(), file.size());
        file = root.openNextFile();
    }
}

bool copyFileToSPIFFS(const char *srcPath, const char *dstPath) {
    File srcFile = SD.open(srcPath, FILE_READ);
    if (!srcFile) {
        Serial.printf("[ERROR] Failed to open source file: %s\n", srcPath);
        return false;
    }
    
    File dstFile = SPIFFS.open(dstPath, FILE_WRITE);
    if (!dstFile) {
        Serial.printf("[ERROR] Failed to create destination file: %s\n", dstPath);
        srcFile.close();
        return false;
    }
    
    uint8_t buffer[512];
    size_t bytesRead;
    while ((bytesRead = srcFile.read(buffer, sizeof(buffer))) > 0) {
        dstFile.write(buffer, bytesRead);
    }
    
    srcFile.close();
    dstFile.close();
    Serial.printf("[SUCCESS] Copied %s -> %s\n", srcPath, dstPath);
    return true;
}

void copyAllFilesFromSD() {
    File dir = SD.open("/16");
    if (!dir || !dir.isDirectory()) {
        Serial.println("[ERROR] Folder '16' not found on SD card");
        return;
    }
    
    File file = dir.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            String srcPath = String("/16/") + file.name();
            String dstPath = String("/") + file.name();
            
            if (copyFileToSPIFFS(srcPath.c_str(), dstPath.c_str())) {
                Serial.printf("Copied: %s -> %s\n", srcPath.c_str(), dstPath.c_str());
            } else {
                Serial.printf("[WARNING] Skipping file: %s\n", srcPath.c_str());
            }
        }
        file = dir.openNextFile();
    }
    Serial.println("File transfer complete.");
    listSPIFFS();
}

void setup() {
    Serial.begin(115200);
    
    // Initialize SD card
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("[ERROR] SD Card initialization failed!");
        return;
    }
    Serial.println("[OK] SD Card initialized");
    
    // Initialize SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("[ERROR] SPIFFS Mount Failed!");
        return;
    }
    Serial.println("[OK] SPIFFS initialized");
    
    // Start copying files
    Serial.println("Starting file transfer...");
    copyAllFilesFromSD();
}

void loop() {
    // Nothing in loop, one-time file transfer
}
