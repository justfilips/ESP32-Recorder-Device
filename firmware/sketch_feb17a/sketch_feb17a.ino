#include <U8g2lib.h>           // OLED display library
#include <Arduino.h>           // Arduino core library
#include <SD.h>                // Micro SD card library
#include <SPI.h>               // SPI communication library
#include <driver/i2s.h>        // ESP32 I2S audio driver
#include <FS.h>                // Filesystem support
#include <WiFi.h>              // WiFi support
#include <ESPAsyncWebServer.h> // Async Web Server for ESP32

// =======================
//       DISPLAY SETUP
// =======================
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);


// =======================
//       MENU VARIABLES
// =======================

#define menu_width 128
#define menu_height 64

static const unsigned char recordIcon[] U8X8_PROGMEM = { // RECORD ICON
  0xC0, 0x03, 0x38, 0x1C, 0xC4, 0x20, 0x32, 0x40, 0x0A, 0x40, 0x05, 0x80,
  0x85, 0x81, 0x45, 0x82, 0x41, 0xA2, 0x81, 0xA1, 0x01, 0xA0, 0x02, 0x50,
  0x02, 0x4C, 0x04, 0x23, 0x38, 0x1C, 0xC0, 0x03};

static const unsigned char recordingsIcon[] U8X8_PROGMEM = { // RECORDINGS ICON
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x0F, 0x01, 0xC8, 0x11, 0xA8,
  0x31, 0x98, 0x71, 0x80, 0x31, 0x80, 0x11, 0x98, 0x01, 0xA8, 0x01, 0xC8,
  0xFE, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const unsigned char WiFiIcon[] U8X8_PROGMEM = { // WiFi ICON
  0x00, 0x00, 0x00, 0x00, 0xF0, 0x07, 0x08, 0x08, 0x04, 0x10, 0xE2, 0x23, 
  0x10, 0x04, 0x08, 0x08, 0xC0, 0x01, 0x20, 0x02, 0x80, 0x00, 0xC0, 0x01, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

static const unsigned char batteryIcon[] U8X8_PROGMEM = { // BATTERY ICON
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x1F, 0x02, 0x10, 
  0xDA, 0x36, 0xDA, 0x36, 0xDA, 0x36, 0xDA, 0x36, 0x02, 0x10, 0xFE, 0x1F, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

static const unsigned char selectedBackground[] U8X8_PROGMEM = { // SELECTED BACKGROUND
  0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x0C, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0xF8, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03};

// Icon array for menu display
const unsigned char* bitmapIcons[4] = {
	recordIcon,
	recordingsIcon,
	WiFiIcon,
	batteryIcon
};

// Menu item labels
const int numOfItems =4;
char menu_items[numOfItems][12]{
	{"Recordings"},
	{"Record"},
	{"WiFi"},
	{"Battery"}
};

// Menu navigation variables
int item_selected = 1;
int item_sel_previous;
int item_sel_next;

// =========================================
//       RECORDINGS VARIABLES (playback)
// =========================================
#define menu_width 128
#define menu_height 64

static const unsigned char audioFileIcon[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x02, 0x40, 0x1A, 0x20, 0x22,
  0x1C, 0x4A, 0x12, 0x52, 0x12, 0x52, 0x12, 0x52, 0x1C, 0x4A, 0x20, 0x22,
  0x40, 0x1A, 0x80, 0x02, 0x00, 0x01, 0x00, 0x00};

int recording_selected;
bool recordingNotSelected = true;
int recording_sel_previous;
int recording_sel_next;

// Amplifier pins
#define I2S_SPK_PORT I2S_NUM_1 // Amplifier I2S chip
#define I2S_BCLK_SPK 20
#define I2S_LRC_SPK  21
#define I2S_DOUT_SPK 45

// Micro SD pins
#define SD_CS_PIN 14
#define SD_MOSI_PIN 11
#define SD_MISO_PIN 13
#define SD_SCK_PIN 12


#define SAMPLE_RATE 48000 // Audio quality: 48 kHz
#define WAVE_HEADER_SIZE 44 // Standard WAV header size


//Amplifier configuration
const i2s_config_t i2s_tx_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
};
i2s_pin_config_t i2s_tx_pin_config = {
  .bck_io_num = I2S_BCLK_SPK,
  .ws_io_num  = I2S_LRC_SPK,
  .data_out_num = I2S_DOUT_SPK,
  .data_in_num  = I2S_PIN_NO_CHANGE
};


// =========================================
//       RECORD VARIABLES
// =========================================

static const unsigned char microphoneIcon[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x04, 0x10, 0x00, 0x00, 0x08,
  0x88, 0x80, 0x01, 0x11, 0x44, 0xC0, 0x03, 0x22, 0x24, 0xE0, 0x07, 0x24,
  0x22, 0xF1, 0x8F, 0x44, 0x92, 0xF8, 0x1F, 0x49, 0x92, 0xF8, 0x1F, 0x49,
  0x92, 0xE0, 0x07, 0x49, 0x22, 0xF9, 0x9F, 0x44, 0x24, 0xE0, 0x07, 0x24,
  0x44, 0xF8, 0x1F, 0x22, 0x88, 0xE0, 0x07, 0x11, 0x10, 0xF8, 0x1F, 0x08,
  0x20, 0xE0, 0x07, 0x04, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00,
  0x00, 0xF2, 0x4F, 0x00, 0x00, 0xF2, 0x4F, 0x00, 0x00, 0xFA, 0x5F, 0x00,
  0x00, 0xFA, 0x5F, 0x00, 0x00, 0xF2, 0x4F, 0x00, 0x00, 0xE6, 0x67, 0x00,
  0x00, 0xC4, 0x23, 0x00, 0x00, 0x0C, 0x30, 0x00, 0x00, 0x38, 0x1C, 0x00,
  0x00, 0xE0, 0x07, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0xC0, 0x03, 0x00,
  0x00, 0xF0, 0x0F, 0x00, 0x00, 0xF8, 0x1F, 0x00};

static const unsigned char microphoneOffIcon[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x70,
  0x00, 0x80, 0x01, 0x38, 0x00, 0xC0, 0x03, 0x1C, 0x00, 0xE0, 0x07, 0x0E,
  0x00, 0xF0, 0x0F, 0x07, 0x00, 0xF8, 0x8F, 0x03, 0x00, 0xF8, 0xCF, 0x01,
  0x00, 0xE0, 0xE7, 0x00, 0x00, 0xF8, 0x73, 0x00, 0x00, 0xE0, 0x39, 0x00,
  0x00, 0xF8, 0x1C, 0x00, 0x00, 0x60, 0x0E, 0x00, 0x00, 0x38, 0x07, 0x00,
  0x00, 0x80, 0x03, 0x00, 0x00, 0xCF, 0xF9, 0x00, 0x00, 0xE7, 0xFC, 0x00,
  0x00, 0x70, 0x4E, 0x00, 0x00, 0x38, 0x4F, 0x00, 0x00, 0x9C, 0x5F, 0x00,
  0x00, 0xCE, 0x5F, 0x00, 0x00, 0xE7, 0x4F, 0x00, 0x80, 0xE3, 0x67, 0x00,
  0xC0, 0xC1, 0x23, 0x00, 0xE0, 0x0C, 0x30, 0x00, 0x70, 0x38, 0x1C, 0x00,
  0x38, 0xE0, 0x07, 0x00, 0x1C, 0xC0, 0x03, 0x00, 0x0E, 0xC0, 0x03, 0x00,
  0x07, 0xF0, 0x0F, 0x00, 0x03, 0xF8, 0x1F, 0x00 };

// Microphone Pins
#define I2S_MIC_PORT I2S_NUM_0 // Microphone I2S chip
#define I2S_BCK_PIN 4
#define I2S_WS_PIN 5
#define I2S_SD_PIN 6

bool recordScreenDrawn = false; // Flag to avoid redrawing display each loop
File file;                     // File handle for SD writing
bool isRecording = false;      // Track recording state
int fileIndex = 1;             // File numbering
char filename[32];             // Current recording filename

// I2S configuration for microphone
const i2s_config_t i2s_config = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
  .sample_rate = SAMPLE_RATE,
  .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
  .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
  .communication_format = I2S_COMM_FORMAT_STAND_I2S,
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
  .dma_buf_count = 8,
  .dma_buf_len = 1024,
  .use_apll = false,
  .tx_desc_auto_clear = false,
  .fixed_mclk = 0
};

const i2s_pin_config_t pin_config = {
  .bck_io_num = I2S_BCK_PIN,
  .ws_io_num = I2S_WS_PIN,
  .data_out_num = I2S_PIN_NO_CHANGE,
  .data_in_num = I2S_SD_PIN
};

// =============================
//       WIFI VARIABLES
// =============================
bool wifiStarted = false;
const char* ssid = "ESP32_WAV";
const char* password = "12345678";
AsyncWebServer server(80); // Web server on port 80

// =============================
//       GLOBAL VARIABLES
// =============================
#define UP_PIN    10
#define DOWN_PIN  7
#define OK_PIN    17
#define LEFT_PIN  16
#define RIGHT_PIN 15

// Button state tracking
int button_up_clicked = 0;
int button_down_clicked = 0;
int button_left_clicked = 0;
int button_right_clicked = 0;
int button_ok_clicked = 0;

// Screen state enum
enum ScreenState {
	SCREEN_MENU,
	SCREEN_RECORDINGS,
	SCREEN_RECORD,
	SCREEN_WiFi,
	SCREEN_BATTERY
};

ScreenState currentScreen = SCREEN_MENU;

// ==================================
//   RECORDINGS PLAYBACK FUNCTIONS
// ==================================
int numOfRecordings = 0;
char recordings_items[1000][15]; // max number of recordings

// Load all WAV files from SD card
void loadRecordings() {
    numOfRecordings = 0; // resets num of recordings to 0
    int fileIndex = 1; // file format starts at /rec1.wav
    // check if a file with current index exists on sd card
    while (SD.exists("/rec" + String(fileIndex) + ".wav")) {
        // adds a recording label to array recordings_items
        sprintf(recordings_items[numOfRecordings], "Recording %d", fileIndex);
        numOfRecordings++;
        fileIndex++;
    }
}

// Plays WAV file via I2S speaker (index is recording_selected)
void playRecording(int index) {
    char playFile[32]; // file name
    sprintf(playFile, "/rec%d.wav", index + 1);

    File audioFile = SD.open(playFile, FILE_READ);
    if (!audioFile) {
        Serial.println("Failed to open file!");
        return;
    }

    audioFile.seek(WAVE_HEADER_SIZE); // skip WAV header

    uint8_t buffer[1024]; // each element is 1 byte
    size_t bytesRead;
    size_t bytesWritten;
    float masterGain = 0.6f;
    // if read() returns 0 then no more bytes to read
    while ((bytesRead = audioFile.read(buffer, sizeof(buffer))) > 0) {
      // one sample is one number (mono)
        int16_t* samples = (int16_t*)buffer; // convert to 16 bit samples because WAV file reads in 16bits not 8 bits

        // loop through each sample (16 bit number)
        for (size_t i = 0; i < bytesRead / 2; i++) {// bytesRead has 1024 bytes max, but one sample is 2 bytes, not one, so we devide by 2
            samples[i] = (int16_t)(samples[i] * masterGain); // (int16_t) converts back to 16 bit integer
        }

        i2s_write(I2S_SPK_PORT, samples, bytesRead, &bytesWritten, portMAX_DELAY);
    }


    audioFile.close();
}

// ==================================
//       RECORD AND SAVE
// ==================================

// Write WAV header to SD card
void writeWavHeader(File &file, int sampleRate, int bitsPerSample, int channels, int dataSize) {
  byte header[WAVE_HEADER_SIZE];
  int fileSize = dataSize + WAVE_HEADER_SIZE - 8;
  int byteRate = sampleRate * channels * (bitsPerSample / 8);

  header[0] = 'R';
  header[1] = 'I';
  header[2] = 'F';
  header[3] = 'F';
  header[4] = (byte)(fileSize & 0xFF);
  header[5] = (byte)((fileSize >> 8) & 0xFF);
  header[6] = (byte)((fileSize >> 16) & 0xFF);
  header[7] = (byte)((fileSize >> 24) & 0xFF);
  header[8] = 'W';
  header[9] = 'A';
  header[10] = 'V';
  header[11] = 'E';
  header[12] = 'f';
  header[13] = 'm';
  header[14] = 't';
  header[15] = ' ';
  header[16] = 16;
  header[17] = 0;
  header[18] = 0;
  header[19] = 0;  // PCM Chunk Size
  header[20] = 1;
  header[21] = 0;  // PCM Format
  header[22] = channels;
  header[23] = 0;
  header[24] = (byte)(sampleRate & 0xFF);
  header[25] = (byte)((sampleRate >> 8) & 0xFF);
  header[26] = (byte)((sampleRate >> 16) & 0xFF);
  header[27] = (byte)((sampleRate >> 24) & 0xFF);
  header[28] = (byte)(byteRate & 0xFF);
  header[29] = (byte)((byteRate >> 8) & 0xFF);
  header[30] = (byte)((byteRate >> 16) & 0xFF);
  header[31] = (byte)((byteRate >> 24) & 0xFF);
  header[32] = (byte)(channels * (bitsPerSample / 8));
  header[33] = 0;  // Block Align
  header[34] = bitsPerSample;
  header[35] = 0;  // Bits Per Sample
  header[36] = 'd';
  header[37] = 'a';
  header[38] = 't';
  header[39] = 'a';
  header[40] = (byte)(dataSize & 0xFF);
  header[41] = (byte)((dataSize >> 8) & 0xFF);
  header[42] = (byte)((dataSize >> 16) & 0xFF);
  header[43] = (byte)((dataSize >> 24) & 0xFF);

  file.write(header, WAVE_HEADER_SIZE);
}

// Creates the file in SD
void startRecording() {
  fileIndex = 1;   // RESET FIRST
  while (SD.exists("/rec" + String(fileIndex) + ".wav")) { // looks for a free file name
    fileIndex++;
  }
  sprintf(filename, "/rec%d.wav", fileIndex); // creates the filename

  Serial.print("Record is Start: ");
  Serial.println(filename);

  file = SD.open(filename, FILE_WRITE); // puts file in write mode
  if (!file) {
    Serial.println("File is not Open!");
    return;
  }
  // 16 bits per sample, 1 num of channels, 0 is datalength
  writeWavHeader(file, SAMPLE_RATE, 16, 1, 0); // writes the header in file to make file valid and writable

  isRecording = true;
}

// Stop recording and finalize WAV file
void stopRecording() {
  Serial.println("Recors is Stopped...");
  isRecording = false;

  unsigned long fileSize = file.size() - WAVE_HEADER_SIZE;
  file.seek(0);
  writeWavHeader(file, SAMPLE_RATE, 16, 1, fileSize); // UPDATES header with the real file size AFTER RECORDING
  file.close();

  Serial.println("Record is Complete!");
  // digitalWrite(LED_PIN, LOW);
}

// Display recording UI
void drawRecordItems(){
	const char* text = "Recording";
	u8g2.setFont(u8g2_font_7x14B_mr);
	int16_t textWidth = u8g2.getStrWidth(text);
	int16_t x = (128 - textWidth) / 2;
	u8g2.drawStr(x, 15, text);


	// draw microphone icon
	u8g2.drawXBMP((128-32)/2, 26, 32, 32, microphoneIcon);
}
void drawRecordOffItems(){
	const char* text = "Not Recording";
	u8g2.setFont(u8g2_font_7x14B_mr);
	int16_t textWidth = u8g2.getStrWidth(text);
	int16_t x = (128 - textWidth) / 2;
	u8g2.drawStr(x, 15, text);


	// draw microphone icon
	u8g2.drawXBMP((128-32)/2, 26, 32, 32, microphoneOffIcon);
}

// Handle recording logic and microphone buffer
void handleRecord() {
    // Reset OK button state
    if (digitalRead(OK_PIN) == HIGH) {
        button_ok_clicked = 0;
    }

    // BACK button
    if (digitalRead(LEFT_PIN) == LOW && button_left_clicked == 0) {
        button_left_clicked = 1;
        currentScreen = SCREEN_MENU;
    }
    if (digitalRead(LEFT_PIN) == HIGH) {
        button_left_clicked = 0;
    }

    // START / STOP RECORDING
    if (digitalRead(OK_PIN) == LOW && button_ok_clicked == 0) {
        button_ok_clicked = 1;
        if (isRecording) {
            stopRecording();
        } else {
            startRecording();
        }
        while (digitalRead(OK_PIN) == LOW);
    }

    if (isRecording) {
        // print every 500ms
        static unsigned long lastPrint = 0;
        if (millis() - lastPrint > 500) {
            Serial.print("Recording... ");
            Serial.println(filename);
            lastPrint = millis();
        }

        // Read samples from I2S
        size_t bytesRead = 0;
        int16_t i2sBuffer[256]; // temporary storage for audio samples from mic. stores 256 2 byte elements, so 256*2=512 bytes
        i2s_read(I2S_MIC_PORT, (void*)i2sBuffer, sizeof(i2sBuffer), &bytesRead, portMAX_DELAY); //reads audio from mic into a buffer
        size_t samplesRead = bytesRead / 2; // how many samples were recorded in one buffer

        float attack = 0.005f;    // reduce gain fast if too loud
        float release = 0.005f;   // restore gain slowly
        float targetPeak = 0.7f;  // desired max amplitude in float [-1,1]
        static float gain = 0.9f; // persistent between buffers. static remembers its value between calls

        for (size_t i = 0; i < samplesRead; i++) {
            // Convert to float [-1,1]
            float sample = i2sBuffer[i] / 32768.0f; // get it between -1 and 1

            // Measure peak
            float absSample = fabs(sample); // get the absolute value of the loudness

            // Adjust gain dynamically
            if (absSample * gain > targetPeak) {
                gain -= attack;    // reduce gain quickly if too loud
                if (gain < 0.1f) gain = 0.1f; // prevent total silence
            } else {
                gain += release;   // slowly restore gain
                if (gain > 5.0f) gain = 5.0f; // prevent huge boost
            }

            // Apply gain
            sample *= gain;

            // Soft clip
            if (sample > 1.0f) sample = 1.0f;
            if (sample < -1.0f) sample = -1.0f;

            // Convert back to int16
            i2sBuffer[i] = (int16_t)(sample * 32767.0f);
        }

        // Write buffer to SD safely
        size_t written = file.write((uint8_t*)i2sBuffer, bytesRead); // converts i2sbuffer back to 1 byte elements and write in sd card
        if (written != bytesRead) {
            Serial.println("Warning: SD write incomplete!");
        }

        // Draw recording UI
        if (!recordScreenDrawn) {
            u8g2.clearBuffer();
            drawRecordItems();
            u8g2.sendBuffer();
            recordScreenDrawn = true;
        }

    } else {
        // Not recording: draw normal UI
        u8g2.clearBuffer();
        drawRecordOffItems();
        u8g2.sendBuffer();

        // Reset drawn flag for next recording session
          recordScreenDrawn = false;
    }
}

// =============================
//       WIFI FUNCTIONS
// =============================

String listFiles() {
    String html = "<h1>ESP32 WAV Files</h1><ul>";
    File root = SD.open("/");
    if(!root || !root.isDirectory()) return "<p><b>Failed to read SD root.</b></p>";

    File file = root.openNextFile();
    while(file) {
        if(!file.isDirectory()) {
            String name = file.name();
            html += "<li><a href='/download?file=" + name + "'>" + name + "</a> (" + String(file.size()) + " bytes)</li>";
        }
        file.close();
        file = root.openNextFile();
    }
    html += "</ul>";
    return html;
}

void startWiFi(){
  delay(100);
    SPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
    if(!SD.begin(SD_CS_PIN, SPI, 500000)) { // 0.5 MHz for stable low-power
        while(true); // halt if SD fails
    }

    //Wi-Fi AP minimal power 
    WiFi.mode(WIFI_AP); // set wifi as access point
    WiFi.softAP(ssid, password); // start wifi network
    WiFi.setTxPower(WIFI_POWER_MINUS_1dBm); //lower transmit power
    WiFi.setSleep(true); // enable modem sleep when idle

    //Root page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send(200, "text/html", listFiles());
    });

    //Download handler
    server.on("/download", HTTP_GET, [](AsyncWebServerRequest* request){
        if(!request->hasParam("file")) { //if url dosent have "file"
            request->send(400, "text/plain", "Missing file parameter");
            return;
        }

        String fileName = request->getParam("file")->value(); //extract file name
        String path = "/" + fileName;

        //check if file exists on sd card
        if(!SD.exists(path)) {
            request->send(404, "text/plain", "File not found");
            return;
        }

        File f = SD.open(path, FILE_READ); // open file for reading
        if(!f){
            request->send(500, "text/plain", "Failed to open file");
            return;
        }

        size_t fileSize = f.size(); // get file size in bytes

        // create streaming response
        AsyncWebServerResponse *response = request->beginResponse(
            "audio/wav", fileSize,
            // [f] so file object exists in lambda
            // buffer is where lambda puts the file bytes for the server to send
            // maxLen is maximum number of bytes server can handle in one chunk
            // index is current position in the file (updates automatically)
            [f](uint8_t *buffer, size_t maxLen, size_t index) mutable -> size_t {
                f.seek(index);
                size_t chunk = (maxLen > 256) ? 256 : maxLen; // limit a chunk to 256 bytes for stability
                return f.read(buffer, chunk); // buffer stores the bytes i read
            }
        );
        // attachment to force download
        response->addHeader("Content-Disposition", "attachment; filename=\"" + fileName + "\"");
        request->send(response);
    });

    server.begin();
}

// Enter low-power WiFi mode (shuts down audio/display)
void enterLowPowerWiFiMode() {
    // Uninstall I2S drivers (mic + speaker)
    i2s_driver_uninstall(I2S_MIC_PORT);
    i2s_driver_uninstall(I2S_SPK_PORT);

    // Turn off OLED
    u8g2.setPowerSave(1);
    // Start server
    startWiFi(); // your existing function
}

// Exit WiFi mode and restore audio/display
void exitLowPowerWiFiMode() {
    // Stop WiFi
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);

    // Reinstall I2S drivers
    i2s_driver_install(I2S_MIC_PORT, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_MIC_PORT, &pin_config);
    i2s_zero_dma_buffer(I2S_MIC_PORT);

    i2s_driver_install(I2S_SPK_PORT, &i2s_tx_config, 0, NULL);
    i2s_set_pin(I2S_SPK_PORT, &i2s_tx_pin_config);
    i2s_zero_dma_buffer(I2S_SPK_PORT);

    // Turn display back on
    u8g2.setPowerSave(0);

    wifiStarted = false;
}


// =============================
//       BATTERY FUNCTIONS
// =============================
#define BATTERY_PIN 2
#define ADC_READINGS 10 // average of 10 readings
const float VOLT_DIV_RATIO = 2.0;
const float ADC_REF_VOLTAGE = 3.3; //esp32 can only safely read up to 3.3v
const int ADC_MAX = 4095; //12 bit ADC
const float CALIBRATION_FACTOR = 1.09;

// Read battery voltage with calibration
float readBatteryVoltage() {
    analogReadResolution(12); // set adc resolution to 12 bits (0-4095)
    analogSetAttenuation(ADC_11db); // ADC_11db lets esp32 read up to 3.3v at the pin

    long totalADC = 0; // sum of ADC readings
    for (int i = 0; i < ADC_READINGS; i++) {
        totalADC += analogRead(BATTERY_PIN);
        delay(5);
    }
    float avgADC = totalADC / (float)ADC_READINGS;
    float voltageAtADC = (avgADC / ADC_MAX) * ADC_REF_VOLTAGE; // get the halved voltage
    float batteryVolt = voltageAtADC * VOLT_DIV_RATIO; //multiply by 2 to get the real voltage
    batteryVolt *= CALIBRATION_FACTOR;  // fine-tune with real measurements
    return batteryVolt;
}

// Convert voltage to realistic Li-ion battery percentage using table + interpolation
int readBatteryPercent() {
    float voltage = readBatteryVoltage();

    // Table from 3.0V to 4.2V
    const float voltages[] = {3.0, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9, 4.0, 4.1, 4.2};
    const int percents[]  = {0,    5,   10,  20,  35,  50,  65,  80,  90,  95, 100};
    const int n = sizeof(voltages)/sizeof(voltages[0]);

    if (voltage <= voltages[0]) return percents[0]; // if <=3.0V return 0%
    if (voltage >= voltages[n-1]) return percents[n-1]; if >=4.2V return 100%

    // Linear interpolation between table points
    for (int i = 1; i < n; i++) {
        //find the two voltage levels current voltage falls between
        if (voltage < voltages[i]) {
            float v1 = voltages[i-1];
            float v2 = voltages[i];
            int p1 = percents[i-1];
            int p2 = percents[i];
            return p1 + (int)((voltage - v1) / (v2 - v1) * (p2 - p1));
        }
    }
    return 0; // fallback
}

// Display battery on OLED
void drawBatteryOnOLED() {
    u8g2.clearBuffer();

    // Battery percentage
    int batteryPercent = readBatteryPercent();
    u8g2.setFont(u8g2_font_7x14B_mr);
    char buf[20];
    sprintf(buf, "Battery: %d%%", batteryPercent);
    u8g2.drawStr(25, 30, buf);

    // Draw battery bar
    int barX = 10, barY = 40, barWidth = 100, barHeight = 10;
    u8g2.drawFrame(barX, barY, barWidth, barHeight);
    u8g2.drawBox(barX + barWidth, barY + 2, 4, barHeight-4);

    // Map battery % to fill width
    int fillWidth = map(batteryPercent, 0, 100, 0, barWidth);
    u8g2.drawBox(barX, barY, fillWidth, barHeight);

    u8g2.sendBuffer();
}

// Handle battery screen interactions
void handleBattery() {
    // BACK BUTTON
    if (digitalRead(LEFT_PIN) == LOW && button_left_clicked == 0) {
        button_left_clicked = 1;
        currentScreen = SCREEN_MENU;
    }
    if (digitalRead(LEFT_PIN) == HIGH) button_left_clicked = 0;

    // DRAW battery info
    drawBatteryOnOLED();
}
// ==================================
//       MENU FUNCTIONS
// ==================================

void drawMenuItems(){
	// Selected Item Frame
	u8g2.drawXBMP(0, 22, 128, 21, selectedBackground);

	// Previous item
	u8g2.drawXBMP(5, 2, 16, 16, bitmapIcons[item_sel_previous]); // icon
	u8g2.setFont(u8g2_font_7x14_mr);
	u8g2.drawStr(26,15,menu_items[item_sel_previous]); // label

	// Selected item
	u8g2.drawXBMP(5, 24, 16, 16, bitmapIcons[item_selected]); // icon
	u8g2.setFont(u8g2_font_7x14B_mr);
	u8g2.drawStr(26,37,menu_items[item_selected]); // label

	// Next item
	u8g2.drawXBMP(5, 46, 16, 16, bitmapIcons[item_sel_next]); // icon
	u8g2.setFont(u8g2_font_7x14_mr);
	u8g2.drawStr(26,59,menu_items[item_sel_next]); // label
}

void handleMenu(){
	// OK BUTTON "currentScreen" CONFIGURATION
	if (digitalRead(OK_PIN) == LOW && button_ok_clicked == 0) {
		button_ok_clicked = 1;
		if(item_selected == 0){ // 0 is "Recordings" label and icon
			currentScreen = SCREEN_RECORDINGS;
      loadRecordings(); // only once
      recording_selected = numOfRecordings - 1;
			return;
		}else if (item_selected == 1) { // 1 is "Record" label and icon
			currentScreen = SCREEN_RECORD;
			return;
		}else if(item_selected == 2){ // 2 is "WiFi" label and icon
			currentScreen = SCREEN_WiFi;
			return;
		}else if(item_selected == 3){ // 3 is "Battery" label and icon
			currentScreen = SCREEN_BATTERY;
			return;
		}
	}
	if (digitalRead(OK_PIN) == HIGH) {
		button_ok_clicked = 0;
	}
  
	// UP BUTTON "item_selected" CONFIGURATION
	if((digitalRead(UP_PIN) == LOW) && (button_up_clicked == 0)){ // UP is pressed
		item_selected = item_selected - 1;
		button_up_clicked = 1;
		if(item_selected < 0){
			item_selected = numOfItems - 1;
		}
	}
	if (digitalRead(UP_PIN) == HIGH) {
		button_up_clicked = 0;
	}


	// DOWN BUTTON "item_selected" CONFIGURATION
	if((digitalRead(DOWN_PIN) == LOW) && (button_down_clicked == 0)){ // DOWN is pressed
		item_selected = item_selected + 1;
		button_down_clicked = 1;
		if(item_selected >= numOfItems){
			item_selected = 0;
		}
	}
	if (digitalRead(DOWN_PIN) == HIGH) {
		button_down_clicked = 0;
	}

	// PREVIOUS ITEM CONFIGURATION
	item_sel_previous = item_selected - 1;
	if(item_sel_previous < 0) item_sel_previous = numOfItems - 1;

	// NEXT ITEM CONFIGURATION
	item_sel_next = item_selected + 1;
	if(item_sel_next >= numOfItems) item_sel_next = 0;

	// DRAW
	u8g2.clearBuffer();
	drawMenuItems();
  u8g2.sendBuffer();

}

// Draw recordings list on OLED
void drawRecordingsItems(char recordings_items[][15]){
	// Selected Item Frame
	u8g2.drawXBMP(0, 22, 128, 21, selectedBackground);

	// Previous item
	u8g2.drawXBMP(5, 2, 16, 16, audioFileIcon); // icon
	u8g2.setFont(u8g2_font_7x14_mr);
	u8g2.drawStr(26,15,recordings_items[recording_sel_previous]); // label

	// Selected item
	u8g2.drawXBMP(5, 24, 16, 16, audioFileIcon); // icon
	u8g2.setFont(u8g2_font_7x14B_mr);
	u8g2.drawStr(26,37,recordings_items[recording_selected]); // label

	// Next item
	u8g2.drawXBMP(5, 46, 16, 16, audioFileIcon); // icon
	u8g2.setFont(u8g2_font_7x14_mr);
	u8g2.drawStr(26,59,recordings_items[recording_sel_next]); // label
}
void handleRecordings() {
	// BACK BUTTON "currentScreen" CONFIGURATION
    if(digitalRead(LEFT_PIN) == LOW && button_left_clicked == 0) {
        button_left_clicked = 1;
        currentScreen = SCREEN_MENU;
    }
    if(digitalRead(LEFT_PIN) == HIGH) {
        button_left_clicked = 0;
    }

    // PLAY SELECTED RECORDING
  if (digitalRead(OK_PIN) == LOW && button_ok_clicked == 0) {
      button_ok_clicked = 1;

      playRecording(recording_selected);
  }
	// MAKE OK BUTTON STATE 0 AGAIN
  if (digitalRead(OK_PIN) == HIGH) {
      button_ok_clicked = 0;
  }
    if(recordingNotSelected){
      recording_selected = numOfRecordings-1; // Start with latest recording selected
      recordingNotSelected = false;
    }

	// UP BUTTON "recording_selected" CONFIGURATION
	if((digitalRead(UP_PIN) == LOW) && (button_up_clicked == 0)){ // UP is pressed
		recording_selected = recording_selected + 1;
		button_up_clicked = 1;
		if(recording_selected >= numOfRecordings){
			recording_selected = 0;
		}
	}
	if (digitalRead(UP_PIN) == HIGH) {
		button_up_clicked = 0;
	}


	// DOWN BUTTON "item_selected" CONFIGURATION
	if((digitalRead(DOWN_PIN) == LOW) && (button_down_clicked == 0)){ // DOWN is pressed
		recording_selected = recording_selected - 1;
		button_down_clicked = 1;
		if(recording_selected < 0){
			recording_selected = numOfRecordings - 1;
		}
	}
	if (digitalRead(DOWN_PIN) == HIGH) {
		button_down_clicked = 0;
	}


	// PREVIOUS ITEM CONFIGURATION
	recording_sel_previous = recording_selected + 1;
	if(recording_sel_previous >= numOfRecordings) recording_sel_previous = 0;

	// NEXT ITEM CONFIGURATION
	recording_sel_next = recording_selected - 1;
	if(recording_sel_next < 0) recording_sel_next = numOfRecordings - 1;


    // DRAW
    u8g2.clearBuffer();
		drawRecordingsItems(recordings_items);
    u8g2.sendBuffer();
}


// Draw WiFi screen
void drawWiFiItems(){
	u8g2.setFont(u8g2_font_7x14B_mr);
	u8g2.drawStr(20, 20, "WiFi");

	u8g2.setFont(u8g2_font_6x10_tr);
	u8g2.drawStr(10, 45, "Press LEFT to go back");
}

// Draw battery info screen
void drawBatteryItems(){
	u8g2.setFont(u8g2_font_7x14B_mr);
	u8g2.drawStr(25, 20, "Battery");

	u8g2.setFont(u8g2_font_6x10_tr);
	u8g2.drawStr(10, 45, "Press LEFT to go back");
}


// =============================
//       SETUP AND LOOP
// =============================
void setup(void) {
	Serial.begin(115200);
  u8g2.begin();

  // Configure buttons
	pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(OK_PIN, INPUT_PULLUP);
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);

  // Initialize SD card
	SPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN); // begin SD card
	if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card not Initialize!");
  }
  Serial.println("SD Card Ready.");

  // Setup I2S for mic and speaker
	i2s_driver_install(I2S_MIC_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_MIC_PORT, &pin_config);
  i2s_zero_dma_buffer(I2S_MIC_PORT);

  i2s_driver_install(I2S_SPK_PORT, &i2s_tx_config, 0, NULL);
  i2s_set_pin(I2S_SPK_PORT, &i2s_tx_pin_config);
  i2s_zero_dma_buffer(I2S_SPK_PORT);

  Serial.println("System is Ready. Click the utton for Recording.");

}


void loop() {
    switch(currentScreen) {
        case SCREEN_MENU:
            handleMenu();
            break;

				case SCREEN_RECORDINGS:
						handleRecordings();
						break;

        case SCREEN_RECORD:
            handleRecord();
            break;

        case SCREEN_WiFi:
            if (!wifiStarted) {
              enterLowPowerWiFiMode(); // shuts down display & I2S, starts WiFi
              wifiStarted = true;
          }

          // Handle back button to exit WiFi mode
          if (digitalRead(LEFT_PIN) == LOW && button_left_clicked == 0) {
              button_left_clicked = 1;
              exitLowPowerWiFiMode();  // restores I2S, display, etc.
              currentScreen = SCREEN_MENU;
              wifiStarted = false;
          }
          if (digitalRead(LEFT_PIN) == HIGH) {
              button_left_clicked = 0;
          }
          break;

        case SCREEN_BATTERY:
            handleBattery();
            break;
    }
}
