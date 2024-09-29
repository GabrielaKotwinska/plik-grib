#include <eccodes.h>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Użycie: " << argv[0] << " <nazwa_pliku_grib>" << std::endl;
        return 1;
    }

    const char* filename = argv[1];
    FILE* file = fopen(filename, "rb");
    if (!file) {
        cerr << "Nie udało się otworzyć pliku: " << filename << std::endl;
        return 1;
    }

    int err = 0;
    codes_handle* h = nullptr;
    long message_count = 0;

    while ((h = codes_handle_new_from_file(nullptr, file, PRODUCT_GRIB, &err)) != nullptr) {
        ++message_count;

        // Odczytaj ogólne informacje o wiadomości
        long param, level, time_unit, step_range;
        codes_get_long(h, "parameterNumber", &param);
        codes_get_long(h, "level", &level);
        codes_get_long(h, "timeUnit", &time_unit);
        codes_get_long(h, "stepRange", &step_range);

        cout << "==========================\n";
        cout << "== General Message Info ==\n";
        cout << "==========================\n";
        cout << "Wiadomość nr: " << message_count << "\n";
        cout << "Numer parametru: " << param << "\n";
        cout << "Poziom: " << level << "\n";
        cout << "Jednostka czasu: " << time_unit << "\n";
        cout << "Zakres kroku: " << step_range << "\n";

        // Odczyt współrzędnych geograficznych 
        double lat, lon;
        codes_get_double(h, "latitudeOfFirstGridPointInDegrees", &lat);
        codes_get_double(h, "longitudeOfFirstGridPointInDegrees", &lon);
        cout << "Początkowa szerokość geograficzna: " << lat << "°\n";
        cout << "Początkowa długość geograficzna: " << lon << "°\n";

        // Zwalnianie zasobów
        codes_handle_delete(h);
    }

    if (err != CODES_SUCCESS) {
        std::cerr << "Błąd podczas odczytywania wiadomości GRIB." << std::endl;
    }

    fclose(file);
    return 0;
}
