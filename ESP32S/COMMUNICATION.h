#define JSON_PACKET_LENGTH 1024

#define KEYWORD "NITROMAX:"
#define KEYWORD_LENGTH 9

#define ASK_DATE_TIME "A0"
#define ASK_SETUP_NORMAL "A1"
#define ASK_SETUP_EMERGENCY "A2"
#define ASK_WIFI_CONNECT "A4"
#define ASK_TO_RESTART "A5"

#define INFO_SD_ERROR "I0"
#define INFO_PRINTER_ERROR "I1"
#define INFO_PRINTER_READY "I2"
#define INFO_WIFI_ERROR "I3"
#define INFO_GET_TIME_ERROR "I4"

#define INFO_SUCCESS "I5"
#define INFO_ERROR "I6"

#define RECOVERY_START "R0"
#define RECOVERY_STOP 'X'

#define CHECK_BT_CLIENT "C0"
#define BT_CLIENT_ERROR "C1"

struct Detail {
  String mode_transaksi;
  String jumlah_ban;
  String jumlah_error;
  String tekanan;
  String tekanan_awal;
};
struct Paket {
  String no_transaksi;
  String tgl_transaksi;
  String jenis_kendaraan;
  String harga;
  String jumlah_kalibrasi;
  String jumlah_cekbocor;
  struct Detail detail;
};
struct Paket paket;
