#include <eccodes.h>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Użycie: " << argv[0] << " <nazwa_pliku_grib>" << endl;
        return 1;
    }

    const char* filename = argv[1];
    FILE* file = fopen(filename, "rb");
    if (!file) {
        cerr << "Nie udało się otworzyć pliku: " << filename << endl;
        return 1;
    }

    int err = 0;
    codes_handle* h = nullptr;
    long message_count = 0;

    // Iterowanie przez wszystkie wiadomości GRIB w pliku
    while ((h = codes_handle_new_from_file(nullptr, file, PRODUCT_GRIB, &err)) != nullptr) {
        ++message_count;

        // Odczytaj ogólne informacje o wiadomości
        long param = 0, level = 0, time_unit = 0, step_range = 0;
        if ((err = codes_get_long(h, "parameterNumber", &param)) != CODES_SUCCESS ||
            (err = codes_get_long(h, "level", &level)) != CODES_SUCCESS ||
            (err = codes_get_long(h, "timeUnit", &time_unit)) != CODES_SUCCESS ||
            (err = codes_get_long(h, "stepRange", &step_range)) != CODES_SUCCESS) {
            cerr << "Błąd odczytu parametrów dla wiadomości nr: " << message_count << " (error: " << err << ")" << endl;
            codes_handle_delete(h);
            continue;  // przejdź do następnej wiadomości
        }

        cout << "==========================\n";
        cout << "== General Message Info ==\n";
        cout << "==========================\n";
        cout << "Wiadomość nr: " << message_count << "\n";
        cout << "Numer parametru: " << param << "\n";
        cout << "Poziom: " << level << "\n";
        cout << "Jednostka czasu: " << time_unit << "\n";
        cout << "Zakres kroku: " << step_range << "\n";

        // Odczyt współrzędnych geograficznych
        double lat = 0.0, lon = 0.0;
        if ((err = codes_get_double(h, "latitudeOfFirstGridPointInDegrees", &lat)) != CODES_SUCCESS ||
            (err = codes_get_double(h, "longitudeOfFirstGridPointInDegrees", &lon)) != CODES_SUCCESS) {
            cerr << "Błąd odczytu współrzędnych geograficznych (error: " << err << ")" << endl;
        }
        else {
            cout << "Początkowa szerokość geograficzna: " << lat << "°\n";
            cout << "Początkowa długość geograficzna: " << lon << "°\n";
        }

        // Zwalnianie zasobów dla tej wiadomości
        codes_handle_delete(h);
    }

    // Sprawdzenie, czy zakończenie było spowodowane błędem
    if (err != CODES_SUCCESS && err != CODES_END_OF_FILE) {
        cerr << "Błąd podczas odczytywania wiadomości GRIB. Error code: " << err << endl;
    }

    fclose(file);
    return 0;
}
