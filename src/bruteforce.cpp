#include <bits/stdc++.h>
#include <chrono>
using namespace std;
using namespace std::chrono;
int baris, kolom, jumlah_tanaman, jumlah_hari, modal_awal, hari_per_musim, sisa, batas_kemarau, awal_hujan;

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

vector<Tanaman> daftar_tanaman;
int total_petak;
int profit_maks = 0;
vector<Aksi> hasil_terbaik;

void tampilkan_lahan(const vector<Aksi>& aksi, int hari_target, const string& musim) {
    cout << "\n=== Lahan Hari ke-" << hari_target << " (" << musim << ") ===\n";
    vector<vector<string>> grid(baris, vector<string>(kolom, "  .  "));

    for (const auto& a : aksi) {
        if (a.hari <= hari_target && a.hari_panen > hari_target) {
            int r = a.petak / kolom;
            int c = a.petak % kolom;
            string n = daftar_tanaman[a.id_tanaman].nama;
            if (n.length() > 4) n = n.substr(0, 4);
            while (n.length() < 4) n += " ";
            grid[r][c] = " " + n + " ";
        }
    }

    cout << "  ";
    for (int j = 0; j < kolom; j++) cout << "+-----";
    cout << "+\n";

    for (int i = 0; i < baris; i++) {
        cout << "  ";
        for (int j = 0; j < kolom; j++) {
            cout << "|" << grid[i][j];
        }
        cout << "|\n";
        cout << "  ";
        for (int j = 0; j < kolom; j++) cout << "+-----";
        cout << "+\n";
    }

    cout << "Keterangan: '.' = kosong, nama = tanaman aktif\n";
}

void brute_force(int hari, int modal, vector<int> ketersediaan, vector<Aksi> aksi, int profit_saat_ini) {
    if (hari > jumlah_hari) {
        if (profit_saat_ini > profit_maks) {
            profit_maks = profit_saat_ini;
            hasil_terbaik = aksi;
        }
        return;
    }

    int tambahan_profit = 0;
    for (const auto& a : aksi) {
        if (a.hari_panen == hari) {
            tambahan_profit += daftar_tanaman[a.id_tanaman].keuntungan();
        }
    }

    function<void(int, vector<int>, vector<Aksi>, int)> coba_petak =
        [&](int indeks, vector<int> lahan, vector<Aksi> aksi_hari_ini, int sisa_modal) {
            if (indeks == total_petak) {
                brute_force(hari + 1, sisa_modal, lahan, aksi_hari_ini, profit_saat_ini + tambahan_profit);
                return;
            }

            coba_petak(indeks + 1, lahan, aksi_hari_ini, sisa_modal);

            if (lahan[indeks] < hari) {
                int batas = jumlah_hari / 2 + (jumlah_hari % 2 > 0 ? 1 : 0);
                int musim = (hari <= batas) ? 0 : 1;
                for (int i = 0; i < jumlah_tanaman; i++) {
                    const auto& t = daftar_tanaman[i];
                    if (t.musim == musim && sisa_modal >= t.harga_beli && hari + t.waktu_panen <= jumlah_hari) {
                        vector<int> lahan_baru = lahan;
                        vector<Aksi> aksi_baru = aksi_hari_ini;
                        lahan_baru[indeks] = hari + t.waktu_panen;
                        aksi_baru.push_back({hari, indeks, i, hari + t.waktu_panen});
                        coba_petak(indeks + 1, lahan_baru, aksi_baru, sisa_modal - t.harga_beli);
                    }
                }
            }
        };

    coba_petak(0, ketersediaan, aksi, modal);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> baris >> kolom >> jumlah_tanaman >> jumlah_hari >> modal_awal;
    total_petak = baris * kolom;

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

    vector<int> lahan_tersedia(total_petak, 0);
    auto mulai = high_resolution_clock::now();
    brute_force(1, modal_awal, lahan_tersedia, {}, 0);
    auto selesai = high_resolution_clock::now();
    auto durasi = duration_cast<milliseconds>(selesai - mulai);

    hari_per_musim = jumlah_hari / 2;
    sisa = jumlah_hari % 2;
    batas_kemarau = hari_per_musim + (sisa > 0 ? 1 : 0);
    awal_hujan = batas_kemarau + 1;

    cout << "\n=== INFO MUSIM ===\n";
    cout << "Jumlah hari: " << jumlah_hari << "\n";
    cout << "Musim KEMARAU: Hari 1 - " << batas_kemarau << "\n";
    if (awal_hujan <= jumlah_hari) {
        cout << "Musim HUJAN: Hari " << awal_hujan << " - " << jumlah_hari << "\n";
    }

    cout << "\n=== HASIL AKHIR ===\n";
    cout << "Keuntungan maksimal: " << profit_maks << "\n";

    for(int i=1; i<=jumlah_hari; i++){
        if(i < awal_hujan) tampilkan_lahan(hasil_terbaik, i, "KEMARAU");
        else  tampilkan_lahan(hasil_terbaik, i, "HUJAN");
    }

    cout << "\n=== AKSI TANAM ===\n";
    sort(hasil_terbaik.begin(), hasil_terbaik.end(), [](const Aksi& a, const Aksi& b) {
        return a.hari < b.hari;
    });

    for (const auto& a : hasil_terbaik) {
        int r = a.petak / kolom + 1;
        int c = a.petak % kolom + 1;
        string musim_str = (a.hari <= batas_kemarau) ? "KEMARAU" : "HUJAN";
        cout << "Hari " << a.hari << " (" << musim_str << "): Tanam "
             << daftar_tanaman[a.id_tanaman].nama << " di petak ("
             << r << "," << c << ") -> Panen hari " << a.hari_panen
             << " (Keuntungan: " << daftar_tanaman[a.id_tanaman].keuntungan() << ")\n";
    }

    cout << "\n=== RINCIAN PROFIT ===\n";
    int total = 0;
    for (const auto& a : hasil_terbaik) {
        int k = daftar_tanaman[a.id_tanaman].keuntungan();
        total += k;
        cout << daftar_tanaman[a.id_tanaman].nama << ": "
             << k << "\n";
    }
    cout << "Total keuntungan dihitung: " << total << "\n";

    cout << "\n=== WAKTU EKSEKUSI ===\n";
    cout << "Program dieksekusi dalam " << durasi.count() << " ms\n";

    return 0;
}