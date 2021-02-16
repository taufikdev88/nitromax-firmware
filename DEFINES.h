/*
 * Ini adalah file dimana definisi definisi untuk masing masing 
 * pin maupun variabel global dari program ini diletakkan
 */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>
#include <Keypad.h>
#include <ArduinoJson.h>

#define TRELOAD 30000
#define TREFRESH 500
#define TPRESSING 150
#define TUPDATEPRESSURE 1500

#define MOTOR_MIN_PRESSURE 17
#define MOTOR_MAX_PRESSURE 58
#define CAR_MIN_PRESSURE 17
#define CAR_MAX_PRESSURE 99

// maximum file name is 8.3
#define FILE_EMERGENCY "emg.txt"
#define FILE_TEMP "tmp.txt"
#define FILE_BACKUP "bkp.txt"
#define FILE_PRICE "prc.txt"
#define FILE_CALIB "clb.txt"

#define RELAY_OFF 1
#define RELAY_ON !RELAY_OFF

#define OUT_SELENOID 38
#define OUT_UP 40
#define OUT_DOWN 41
#define OUT_MOTOR 42
#define OUT_PAUSE 43
#define OUT_AUTO 44
#define OUT_INFLATION 45
#define OUT_CAR 46

#define IN_CAR 36
#define IN_DOWN 39
#define IN_INFLATION 34
#define IN_MOTOR 35
#define IN_UP 37
#define IN_AUTO 32
#define IN_PAUSE 33

#define SEN_PRESSURE A3

const uint8_t inputPin[7] = { IN_CAR, IN_DOWN, IN_INFLATION, IN_MOTOR, IN_UP, IN_AUTO, IN_PAUSE };
const uint8_t outputPin[8] = { OUT_CAR, OUT_DOWN, OUT_INFLATION, OUT_MOTOR, OUT_UP, OUT_AUTO, OUT_PAUSE, OUT_SELENOID };

LiquidCrystal_I2C lcd(0x27, 20, 4);
const byte customChar[] = { 0b10000,0b11000,0b11100,0b11110,0b11110,0b11100,0b11000,0b10000 };

//***************************************** KEYPAD
char customKey;
const byte ROWS = 5;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'!','@','#','*'},
  {'1','2','3','U'},
  {'4','5','6','D'},
  {'7','8','9','E'},
  {'L','0','R','G'},
};
byte rowPins[ROWS] = {11, 10, 9, 8, 7};
byte colPins[COLS] = {3, 4, 5, 6};
Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); 

File file;
String date = "";
String globalString = "";
bool emergency = false;

#define NORMAL_PRESSURE 2
#define OFFSET_PRESSURE 1
int8_t currentPressure = 0;
int8_t referencePressure = 0;

int8_t startPressure = 0;
int8_t endPressure = 0;

uint8_t ban = 0;
uint8_t err = 0;
uint8_t mode[2] = { 0 };
/*
 * mode[0] = jenis kendaraan, 0 = motor, 1 = mobil
 * mode[1] = jenis transaksi, 0 = kuras, 1 = tambah, 2 = tambal
 */

enum Step {
  step1,
  step2,
  step3,
  step4,
  step5,
  step6,
  step7
} step;

//***************************************** PROGRAM'S VARIABLE
String modeTransaksi[6] = { "1","2","3","4","5","6" };
String jenisKendaraan[2] = { "84","85" };
String stringTransaksi[3] = { "  Isi Baru Nitrogen ","  Isi Tambah Nitro..","  Tambal Ban        " };

unsigned long tRefresh = 0;
unsigned long tReload = 0;
