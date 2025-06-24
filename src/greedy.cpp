#include <bits/stdc++.h>
using namespace std;

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
vector<int> lahan_tersedia;
vector<Aksi> aksi_terjadi;

int hitung_musim(int hari) {
    int hari_per_musim = jumlah_hari / 2;
    int sisa = jumlah_hari % 2;
    int batas_kemarau = hari_per_musim + (sisa > 0 ? 1 : 0);
    return (hari <= batas_kemarau) ? 0 : 1;
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

    // Urutkan berdasarkan rasio keuntungan / waktu_panen
    sort(daftar_tanaman.begin(), daftar_tanaman.end(), [](const Tanaman& a, const Tanaman& b) {
        return (double)a.keuntungan() / a.waktu_panen > (double)b.keuntungan() / b.waktu_panen;
    });

    int modal = modal_awal;
    int total_petak = baris * kolom;
    lahan_tersedia.assign(total_petak, 0);

    for (int hari = 1; hari <= jumlah_hari; hari++) {
        int musim = hitung_musim(hari);
        for (int petak = 0; petak < total_petak; petak++) {
            if (lahan_tersedia[petak] < hari) {
                for (int i = 0; i < jumlah_tanaman; i++) {
                    Tanaman& t = daftar_tanaman[i];
                    if (t.musim == musim && modal >= t.harga_beli && hari + t.waktu_panen <= jumlah_hari) {
                        lahan_tersedia[petak] = hari + t.waktu_panen;
                        modal -= t.harga_beli;
                        aksi_terjadi.push_back({hari, petak, i, hari + t.waktu_panen});
                        break;
                    }
                }
            }
        }
    }

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

    cout << "\n=== AKSI TANAM ===\n";
    for (const auto& aksi : aksi_terjadi) {
        int baris_ = aksi.petak / kolom + 1;
        int kolom_ = aksi.petak % kolom + 1;
        string musim_str = (aksi.hari <= batas_kemarau) ? "KEMARAU" : "HUJAN";
        cout << "Hari " << aksi.hari << " (" << musim_str << "): Tanam "
             << daftar_tanaman[aksi.id_tanaman].nama
             << " di petak (" << baris_ << "," << kolom_ << ")"
             << " -> Panen hari " << aksi.hari_panen
             << " (Keuntungan: " << daftar_tanaman[aksi.id_tanaman].keuntungan() << ")\n";
    }

    cout << "\n=== HASIL AKHIR ===\n";
    tampilkan_lahan(aksi_terjadi, 1, "KEMARAU");
    if (awal_hujan <= jumlah_hari) {
        tampilkan_lahan(aksi_terjadi, awal_hujan, "HUJAN");
    }

    cout << "\n=== TOTAL PROFIT ===\n";
    int profit = 0;
    for (auto& aksi : aksi_terjadi) {
        profit += daftar_tanaman[aksi.id_tanaman].keuntungan();
    }
    cout << "Profit total: " << profit << endl;

    return 0;
}
