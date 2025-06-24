#include <bits/stdc++.h>
#include <chrono>
using namespace std;
using namespace std::chrono;

struct Tanaman {
    string nama;
    int harga_beli;
    int harga_jual;
    int waktu_panen;
    int musim; // 0 = KEMARAU, 1 = HUJAN
    int keuntungan() const { return harga_jual - harga_beli; }
};

struct Aksi {
    int hari;
    int petak;
    int id_tanaman;
    int hari_panen;
};

int baris, kolom, jumlah_tanaman, jumlah_hari, modal_awal;
vector<Tanaman> daftar_tanaman;

struct HasilMemo {
    int total_keuntungan;
    vector<Aksi> daftar_aksi;
};

map<string, HasilMemo> memo;

string buat_kunci(int hari, int modal, const vector<int>& ketersediaan) {
    string kunci = to_string(hari) + "," + to_string(modal) + ",";
    for (int x : ketersediaan) {
        kunci += to_string(x) + ",";
    }
    return kunci;
}

HasilMemo cari(int hari, int modal, vector<int>& ketersediaan) {
    if (hari > jumlah_hari) return {0, {}};
    
    string kunci = buat_kunci(hari, modal, ketersediaan);
    if (memo.count(kunci)) return memo[kunci];

    int hari_per_musim = jumlah_hari / 2;
    int sisa = jumlah_hari % 2;
    int batas_kemarau = hari_per_musim + (sisa > 0 ? 1 : 0);
    int musim = (hari <= batas_kemarau) ? 0 : 1;

    HasilMemo hasil_terbaik = {0, {}};

    function<void(int, int, vector<int>&, vector<Aksi>&)> coba_tanam = 
        [&](int modal_sekarang, int indeks_petak, vector<int>& ketersediaan_sekarang, vector<Aksi>& aksi_sekarang) {
            
            HasilMemo hasil_mendatang = cari(hari + 1, modal_sekarang, ketersediaan_sekarang);
            int keuntungan_total = hasil_mendatang.total_keuntungan;

            for (const auto& aksi : aksi_sekarang) {
                keuntungan_total += daftar_tanaman[aksi.id_tanaman].keuntungan();
            }

            if (keuntungan_total > hasil_terbaik.total_keuntungan) {
                hasil_terbaik.total_keuntungan = keuntungan_total;
                hasil_terbaik.daftar_aksi = hasil_mendatang.daftar_aksi;
                for (const auto& aksi : aksi_sekarang) {
                    hasil_terbaik.daftar_aksi.push_back(aksi);
                }
            }

            for (int petak = indeks_petak; petak < baris * kolom; petak++) {
                if (ketersediaan_sekarang[petak] < hari) {
                    for (int i = 0; i < jumlah_tanaman; i++) {
                        const auto& t = daftar_tanaman[i];
                        if (t.musim == musim && modal_sekarang >= t.harga_beli &&
                            hari + t.waktu_panen <= jumlah_hari) {
                            
                            int backup = ketersediaan_sekarang[petak];
                            ketersediaan_sekarang[petak] = hari + t.waktu_panen;
                            aksi_sekarang.push_back({hari, petak, i, hari + t.waktu_panen});
                            coba_tanam(modal_sekarang - t.harga_beli, petak + 1, ketersediaan_sekarang, aksi_sekarang);
                            ketersediaan_sekarang[petak] = backup;
                            aksi_sekarang.pop_back();
                        }
                    }
                }
            }
        };

    vector<Aksi> kosong;
    coba_tanam(modal, 0, ketersediaan, kosong);

    return memo[kunci] = hasil_terbaik;
}

void tampilkan_lahan(const vector<Aksi>& aksi, int hari_target, string nama_musim) {
    cout << "\n=== Lahan Hari ke-" << hari_target << " (" << nama_musim << ") ===" << endl;
    vector<vector<string>> lahan(baris, vector<string>(kolom, "  .  "));
    for (const auto& a : aksi) {
        if (a.hari <= hari_target && a.hari_panen > hari_target) {
            int r = a.petak / kolom;
            int c = a.petak % kolom;
            string n = daftar_tanaman[a.id_tanaman].nama;
            if (n.length() > 4) n = n.substr(0, 4);
            while (n.length() < 4) n += " ";
            lahan[r][c] = " " + n + " ";
        }
    }
    cout << "  ";
    for (int j = 0; j < kolom; j++) cout << "+-----";
    cout << "+\n";

    for (int i = 0; i < baris; i++) {
        cout << "  ";
        for (int j = 0; j < kolom; j++) {
            cout << "|" << lahan[i][j];
        }
        cout << "|\n";
        cout << "  ";
        for (int j = 0; j < kolom; j++) cout << "+-----";
        cout << "+\n";
    }

    cout << "Keterangan: '.' = kosong, nama = tanaman aktif\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> baris >> kolom >> jumlah_tanaman >> jumlah_hari >> modal_awal;
    daftar_tanaman.resize(jumlah_tanaman);
    
    cout << "Masukkan tanaman (nama beli jual panen musim):\n";
    for (int i = 0; i < jumlah_tanaman; i++) {
        string musim_str;
        cin >> daftar_tanaman[i].nama
            >> daftar_tanaman[i].harga_beli
            >> daftar_tanaman[i].harga_jual
            >> daftar_tanaman[i].waktu_panen
            >> musim_str;
        daftar_tanaman[i].musim = (musim_str == "KEMARAU") ? 0 : 1;
    }

    vector<int> lahan_tersedia(baris * kolom, 0);
    auto mulai = high_resolution_clock::now();
    HasilMemo hasil = cari(1, modal_awal, lahan_tersedia);
    auto selesai = high_resolution_clock::now();
    auto durasi = duration_cast<milliseconds>(selesai - mulai);

    int hari_per_musim = jumlah_hari / 2;
    int sisa = jumlah_hari % 2;
    int batas_kemarau = hari_per_musim + (sisa > 0 ? 1 : 0);
    int awal_hujan = batas_kemarau + 1;

    cout << "\n=== INFO MUSIM ===\n";
    cout << "Jumlah hari: " << jumlah_hari << "\n";
    cout << "Musim KEMARAU: Hari 1 - " << batas_kemarau << "\n";
    if (awal_hujan <= jumlah_hari) {
        cout << "Musim HUJAN: Hari " << awal_hujan << " - " << jumlah_hari << "\n";
    }

    cout << "\n=== HASIL AKHIR ===\n";
    cout << "Keuntungan maksimal: " << hasil.total_keuntungan << "\n";

    tampilkan_lahan(hasil.daftar_aksi, 1, "KEMARAU");
    if (awal_hujan <= jumlah_hari) {
        tampilkan_lahan(hasil.daftar_aksi, awal_hujan, "HUJAN");
    }

    cout << "\n=== AKSI TANAM ===\n";
    vector<Aksi> daftar = hasil.daftar_aksi;
    sort(daftar.begin(), daftar.end(), [](const Aksi& a, const Aksi& b) {
        return a.hari < b.hari;
    });

    for (const auto& aksi : daftar) {
        int baris_ = aksi.petak / kolom + 1;
        int kolom_ = aksi.petak % kolom + 1;
        string musim_str = (aksi.hari <= batas_kemarau) ? "KEMARAU" : "HUJAN";
        cout << "Hari " << aksi.hari << " (" << musim_str << "): Tanam " 
             << daftar_tanaman[aksi.id_tanaman].nama
             << " di petak (" << baris_ << "," << kolom_ << ")"
             << " -> Panen hari " << aksi.hari_panen
             << " (Keuntungan: " << daftar_tanaman[aksi.id_tanaman].keuntungan() << ")\n";
    }

    cout << "\n=== RINCIAN PROFIT ===\n";
    int total = 0;
    for (const auto& aksi : daftar) {
        int k = daftar_tanaman[aksi.id_tanaman].keuntungan();
        total += k;
        cout << daftar_tanaman[aksi.id_tanaman].nama << ": " << k << "\n";
    }
    cout << "Total keuntungan dihitung: " << total << "\n";

    cout << "\n=== WAKTU EKSEKUSI ===\n";
    cout << "Program dieksekusi dalam " << durasi.count() << " ms\n";

    return 0;
}
