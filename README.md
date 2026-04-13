# BLE USB Keyboard — Klawiatura USB → Bezprzewodowa BLE (ESP32-S3)

> Projekt dokumentuje proces przeróbki klawiatury przewodowej (USB) na bezprzewodową przy użyciu mikrokontrolera **ESP32-S3** i protokołu **Bluetooth Low Energy (BLE)**.

---

## 📋 Spis treści

- [Opis projektu](#opis-projektu)
- [Komponenty](#komponenty)
- [Schemat połączeń](#schemat-połączeń)
- [Oprogramowanie i środowisko](#oprogramowanie-i-środowisko)
- [Instalacja i konfiguracja](#instalacja-i-konfiguracja)
- [Przebieg realizacji](#przebieg-realizacji)
- [Zdjęcia i wyniki](#zdjęcia-i-wyniki)
- [Znane problemy](#znane-problemy)
- [Plany rozwoju](#plany-rozwoju)
- [Literatura i zasoby](#literatura-i-zasoby)
- [Licencja](#licencja)

---

## Opis projektu

<!-- TODO: Opisz cel projektu. Jaką klawiaturę przerobiłeś? Dlaczego wybrałeś ESP32-S3 i BLE? -->

Ten projekt pokazuje, jak przerobić standardową klawiaturę USB na bezprzewodową BLE przy minimalnej ingerencji w oryginalny sprzęt. Mikrokontroler **ESP32-S3** pełni rolę pośrednika: odbiera sygnały z matrycy klawiszy lub bezpośrednio z interfejsu USB klawiatury i wysyła je jako zdarzenia HID przez Bluetooth Low Energy.

**Klawiatura:** <!-- TODO: np. „Logitech K120", „mechaniczna klawiatura XYZ" -->  
**Cel:** bezprzewodowa praca z urządzeniami obsługującymi BLE HID (laptop, tablet, smartfon)

---

## Komponenty

### Elektronika

| Komponent | Ilość | Uwagi |
|-----------|-------|-------|
| ESP32-S3 (moduł/devkit) | 1 | <!-- TODO: podaj konkretny model, np. ESP32-S3-DevKitC-1 --> |
| Klawiatura USB do przeróbki | 1 | <!-- TODO: model klawiatury --> |
| Akumulator Li-Ion / LiPo | 1 | <!-- TODO: pojemność, np. 1000 mAh --> |
| Moduł ładowania TP4056 | 1 | opcjonalnie |
| Konwerter USB-A (wtyczka) | 1 | do podłączenia istniejącego kabla klawiatury |
| Kondensatory, rezystory | wg schematu | <!-- TODO: uzupełnij --> |
| Obudowa / mocowanie | — | <!-- TODO: druk 3D, modyfikacja oryginału itp. --> |

### Narzędzia

- Lutownica + cyna
- Multimetr
- <!-- TODO: inne narzędzia -->

---

## Schemat połączeń

<!-- TODO: Wstaw schemat elektryczny (obrazek lub link do pliku w repozytorium). -->
<!-- Przykład: ![Schemat](docs/schematic.png) -->

```
[Klawiatura USB]
      |
   USB D+ / D-
      |
  [ESP32-S3]  ←→  BLE HID  ←→  [Komputer / Tablet]
      |
  [Akumulator]
```

> Szczegółowy schemat będzie dostępny w: `docs/schematic.pdf` *(do dodania)*

---

## Oprogramowanie i środowisko

### Wymagania

- [Arduino IDE](https://www.arduino.cc/en/software) ≥ 2.x **lub** [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/) ≥ 5.x
- Wsparcie dla płytki ESP32-S3:
  - Arduino: dodaj URL w menedżerze płytek: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
  - ESP-IDF: oficjalne SDK Espressif
- Biblioteki:
  - <!-- TODO: np. `ESP32-BLE-Keyboard` (https://github.com/T-vK/ESP32-BLE-Keyboard) -->
  - <!-- TODO: np. `USB Host Shield Library 2.0` jeśli używasz USB Host -->

### Konfiguracja środowiska

```bash
# Klonowanie repozytorium
git clone https://github.com/areisigma/ble-usb-keyboard.git
cd ble-usb-keyboard

# TODO: opisz kolejne kroki instalacji zależności
```

---

## Instalacja i konfiguracja

1. **Zainstaluj środowisko** zgodnie z sekcją [Oprogramowanie i środowisko](#oprogramowanie-i-środowisko).
2. **Otwórz projekt** w Arduino IDE / ESP-IDF.
3. **Dostosuj konfigurację** w pliku `config.h` *(do dodania)*:
   ```c
   // TODO: przykładowe parametry konfiguracyjne
   #define DEVICE_NAME   "BLE Keyboard"
   #define BATTERY_PIN   GPIO_NUM_1
   ```
4. **Wgraj firmware** na ESP32-S3:
   - Podłącz płytkę przez USB.
   - Wybierz właściwą płytkę i port w IDE.
   - Kliknij *Upload* / uruchom `idf.py flash`.
5. **Sparuj klawiaturę** z urządzeniem docelowym przez Bluetooth.

---

## Przebieg realizacji

### Etap 1 — Analiza klawiatury

<!-- TODO: Opisz, jak zbadałeś oryginalną klawiaturę. Czy używałeś USB HID, matrycy klawiszy, czy interfejsu PS/2? -->

### Etap 2 — Projekt elektroniki

<!-- TODO: Opisz decyzje projektowe: wybór ESP32-S3, zasilanie, integracja z klawiaturą. -->

### Etap 3 — Prototyp

<!-- TODO: Opisz budowę prototypu na płytce stykowej. Wstaw zdjęcia. -->

### Etap 4 — Firmware

<!-- TODO: Opisz, jak działa oprogramowanie. Jakie biblioteki BLE HID zastosowałeś? -->

### Etap 5 — Integracja i testy

<!-- TODO: Opisz montaż w obudowie, testy działania, czas pracy na baterii. -->

---

## Zdjęcia i wyniki

<!-- TODO: Wstaw zdjęcia gotowego projektu. -->
<!-- Przykład:
![Wnętrze klawiatury](docs/images/inside.jpg)
![Gotowy projekt](docs/images/finished.jpg)
-->

| Widok | Opis |
|-------|------|
| *(zdjęcie)* | Wnętrze klawiatury z ESP32-S3 |
| *(zdjęcie)* | Gotowy, zmontowany projekt |

**Czas pracy na baterii:** <!-- TODO: np. „~2 tygodnie przy codziennym użytkowaniu" -->  
**Opóźnienie (latency):** <!-- TODO: np. „< 10 ms" -->

---

## Znane problemy

<!-- TODO: Opisz napotkane problemy i ich rozwiązania lub obecne ograniczenia. -->

- [ ] <!-- TODO: np. „Problemy z parzystością przy USB Full Speed" -->
- [ ] <!-- TODO: np. „Brak wsparcia dla klawiszy multimedialnych" -->

---

## Plany rozwoju

<!-- TODO: Co chciałbyś jeszcze dodać lub ulepszyć? -->

- [ ] Obsługa kilku urządzeń BLE jednocześnie
- [ ] Wskaźnik poziomu baterii (LED / e-ink)
- [ ] Tryb USB-C passthrough
- [ ] <!-- TODO: inne pomysły -->

---

## Literatura i zasoby

- [ESP32-S3 — dokumentacja techniczna (Espressif)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/)
- [Bluetooth HID over GATT Profile Specification](https://www.bluetooth.com/specifications/specs/hid-over-gatt-profile-1-0/)
- <!-- TODO: inne przydatne linki, artykuły, projekty, które Cię zainspirowały -->

---

## Licencja

<!-- TODO: Wybierz licencję, np. MIT, GPL-3.0 lub inna. -->

Projekt udostępniony na licencji [MIT](LICENSE) *(plik LICENSE do dodania)*.
