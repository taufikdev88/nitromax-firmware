void printRPP02N(){
  printer.justify('C');
  printer.boldOn();
  printer.println(F("NitroMax"));
  printer.boldOff();
  printer.feed(1);

  printer.underlineOn();
  printer.justify('L');
  printer.print(F("Tanggal: "));
  printer.println(paket.tgl_transaksi);
  printer.print(F("No Transaksi: "));
  printer.println(paket.no_transaksi);
  printer.underlineOff();
  printer.println(F("Nama/No Outlet: "));
  printer.feed(1);
  printer.println(F("NoPol:"));
  printer.feed(2);

  printer.println(F("Jenis Kendaraan:"));
  printer.justify('R');
  printer.print((String) "("+paket.jenis_kendaraan+") ");
  printer.println(paket.jenis_kendaraan == "84" ? "Motor" : "Mobil");
  printer.justify('L');
  printer.println(F("Mode Transaksi:"));
  printer.justify('R');
  printer.print((String) "("+paket.detail.mode_transaksi+") ");
  switch(paket.detail.mode_transaksi.toInt()){
    case 1:
    case 2: printer.println(F("Isi Baru Nitrogen")); break;
    case 3:
    case 4: printer.println(F("Isi Tambah Nitrogen")); break;
    case 5:
    case 6: printer.println(F("Tambal Ban Tubles")); break;
  }
  printer.justify('L');
  if(paket.detail.mode_transaksi.toInt() < 5)
    printer.println(F("Jumlah Ban:"));
  else
    printer.println(F("Jumlah Lubang:"));
  printer.justify('R');
  printer.println(paket.detail.jumlah_ban);
  printer.justify('L');
  printer.println(F("Tekanan Udara (B/A):"));
  printer.justify('R');
  printer.print(paket.detail.tekanan_awal);
  printer.print('/');
  printer.println(paket.detail.tekanan);
  printer.justify('L');
  printer.feed(1);
  printer.println("Total");
  printer.justify('R');
  printer.println(paket.harga);
  printer.justify('L');

  printer.feed(2);
  printer.justify('C');
  printer.println(F("Terima Kasih"));
  printer.feed(3);
}
