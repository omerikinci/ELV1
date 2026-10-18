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
3. Özel kütüphaneler artık **global** tablodan yükleniyor (18.10.2026). Proje-içi
   `sym-lib-table` / `fp-lib-table` dosyaları kaldırıldı, çünkü global tabloda aynı
   isimler tanımlıyken KiCad çakışma uyarısı veriyordu.

   Yeni bir bilgisayarda kurulum: `hardware/libraries/` içeriğini kalıcı bir yere kopyala
   (bu makinede `~/Documents/Elektronik/Library/`), sonra KiCad'de
   **Preferences → Manage Symbol Libraries / Manage Footprint Libraries → Global**
   sekmesine şu isimlerle ekle:

   | Sembol kütüphanesi | Footprint kütüphanesi |
   | --- | --- |
   | `myCONN`, `myCPU`, `myDiodes`, `myIC`, `myNMOS`, `myNTC`, `mySW` | `myNTC`, `myTO247`, `myFuse` |

   Dikkat: eski isimler `my-NMOS` ve `mydiodes` kullanımdan kalktı; sırasıyla `myNMOS`
   ve `myDiodes` oldu. `hardware/libraries/` sürüm kontrolündeki asıl kaynaktır —
   global kopyayı ondan tazele, ters yönde değil.
4. Çalışmaya başlamadan önce `git pull`, bitirince `git add` / `git commit` / `git push` yap.
   İki bilgisayarda aynı anda aynı dosyayı düzenleme; KiCad dosyaları birleştirilemez.

Not: `*-backups/`, `*.kicad_prl`, `fp-info-cache` ve kilit dosyaları `.gitignore` ile dışarıda tutulur.

Bu makinedeki konum: `~/Documents/GitHub/ELV1-revision/` — GitHub Desktop'ın yönettiği
klasörde, `ELV2` deposunun yanında. Bu repo git üzerinden çalışılır; KiCad projeleri
`hardware/` altında.

`~/Documents/Elektronik/Projeler/ElectronicLoad/` orada duran ayrı bir kopyadır ve bu
repodaki `hardware/ELV1/` ile aynı kartın eski adıdır — hangisinin kalacağına karar
verilmedi.
