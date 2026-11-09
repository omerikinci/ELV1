# ELV1 — Elektronik Kör Yük V1

Elektronik kör yük (electronic dummy load) cihazının ilk stabil versiyonu.
150 W ile sınırlandırılmış, 30 V / 10 A aralığını destekler.

Proje detayları ve görseller: https://omerikinci.com/projects/electronic-dummy-load

## İçerik

| Klasör | Açıklama |
| --- | --- |
| `firmware/` | STM32G030F6 için STM32CubeIDE projesi (`ilkdeneme`). Final kod: `firmware/Core/Src/main.c` |
| `firmware-history/2026-04-27-stable/` | 27.04.2026 tarihli önceki stabil `main.c` |
| `hardware/ELV1/` | KiCad şema ve PCB (`ELV1`, eski adıyla `Electronic Load`), PCB görselleri, projeye özel sembol kütüphanesi. Şema orijinal tasarımı gösterir; karttaki farklar aşağıda. |
| `hardware/ELV1-revision/` | ELV1'in revizyon kartı (tasarım aşamasında, STM32G071CBT6): KiCad şema ve PCB (`ELV1 revision`), G071 pin planı (`G071 pin plani.md`), firmware rehberi (`ELV1 revision - FIRMWARE REHBERI.txt`), görseller |
| `hardware/libraries/` | Projelerde kullanılan özel KiCad sembol (`symbols/`) ve footprint (`footprints/`) kütüphaneleri |
| `arduino-prototype/` | STM32'ye geçmeden önceki Arduino tabanlı ilk prototip (`koryukprojesi.ino`) |

## Firmware

- MCU: STM32G030F6Px
- ADS1115 (I2C) ile akım/gerilim ölçümü, dahili ADC
- DS18B20 sıcaklık sensörü
- I2C LCD, buzzer, soft-start

Derlemek için `firmware/` klasörünü STM32CubeIDE'ye mevcut proje olarak içe aktarın.

## ELV1'in final hali (şemadan farklar)

ELV1 ilk cihazdır ve bu haliyle kalır. Sorunlar anlaşıldıkça yalnızca düzeltme yapılır.
Yeni özellikler (USB/PC bağlantısı, pot yerine encoder vb.) ELV1 revision'a aittir.

`hardware/ELV1/ELV1.kicad_sch` **bilerek güncellenmedi**; orijinal tasarımı gösterir
(4 MOSFET, 4 x 0.1 Ω şönt, LM358). Karttaki gerçek durum:

| Parça | Şemada | Kartta | Tarih |
| --- | --- | --- | --- |
| Yük MOSFET'leri | U1, U6, U12, U13 (4 x IRFP260N) | Yalnız U1 ve U6. U12, U13 söküldü. | 10.2026 |
| Şöntler | R2, R23, R30, R45: 0.1 Ω 10 W (markasız) | R2 ve R23 yerinde: 2 x Isabellenhütte VMS-R100-1.0-U paralel = 0.05 Ω. R30, R45 söküldü. | 20.10.2026 |
| Op-amp'ler (U2, U3) | LM358 | OPA2197 | — |
| R1 (akım potu besleme direnci) | 440K | 440K olmalı (aşağıdaki nota bak) | 20.10.2026 |
| Şebeke X kondansatörü boşaltması | yok | 1 MΩ 1 W THT, AC girişte L-N arası | 20.10.2026 |

**Neden:**
- **Şöntler:**
  - Markasız 10 W dirençler ısındıkça akım kayıyordu. VMS'nin sıcaklık katsayısı < 20 ppm/K.
  - 2 MOSFET'te tek 0.1 Ω VMS, 5 A'de 2.5 W harcar; bu, 3 W sınırının %83'üdür ve direnç aşırı ısınıyordu.
  - Paralel çiftte direnç başına 0.625 W düşer (sınırın %21'i).
- **R1:**
  - Akımı pot ayarlar. Op-amp her şöntün gerilimini pot gerilimine eşitler, bu yüzden toplam akım = kanal sayısı x pot gerilimi / şönt direnci.
  - 2 x 0.1 Ω döneminde pot tam turda ~5 A veriyordu. Bu yüzden R1 geçici olarak 220K yapıldı.
  - 2 x 0.05 Ω ile 440K tekrar tam turda ~10 A verir; bu, orijinal 4 x 0.1 Ω ölçeğidir.
  - 220K ile tam tur ~20 A ister ve ISET % göstergesi yarı turda %100'e ulaşır.
- **Boşaltma direnci:**
  - Cihaz kapalıyken fiş çekilince EMI filtresinin X kondansatöründe kalan şarj çarpıyordu.
  - Önce geçici olarak 12 x 10 MΩ paralel (~790k) takıldı. Ancak paralel bağlı her direnç tam şebeke tepe gerilimini (~325 V) görür; 1/8 ve 1/4 W dirençler bu gerilim için uygun değil.
  - 20.10.2026'da bunların yerine tek 1 MΩ, 1 W, 500 V direnç (Royalohm MF02WJJ0105A19) takıldı.

**Durum (20.10.2026):** ELV1 tamamlandı. Bir sorun çıkmadıkça açılmayacak; geliştirme ELV1 revision'da sürüyor.

**Firmware'de buna göre ayarlananlar** (`firmware/Core/Src/main.c`):
- Akım, iki kanalın tek uçlu okumalarının toplamıdır: ADS1115 AIN0 (U1) + AIN1 (U6), `CURRENT_ADS_CH_MASK 0x03`.
  - ±0.512 V aralık, 250 SPS, kanal başına 8 örnek ortalaması.
  - Kelvin bağlantısı yok. Her kanal şöntle birlikte ortak GND yolunun düşümünü de görür. Bu düşüm `CURRENT_UV_PER_A 59172` (0.05 Ω + 2 x ~4.6 mΩ) katsayısının içindedir.
  - Kalibrasyon: seri multimetreyle ~5 A'de karşılaştır, yeni = 59172 x (ekrandaki / multimetredeki).
- Diğer ayarlar:
  - SET V düşük koruması 10 mA'den itibaren devrede.
  - Ekrandaki gerilimler kesilmez, yuvarlanır.
  - Güç koruması 160 W (MOSFET başına ~80 W).

**Ölçülen özellikler:**
- Ters polarite algılaması 1.71 V'ta devreye giriyor.

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
