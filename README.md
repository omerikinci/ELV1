# ELV1 — Elektronik Kör Yük V1

Elektronik kör yük (electronic dummy load) cihazının ilk stabil versiyonu.
150 W ile sınırlandırılmış, 30 V / 10 A aralığını destekler.

Proje detayları ve görseller: https://omerikinci.com/projects/electronic-dummy-load

## İçerik

| Klasör | Açıklama |
| --- | --- |
| `firmware/` | STM32G030F6 için STM32CubeIDE projesi (`ilkdeneme`). Final kod: `firmware/Core/Src/main.c` |
| `firmware-history/2026-04-27-stable/` | 27.04.2026 tarihli önceki stabil `main.c` |
| `hardware/ELV1/` | KiCad şema ve PCB (`ELV1`, eski adıyla `Electronic Load`), PCB görselleri, projeye özel sembol kütüphanesi |
| `hardware/ELV1-revision/` | ELV1'in revizyon kartı (tasarım aşamasında, STM32G071CBT6): KiCad şema ve PCB (`ELV1 revision`), G071 pin planı (`G071 pin plani.md`), firmware rehberi (`ELV1 revision - FIRMWARE REHBERI.txt`), görseller |
| `hardware/libraries/` | Projelerde kullanılan özel KiCad sembol (`symbols/`) ve footprint (`footprints/`) kütüphaneleri |
| `arduino-prototype/` | STM32'ye geçmeden önceki Arduino tabanlı ilk prototip (`koryukprojesi.ino`) |

## Firmware

- MCU: STM32G030F6Px
- ADS1115 (I2C) ile akım/gerilim ölçümü, dahili ADC
- DS18B20 sıcaklık sensörü
- I2C LCD, buzzer, soft-start

Derlemek için `firmware/` klasörünü STM32CubeIDE'ye mevcut proje olarak içe aktarın.

## ELV1 revision'ı başka bilgisayarda açmak (Windows / macOS)

1. Repoyu klonla:
   ```
   git clone https://github.com/omerikinci/ELV1.git
   ```
2. KiCad 10 ile `hardware/ELV1-revision/ELV1 revision.kicad_pro` dosyasını aç.
3. Özel kütüphaneler (`myCPU`, `myIC`, `myNMOS`, `myNTC`, `mySW`, `myCONN`, `myDiodes`, `myTO247`, `myFuse`)
   projenin kendi `sym-lib-table` / `fp-lib-table` dosyalarından,
   `${KIPRJMOD}/../libraries/` altından otomatik yüklenir. Global kütüphane ayarı gerekmez.
4. Çalışmaya başlamadan önce `git pull`, bitirince `git add` / `git commit` / `git push` yap.
   İki bilgisayarda aynı anda aynı dosyayı düzenleme; KiCad dosyaları birleştirilemez.

Not: `*-backups/`, `*.kicad_prl`, `fp-info-cache` ve kilit dosyaları `.gitignore` ile dışarıda tutulur.
