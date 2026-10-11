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
| `hardware/ELV1-revision/` | ELV1'in revizyon kartı: KiCad şema ve PCB (`ELV1-revision`), PCB görselleri |
| `hardware/libraries/` | Projelerde kullanılan özel KiCad sembol (`symbols/`) ve footprint (`footprints/`) kütüphaneleri |
| `arduino-prototype/` | STM32'ye geçmeden önceki Arduino tabanlı ilk prototip (`koryukprojesi.ino`) |

## Firmware

- MCU: STM32G030F6Px
- ADS1115 (I2C) ile akım/gerilim ölçümü, dahili ADC
- DS18B20 sıcaklık sensörü
- I2C LCD, buzzer, soft-start

Derlemek için `firmware/` klasörünü STM32CubeIDE'ye mevcut proje olarak içe aktarın.
