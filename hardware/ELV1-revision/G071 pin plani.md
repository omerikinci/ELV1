# ELV1 revision — STM32G071CBT6 pin atama planı

- Eski işlemci: U14 STM32G030F6P6 (TSSOP20)
- Yeni işlemci: STM32G071CBT6 (LQFP48)
- Sembol: `myCPU:STM32G071CBT6` (ELV2'deki ile aynı)
- Footprint: `Package_QFP:LQFP-48_7x7mm_P0.5mm`
- Doğrulama: Alternatif fonksiyonlar CubeMX 6.13 veritabanındaki `STM32G071C(6-8-B)Tx.xml` dosyasına göre kontrol edildi.

**Temel kural:** ELV2'de de bulunan her sinyal, ELV2'deki pinle aynı pine atandı. Böylece iki kart ileride aynı pin tanımlarını (`main.h` / BSP) paylaşabilir.

## 1. Mevcut sinyaller

| LQFP48 pin | Port | Periferal / mod | ELV1 revision sinyali | G030'daki pin | ELV2'de aynı pin? |
|---|---|---|---|---|---|
| 13 | PA2 | GPIO giriş (EXTI2) | /RCP (LTV-817S → D8, R47) | 7 (PA0) | ✅ RCP |
| 14 | PA3 | ADC1_IN3 | /LOAD VOLTAGE (R54/R55 bölücü) | 11 (PA4) | ✅ LOAD VOLTAGE |
| 17 | PA6 | GPIO çıkış veya TIM3_CH1 / TIM16_CH1 PWM | /BUZZER1 (Q4 kapısı) | 8 (PA1) | — (ELV2'de SW2) |
| 18 | PA7 | GPIO çıkış veya TIM3_CH2 / TIM14_CH1 PWM | /BUZZER2 (R72) | — (yeni) | ✅ BUZZER |
| 22 | PB10 | GPIO çıkış (aç/kapa) | /FAN CONTROL 2 → R53 → Q2 → J8 MAIN FAN | 9 (PA2) | ✅ FAN CONTROL |
| 48 | PB9 | GPIO çıkış (aç/kapa) | /FAN CONTROL 1 → R81 → Q5 → J9 VENTILATION FAN | — (yeni) | — (ELV2'de EXP12) |
| 43 | PB4 | TIM3_CH1 input capture (EXTI4) | /TACT1 ← J9.3 (R76 pull-up, R79 10k, C70 10 nF) | — (yeni) | — (ELV2'de TACT1 = PB1) |
| 44 | PB5 | TIM3_CH2 input capture (EXTI5) | /TACT2 ← J8.3 (R75 pull-up, R74 10k, C71 10 nF) | — (yeni) | — (ELV2'de TACT2 = PB2) |
| 19 | PB0 | GPIO çıkış | STATUS LED (R80 330 Ω, D13) | — (yeni) | ✅ STATUS LED |
| 23 | PB11 | GPIO open-drain | /DS18B20 DQ | 15 (PA8/PB0–2) | ✅ DS18B20 DQ |
| 25 | PB13 | GPIO giriş (EXTI13) | /ENCODER A | 14 (PA7) | ✅ ENCODER A |
| 26 | PB14 | GPIO giriş (EXTI14) | /ENCODER B | 13 (PA6) | ✅ ENCODER B |
| 27 | PB15 | GPIO giriş (EXTI15) | /ENCODER SW | 12 (PA5) | ✅ ENCODER SW |
| 30 | PC6 | GPIO çıkış | /DAC CONTROL (Q1 kapısı, MCP4725 çıkışını sıfırlar) | 2 (PB9/PC14) | — (ELV2'de EXP4) |
| 31 | PC7 | GPIO giriş (EXTI7) | /RESET BUTTON (J4.13) | 10 (PA3) | — (ELV2'de EXP5) |
| 33 | PA11 | I2C2_SCL | /I2C SCL (MCP4725, ADS1115, J4.5) | 16 | ✅ SCL |
| 34 | PA12 | I2C2_SDA | /I2C SDA (MCP4725, ADS1115, J4.7) | 17 | ✅ SDA |
| 35 | PA13 | SYS_SWDIO | J7.2 | 18 | ✅ |
| 36 | PA14 | SYS_SWCLK | J7.3 | 19 | ✅ |
| 37 | PA15 | GPIO çıkış | /USB CHARGE (SY6280 EN) | 3 (PC15) | ✅ USB CONTROL |
| 45 | PB6 | USART1_TX | /TX → CH340G RXD | 20 (PB3–6) | ✅ TX |
| 46 | PB7 | USART1_RX | /RX ← CH340G TXD | 1 (PB7/PB8) | ✅ RX |
| 10 | PF2-NRST | NRST | J7.4, SW1, R53, C43 | 6 | ✅ |
| 24 | PB12 | GPIO çıkış (push-pull) | **/OUT CONTROL** (R40 → Q3 → U10 gate). **LOW = U10 açık**, HIGH veya Hi-Z = kapalı (R42 pull-up) | **bağlı değil** | ✅ aynı rol (ELV2'de LOAD CONTROL), **polarite ters** |

EXTI hatları birbirinden farklı: 2, 4, 5, 7, 13, 14, 15. Çakışma yok.

**TACT notu (16.10.2026):** TACT1/TACT2 önce PC13/PC14'teydi. Ancak EXTI13/14 encoder A/B (PB13/PB14) ile çakışıyordu ve bu pinlerde input capture yok. Bu yüzden PB4/PB5'e (TIM3_CH1/CH2) taşındı. C70/C71 etiket üzerinden bağlı; PCB'de PB4/PB5'e yakın yerleştir. ELV2'deki PB2 tach için uygun değil: timer'ı yok ve EXTI2, RCP (PA2) ile çakışıyor.

**/OUT CONTROL notu (18.10.2026'da güncellendi):** Sinyal artık işlemciye bağlı. Netlist'te `/OUT CONTROL` = R40.1 + R42.2 + **U20.24 (PB12)**. Önceki "yalnızca hierarchical label, işlemciye gitmiyor, U10 hep kapalı" notu geçersiz (bkz. Bölüm 7, Bulgu 2). PB12, ELV2'de de giriş yolunu açıp kapatan pin. Ancak aktif seviyeler ters:
- ELV1 revision: LOW = açık.
- ELV2: HIGH = açık.

Bu yüzden firmware'de `LOAD_PATH_ON()` / `LOAD_PATH_OFF()` makroları kart başına ayrı tanımlanmalı. İki kartta da reset anında yol kapalı kalıyor: ELV1 revision'da R42 pull-up, ELV2'de R101 pull-down.

### 1a. Setpoint DAC kararı (14.10.2026)

**MCP4725 ile devam ediliyor.** İki kanal ortak setpoint'te kalıyor. PA4/PA5 boş kalıyor. Gerekçe ve karşılaştırma için bkz. Bölüm 6.

## 2. Besleme pinleri

| Pin | Bağlantı |
|---|---|
| 4 VBAT | +3.3V (ELV2'deki gibi) |
| 5 VREF+ | +3.3V → ferrit + 100 nF + 1 µF. ELV2'deki FB1/C43/C44 devresini kopyala. **LOAD VOLTAGE dahili ADC ile ölçüldüğü için önemli.** |
| 6 VDD | Mevcut FB2 + C37 (10 µF) + C38 (100 nF) |
| 7 VSS | GND |

## 3. Bağlanmadan bırakılacak pinler

| Pin | Neden |
|---|---|
| 29 (PA9), 32 (PA10) | UCPD1 dead-battery pinleri (DBCC1/DBCC2). Sembolde zaten NC olarak işaretli. ELV2'de de boş. Bunları CC pinlerine (PA8/PB15) **bağlama**. |

## 4. Boşta kalan pinler

Yedek pinler (18.10.2026 itibarıyla 15 adet, ERC ile doğrulandı; PF0/PF1 HSE kristale bağlı):

- PC13, PC14, PC15
- PA1 (ADC, TIM2_CH2) — **PA0 artık yedek değil: `/20V SENSE`'e bağlı (18.10.2026)**
- PA4 (DAC1_OUT1), PA5 (DAC1_OUT2)
- PA8 (UCPD1_CC1, dead-battery notu)
- PB1, PB2, PB3
- PB8
- PD0–PD3

Geliştirme kartı kullanımı için bu pinler, 3.3V ve GND ile birlikte bir genişleme başlığına çıkarılabilir (ELV2'deki EXP başlığı gibi).

Önerilen opsiyonel eklemeler (ELV2 ile aynı pinlerde):

- **PB0 → STATUS LED** (330 Ω): ELV2'de D11. Hata ayıklarken çok işe yarar.
- **PA4 → DAC1_OUT1**: ELV2 setpoint için MCP4725 yerine dahili DAC kullanıyor ("DAB BUS"). İleride MCP4725'i kaldırmak istersen PA4'ten DAC hattına 0 Ω / test noktası koyabilirsin.

## 5. Dikkat edilecekler

1. **DAC CONTROL'ü PB12'ye koyma.** ELV2'de PB12 = LOAD CONTROL. Orada R101 ile GND'ye çekili ve HIGH = yük açık anlamına geliyor. ELV1 revision'daki DAC CONTROL'de ise R5 ile 3.3V'a çekme var ve HIGH = Q1 iletimde = setpoint sıfır = yük kapalı. Polarite ters olduğu için aynı pin, paylaşılan firmware'de tehlikeli bir hataya yol açabilir. Bu yüzden ayrı pine (PC6) koydum.
2. **Reset sırasında güvenli durum:** G0 pinleri reset anında analog modda (yüksek empedans) gelir. R5 pull-up'ı Q1'i açık tutar ve firmware başlayana kadar yük kapalı kalır. R5'i koru.
3. **PB15 (ENCODER SW) UCPD1_CC2 pinidir.** CubeMX'in `HAL_MspInit()` içinde ürettiği `HAL_SYSCFG_StrobeDBattpinsConfig(...)` çağrısını silme. Bu çağrı dead-battery pull-down'unu kaldırıp pini GPIO olarak serbest bırakır.
4. **PA11/PA12 eşlemesi:** LQFP48'de 33/34 numaralı pinler PA11 [PA9] ve PA12 [PA10] olarak geçer. Varsayılan hâliyle PA11/PA12 olarak kullanılır, I2C2 için remap gerekmez.
5. **Encoder (opsiyonel iyileştirme):** PB13/PB14 donanım encoder moduna bağlanamaz (yalnızca TIM1_CH1N/CH2N var). Bu pinlerde encoder EXTI veya yazılım ile okunur. Adım kaçırmayan donanım sayacı istersen encoder'ı **PA0/PA1 (TIM2_CH1/CH2, 32 bit)** üzerine al. Ancak o zaman ELV2'de de aynı değişikliği yapmak gerekir; yoksa iki kart arasındaki uyum bozulur.
6. **Firmware:** `.ioc` dosyasında işlemciyi STM32G071CBTx yap, linker script'i `STM32G071CBTX_FLASH.ld` olarak değiştir. Pin tanımlarının hepsi değiştiği için `main.h` içindeki etiketler yeniden üretilmeli.

## 6. DAC kararı — MCP4725 mi, dahili DAC mı?

### Mevcut setpoint zinciri (netlist'ten)

```
MCP4725 (Vref = 3.3V LDO) → R2 22k / R4 2.2k (×0.0909) → C11 100n + C13 10µ ─┬─ R6 1k → U4A+ → U7 (IRFP260N), shunt R18||R116 0.05Ω
                                      Q1 (DAC CONTROL kelepçesi) ──────────────┘─ R7 1k → U4B+ → U8 (IRFP260N), shunt R19||R117 0.05Ω
```

- Tam skala: 3.3 V × 0.180 = 0.595 V setpoint. Bu, kanal başına 5.95 A, toplamda yaklaşık 11.9 A eder.
- 1 LSB: 0.806 mV × 0.180 = 0.145 mV, yani **kanal başına 1.45 mA**. Ortak DAC'ta toplam akım 2.9 mA'lık adımlarla değişir.
- Her kanalın kendi op-amp döngüsü ve kendi şöntü var. Akım paylaşımını zaten donanım sağlıyor.
- ADS1115 iki şöntü ayrı ayrı ölçüyor (AIN0–1 ve AIN2–3, Kelvin bağlantılı NetTie ile).

### Bağımsız kontrolün gerçek kazancı

1. **Kanallar arası uyumsuzluğu kalibre etme.** U4A/B'nin geri beslemesinde R8 (220k) gate'ten eviren girişe bağlı. Bu yüzden şönt gerilimi yaklaşık **Vset − Vgate/220** oluyor. Gate 3–4.5 V iken bu, kanal başına 14–20 mV'luk (~0.14–0.2 A) bir hata demek. Hata MOSFET'in Vth değerine ve sıcaklığına bağlı. İki IRFP260N arasındaki Vth farkı (datasheet'te 2–4 V) kanallar arasında ±45 mA'e varan dengesizlik yaratabilir. Ortak DAC'la bu farkı düzeltemezsin; bağımsız DAC'la her kanal ADS1115 okumasıyla kendi başına trimlenir.
2. **Toplam akımda daha ince adım.** Kanallar sırayla 1 LSB artırılırsa toplam adım 2.9 mA'dan 1.45 mA'e iner. Bu, yaklaşık 13 bit efektif çözünürlük demek.
3. **Düşük akımda tek kanal.** Düşük akımlarda tek MOSFET çalıştırılırsa sabit hatalar ikiye bölünmez, oransal hata yarıya iner.
4. **Termal dengeleme ve arıza izolasyonu.** Daha sıcak olan MOSFET'ten yük kaydırılabilir, arızalı kanal kapatılabilir.

Kanal başına DAC çözünürlüğü değişmez; iki DAC da 12 bit. Kazanç esas olarak **kalibrasyon ve dengeleme** tarafında.

**Risk:** Firmware'deki bir hata tek kanalı tam skalaya sürerse o MOSFET iki katı güç harcar. Kanal başına güç ve SOA limiti firmware'de zorunlu olmalı.

### Datasheet değerleri (doğrulandı, 14.10.2026)

**STM32G071 DAC** — DS12232 Rev 5, Tablo 61 ve 62 (st.com):

| Parametre | Buffer ON | Buffer OFF |
|---|---|---|
| Çıkış aralığı | **0.2 V … VREF+ − 0.2 V** | **0 … VREF+** |
| Yük / çıkış empedansı | R_L ≥ 5 kΩ (GND'ye), C_L ≤ 50 pF | **R_O = 9.6 / 11.7 / 13.8 kΩ** (min/typ/max) |
| DNL (max) | ±2 LSB | ±2 LSB |
| Monotonluk | **10 bit garanti** | **10 bit garanti** |
| INL (max) | ±4 LSB | ±4 LSB |
| Ofset, kod 0x800 (max) | ±12 LSB (VREF+ = 3.6 V) | ±8 LSB |
| Ofset, kod 0x001 (max) | — | ±5 LSB |
| Kazanç hatası (max) | ±0.5 % | ±0.5 % |
| TUE (max) | ±30 LSB (kalibrasyon sonrası ±23) | **±12 LSB** |
| Oturma süresi (tam skala, ±1 LSB) | 1.6 µs typ / 2.9 µs max | 2.5 µs max |

**STM32G071 VREFBUF** (aynı datasheet):
- VRS = 1 → 2.497 / 2.5 / 2.503 V (100 µA yük, 30 °C).
- VRS = 0 → 2.038 / 2.042 / 2.046 V.
- Sıcaklık katsayısı ≤ 50 ppm/°C, yük akımı ≤ 4 mA.
- 2.5 V modu için VDDA ≥ 2.8 V gerekir; 3.3 V ile sorun yok.

**MCP4725** (microchip.com ürün sayfası):
- INL max 14.5 LSB, oturma süresi 6 µs.
- **Durum: "Not Recommended for new designs".** Microchip yerine MCP4726'yı öneriyor (In Production, INL max 14.5 LSB, harici VREF veya VDD referansı).
- MCP4725'in DNL ve ofset değerlerini okuyamadım; PDF şifreli.

### Değerlendirme

- **Buffer ON kullanılamaz.** Çıkış 0.2 V'un altına inmez. 0.2 V × 0.180 = 36 mV setpoint, yani kanal başına ~0.2 A'in altına inilemez.
- **Buffer OFF + op-amp izleyici sorunsuz.** R_O (~12 kΩ) doğrudan 12.2 kΩ'luk bölücüye bağlansaydı ~%50 kazanç kaybı olurdu. Op-amp'in yüksek empedanslı girişi bu sorunu ortadan kaldırır.
- **Doğruluk:** Buffer OFF'ta INL ±4 LSB, yani MCP4725'in ±14.5 LSB değerinden daha iyi. TUE ±12 LSB ≈ kanal başına ±17 mA. Bu hata ADS1115 ile yapılan kanal başına kalibrasyonla giderilir.
- **Zayıf nokta: monotonluk sadece 10 bit garanti.** Kanal başına 1.45 mA'lik 12 bit adımlarda ±1–2 LSB'lik "geri adım" görülebilir. Garanti edilen monotonik adım 4 LSB ≈ 5.8 mA. ADS1115 geri beslemeli bir firmware döngüsünde bu, ±1–2 LSB'lik salınım (dither) olarak görünür. Döngüye ölü bant (deadband) koymak yeterli.
- **ELV2 için de geçerli:** ELV2'de PA4 → R79 → "DAB BUS" → 4× (12k/10k) bölücü var. Bu yük ~5.5 kΩ ediyor ve 5 kΩ sınırına çok yakın; ayrıca buffer ON gerektiriyor. Dolayısıyla 0.2 V taban sorunu ELV2'de de var.

### Seçenekler

| | A) Dahili DAC ×2 + op-amp izleyici | B) 2× MCP4726 | C) Mevcut (tek MCP4725) |
|---|---|---|---|
| Bağımsız kanal | ✅ | ✅ | ❌ |
| 0 V'a inme | ✅ | ✅ | ✅ |
| INL (max) | ±4 LSB | 14.5 LSB | 14.5 LSB |
| Monotonluk garantisi | 10 bit | teyit edilmedi | teyit edilmedi |
| Ek parça | 1× OPA2197 (+15 V) + 1× kelepçe MOSFET + 1 bölücü | 2× MCP4726 (MCP4725 ile pin uyumu teyit edilmeli) + 1× kelepçe MOSFET + 1 bölücü | — |
| Çıkarılan parça | U2 MCP4725, R1 | U2 MCP4725 | — |
| İki kanalın güncellenmesi | Aynı anda (DHR12RD), DMA/timer ile dinamik yük testi | I2C üzerinden sırayla | — |
| Referans | VREF+ (filtreli 3.3 V) veya VREFBUF 2.5 V (±0.12 %, ≤50 ppm/°C) | VDD veya harici VREF | 3.3 V LDO |
| Parça durumu | MCU'nun içinde | In Production | **NRND** |
| Elindeki MCP4725'ler | Boşa çıkar | Boşa çıkar | Kullanılır |

VREFBUF notu: VREF+ = 2.5 V yapılırsa LOAD VOLTAGE ADC aralığı da değişir (R54/R55 47k/3.3k: 3.3 V'ta 50.3 V maksimum, 2.5 V'ta 38.1 V). Ayrıca setpoint bölücüsü yeniden hesaplanmalı (örneğin R4 = 3.16k ile tam skala 0.600 V).

### Öneri

**A.** Kanal başına: PA4/PA5 (buffer OFF) → OPA2197 gerilim izleyici → mevcut R2/R4 + C11/C13 bölücü ve filtresinin kopyası → R6/R7.
- İki kelepçe MOSFET'in gate'leri aynı DAC CONTROL (PC6) hattına bağlanır, ortak R3/R5 ile.
- Kalibrasyon kanal başına iki noktada, ADS1115 okumasıyla yapılır. Firmware döngüsünde ±2 LSB ölü bant olmalı.
- MCP4725 NRND olduğu için yeni revizyonda B, ancak MCP4726'ya geçilerek mümkün. MCP4726 MCP4725 ile pin uyumlu olmayabilir (A0 yerine VREF pini olduğunu tahmin ediyorum; datasheet DS22272'den teyit edilmeli). Bu da B'nin "mevcut zincirin birebir kopyası" avantajını zayıflatıyor.

## 7. Ters polarite koruması — analiz (14.10.2026)

### Devre (netlist'ten)

```
J6.2 (DUT +) ──F2 15A── VCC LINE ── U7/U8 drain (yük MOSFET'leri)
   │
   ├─ R54 47k ─┬─ LOAD VOLTAGE → PA3 (ADC)
   │           └─ R55 3.3k ─ GND
   │
   └─ U13.2 (LED K) ── LED ── U13.1 (LED A) ── D7 (K←A) ── R44 5.6k ── GND1
                 └── R48 100k (LED'in paralelinde) ──┘

J6.1 (DUT −) = GND1 ── U10 IRFB4110 drain
                       U10 source = board GND
                       U10 gate ── R38 100k ── +15V
                                ├─ D4 12V zener → GND (gate kelepçesi)
                                ├─ Q3 drain (Q3 gate: R40 ← /OUT CONTROL ← R42 10k pull-up 3.3V)
                                └─ D5 (A) → (K) U13.4 opto kollektör ── D8 (K←A) ── R47 100Ω → PA2 (RCP)
                                                                              └─ R45 47k → 3.3V
                       U13.3 opto emiter = GND
```

### Nasıl çalışıyor

| Durum | U10 | Akım yolu | RCP (PA2) |
|---|---|---|---|
| DUT doğru, OUT CONTROL HIGH/Hi-Z (varsayılan) | Kapalı (Q3 gate'i çekiyor) | **U10 body diyodu iletir** (S→D, ~0.5–1 V düşüm) | HIGH |
| DUT doğru, firmware OUT CONTROL = LOW | Açık (Vgs ≈ 12 V, R_DS(on) ≈ 4 mΩ) | Kanal | HIGH |
| DUT ters, U10 kapalı | Kapalı | Body diyodu ters kutuplu, **akım yok** | LOW (Vin ≳ 5 V ise) |
| DUT ters, U10 açıkken (sıcak takma) | Opto, D5 üzerinden gate'i çeker → kapanır | Kapanana kadar U7/U8 body diyotları + şöntler | LOW |

**Tasarımın mantığı doğru.** U10 S = board GND, D = GND1 yönünde bağlı. Body diyodu doğru polaritede iletir, ters polaritede bloklar. Gate işlemciden bağımsız olarak +15V'tan beslendiği için düşük DUT geriliminde (2 V) bile U10 tam açılır. Ters polaritede koruma opto'ya bağlı değil; body diyodu ve varsayılan "kapalı" durum her gerilimde korur.

**Düzeltme:** "U10 kapalıyken güç kesik" doğru değil. U10 kapalıyken body diyodu doğru polaritede iletir. Firmware, U10'u açmadan yük akımı vermemeli (DAC CONTROL'ü bırakmamalı). Yoksa 10 A × ~0.8 V ≈ 8 W, soğutucusu olmayan body diyodunda harcanır.

### Bulgular (önem sırasıyla)

1. **J5 ("outside for batteries") korumayı tamamen atlıyor.**
   - J5.1 doğrudan board GND'ye, J5.2 doğrudan VCC LINE'a bağlı.
   - Yolda U10 yok, **F2 sigortası yok**, opto/RCP algılaması yok.
   - Ters bağlanan bir batarya, U7/U8 body diyotları ve şöntler (kanal başına 2 x 0.1 Ω paralel = 0.05 Ω, 2512) üzerinden kısa devre olur. 3 V'luk bir hücrede bile (3 − 0.7) / 0.025 ≈ 92 A eder.
   - Ayrıca J5'in eksi ucu doğrudan board GND'ye, dolayısıyla USB üzerinden PC GND'sine bağlı.
   - → J5 ya U10'un arkasına (GND1 tarafına) ve F2'nin önüne alınmalı ya da kaldırılmalı.
2. **/OUT CONTROL işlemciye bağlı değil.** → **ÇÖZÜLDÜ (18.10.2026).** PB12'ye (U20.24) bağlandı, netlist ile doğrulandı. Reset anında R42 pull-up yolu kapalı tutuyor; firmware LOW vererek açar. "Tüm yük akımı body diyodundan geçer" riski kalktı. Firmware yük akımı vermeden önce U10'u açmak zorunda (bkz. yukarıdaki Düzeltme).
3. **Ters algılama eşiği yaklaşık 5 V.**
   - Opto'nun sürmesi gereken akım: R38 üzerinden (15 − 2) V / 100 k ≈ 130 µA + R45 üzerinden ≈ 55 µA → ~190 µA.
   - LTV-817 CTR'si (5 mA'de min %50) düşük IF'de daha da düşer. Güvenli algılama için IF ≈ 0.6 mA gerekiyor: Vin ≈ 1.7 V + 0.6 mA × 5.6 k ≈ **5.2 V**.
   - 2–5 V ters bağlantıda **koruma çalışır ama uyarı garanti değil.**
   - → İsteğe bağlı: R44 = 2.7 k / 2512. Bu durumda 3 V'ta IF ≈ 0.5 mA olur; 30 V'ta 10.4 mA ve 0.29 W.
4. **Opto ile donanımsal kapatma yavaş.** IRFB4110'un gate yükü ~150 nC; opto yaklaşık 0.2–1 mA ile deşarj eder, bu da **yüzlerce µs – ms** sürer. Yani bu katman sadece ikinci savunma hattı. Asıl önlem şu: firmware LOAD VOLTAGE kaybolunca U10'u hemen kapatmalı (bağlantı kesilince ms içinde).
5. **RCP lojik-0 seviyesi sınırda.** Opto açıkken PA2 ≈ V_CE(sat) + V_F(D8) ≈ 0.6–0.7 V; soğukta daha yüksek. G0'ın V_IL değeri ~0.3 × VDD ≈ 1 V (datasheet'ten teyit et). Marj ~0.3 V.
   - → D8'i BAT54 (Schottky) yap ya da PA2'yi ADC ile oku (PA2 = ADC1_IN2).
   - D8'in kendisi gerekli: Opto kapalıyken kollektör düğümü D5 üzerinden ~11 V'a çıkar, D8 bunun MCU'ya ulaşmasını engeller.
6. **Güç açılış sırası.** +15V (LM7815, hızlı) 3.3 V'tan (TPS54302 soft-start → 5 V → AP2112) önce gelirse:
   - Q3'ün gate'i 0 V'ta kalır.
   - U10 gate'i R38 üzerinden yükselir ve U10 birkaç ms **kısa süreliğine açılabilir**.
   - Doğru polaritede zararsız. Ters ve 5 V'tan düşük bir kaynak takılıyken kart açılırsa kısa bir ters akım darbesi olur.
   - → Osiloskopla U10 gate'ini kart açılışında ölç.
7. **Girişte TVS yok.** ELV2'de SMCJ33CA var. 2–30 V aralığı için ELV1 revision'a da 33 V'luk çift yönlü TVS (J6 uçlarına) eklenebilir.

### 2–30 V aralığında parça stresleri

| Parça | 30 V'ta stres | Sınır | Durum |
|---|---|---|---|
| D7 1N4148W (doğru polaritede ters gerilim) | 30 V | V_R 75 V | ✅ |
| R44 5.6k 1206 (ters 30 V) | 5 mA, 0.14 W | 0.25 W | ✅ |
| LTV-817 LED (ters 30 V) | I_F ≈ 5 mA | 50 mA | ✅ |
| LTV-817 V_CE (kollektör ~11 V) | 11 V | V_CEO 35 V | ✅ |
| U10 IRFB4110 V_DS (ters 30 V) | 30 V | 100 V | ✅ |
| LOAD VOLTAGE ADC (47k/3.3k) | 1.97 V | 3.3 V → ölçüm üst sınırı 50.3 V | ✅ |

### J5 kararı — öneri: J6'ya paralel bağla

**Durum (14.10.2026):** J5 şimdilik olduğu gibi bırakıldı; F2 sigortasını ve ters polarite korumasını atlamaya devam ediyor. Aşağıdaki öneri ileride değerlendirilmek üzere duruyor.

J5'in korumayı atlaması tasarım tercihiydi: XT60 ters takılamıyor ve U10'un direnci istenmiyordu. Yine de paralel bağlamak öneriliyor.

**U10'un etkisi küçük.** IRFB4110 için R_DS(on) 25 °C'de 3.7 mΩ typ / 4.5 mΩ max, ~100 °C'de yaklaşık 7.6 mΩ.

| Akım | Düşüm (25 °C max / ~100 °C) | Güç (U10) | 2 V'ta hata | 30 V'ta hata |
|---|---|---|---|---|
| 5 A | 22 / 38 mV | 0.11 / 0.19 W | %1.1 / %1.9 | %0.08 / %0.13 |
| 10 A | 45 / 77 mV | 0.45 / 0.76 W | %2.3 / %3.8 | %0.15 / %0.26 |
| 15 A | 68 / 115 mV | 1.0 / 1.7 W | %3.4 / %5.7 | %0.23 / %0.38 |

- Karşılaştırma için: şönt düşümü 7.5 A/kanalda **0.75 V**. Yani minimum çalışma gerilimini U10 değil şöntler belirliyor.
- U10'un düşümü LOAD VOLTAGE ölçümüne girer, çünkü ölçüm J6.2 ile board GND arasında. Firmware'de `V_dut ≈ V_ölçülen + I × R_DS(on)` (yaklaşık 4–5 mΩ) ile düzeltilebilir. ADS1115 toplam akımı zaten ölçüyor.

**Paralel bağlamanın kazancı:**
1. **F2 sigortası.** Şu an J5 sigortayı da atlıyor. Lineer modda çalışan IRFP260N'lerin en olası arıza biçimi drain–source kısa devresi. Böyle bir arızada bataryanın akımını sınırlayan hiçbir şey yok. Bu, ters polariteden daha olası bir risk.
2. **Ters bağlantı koruması ve RCP uyarısı.** XT60 soket ters takılamaz, ama adaptör kablolar (timsah→XT60) ve el yapımı batarya kabloları ters lehimlenebilir. J6 da XT60 olduğu hâlde orada korumaya gerek duyulmuş; aynı mantık J5 için de geçerli.
3. **Ortak GND.** Şu an J5 eksi ucu doğrudan board GND'ye, dolayısıyla USB üzerinden PC GND'sine bağlı. Paralel bağlantıda iki giriş aynı yoldan geçer.

**Bağlantı:** J5.2 → J6.2 düğümüne (F2'nin dış tarafı; R54, U13.2 ile aynı net). J5.1 → GND1.

**Termal kontrol (J6 için de geçerli):** 15 A'de U10 1–1.7 W harcar. TO-220 yatay, tab aşağıda montajda tab'ın altına geniş bakır alan ve termal via'lar konmalı, gerekirse küçük bir soğutucu kullanılmalı.

## 8. U20 çevre elemanları — datasheet kontrolü (14.10.2026)

Kaynaklar:
- DS12232 Rev 5 (Desktop/DS_stm32g071cb.pdf): Şekil 13 "Power supply scheme", Tablo 39 "HSE oscillator", Şekil 23 "NRST protection".
- AN2867 Rev 24.
- Abracon ABM7 datasheet ve ABM7-8.000MHZ-D2Y-T ürün sayfası (CL = 18 pF, ESR max 100 Ω, ±20 ppm / ±30 ppm, −40…85 °C).

| Eleman | Şematikte | Datasheet / AN | Durum |
|---|---|---|---|
| VDD (pin 6): C60 + C61, FB4 üzerinden | 100 nF + **1 µF** | **100 nF + 4.7 µF** (Şekil 13) | ⚠️ **C61 → 4.7 µF** (0805, ≥10 V, X5R/X7R) |
| VBAT (pin 4): C59 | 100 nF, +3.3V'a bağlı | Ayrı bir şart yok (RTC/LSE kullanılmıyor) | ✅ |
| VREF+ (pin 5): C57 + C56, FB3 üzerinden | 100 nF + 1 µF | 100 nF + 1 µF (1 µF sadece VREFBUF için zorunlu) | ✅ |
| NRST (pin 10): C69, R100, SW3 | 100 nF + 10 k pull-up | 0.1 µF önerilir; dahili R_PU 25/40/55 kΩ | ✅ (R100 gereksiz ama zararsız) |
| HSE: C54 = C55 | **18 pF** | C1 = C2 = 2 × (CL − Cs) = 2 × (18 − 7) ≈ **22 pF** | ⚠️ önerilen **22 pF** |
| HSE: R78 (OSC_OUT), R77 (OSC_IN) | 0 Ω | AN2867: R_Ext gerekirse OSC_OUT tarafına | ✅ R78 footprint'i R_Ext için yedek kalsın |
| D13 + R80 (PB0) | 330 Ω | (3.3 − ~2.0) / 330 ≈ 4 mA | ✅ |

### Kristal hesabı

- **Kazanç (AN2867, G0 için Gm = 1.5 mA/V; ×5 marj bu değerin içinde):**
  - gm_crit = 4 × ESR × (2πF)² × (C0 + CL)²
  - ESR = 100 Ω (max), F = 8 MHz, CL = 18 pF ile:
    - C0 = 7 pF (varsayım; ABM7 datasheet'inde bu satır okunamadı) → **0.63 mA/V**
    - C0 = 5 pF → 0.54 mA/V
  - Sonuç < 1.5 mA/V → **✅ salınım garanti**. Marj, sınırın 2.4–2.8 katı.
- **Yük kapasitesi:** CL_eff = C54 × C55 / (C54 + C55) + Cs. Cs, pin ve PCB kapasitesi: ST'nin önerdiği aralık 5–10 pF.

| C54 = C55 | Cs = 5 pF | Cs = 7 pF | Cs = 10 pF |
|---|---|---|---|
| 18 pF (şu an) | 14.0 pF | 16.0 pF | 19.0 pF |
| **22 pF** | 16.0 pF | **18.0 pF** | 21.0 pF |
| 27 pF | 18.5 pF | 20.5 pF | 23.5 pF |

18 pF ile kristal büyük ihtimalle yük kapasitesinin biraz altında çalışır. Frekans birkaç on ppm yukarı kayar. Bu kartta (UART, zaman sayımı) etkisi pratikte yok, ama 22 pF hedefi daha iyi ortalar.

### Notlar

1. **VREFBUF bu donanımla açılmamalı.** VREF+, FB3 üzerinden 3.3V'a bağlı. VREFBUF açılırsa dahili 2.5 V buffer harici 3.3V ile çakışır. Firmware'de VREFBUF kapalı kalmalı; varsayılan zaten kapalı.
2. **HSE zorunlu değil.** HSI16 (±%1) UART ve zamanlama için yeterli. HSE'nin avantajı uzun süreli zaman ve Ah ölçümünde ppm seviyesinde doğruluk. Kullanılmayacaksa Y2/C54/C55 DNP bırakılabilir.
3. **Yerleşim:** C60/C61 VDD pinine, C56/C57 VREF+ pinine, C69 NRST pinine, Y2/C54/C55 PF0/PF1'e mümkün olduğunca yakın konmalı. Kristalin altında başka sinyal geçmemeli; altı GND ile dolu olmalı.

## 9. Errata kontrolü — STM32G071 (ES0418 Rev 6, Eylül 2026)

### Eski G030'daki kristal sorunu neden çıktı

STM32G030F6P6 (TSSOP20) CubeMX veritabanına göre:
- HSE'nin giriş pini **OSC_IN, pin 2'de PC14 ve PB9 ile aynı ped üzerinde (çift bağlı)**.
- HSE için **OSC_OUT pini hiç yok.** PC15 sadece LSE çıkışı (OSC32_OUT).
- Sonuç: o kılıfta 8 MHz kristalle Pierce osilatörü kurulamaz; sadece harici saat (HSE bypass) kullanılabilir. Bu bir errata değil, kılıfın pin kısıtı.

**G071CBT6 (LQFP48):** PF0-OSC_IN (pin 8) ve PF1-OSC_OUT (pin 9) ayrı, tek bağlı pinler. LSE pinleri (PC14/PC15) de ayrı. Kristal sorunu tekrarlanmaz.

### Bu tasarımı ilgilendiren errata maddeleri

Revizyonlar ve REV_ID değerleri (DBGMCU_IDCODE): **B = 0x2000**, **Y/1/2 = 0x2001**, **X/3 = 0x2003**.

| Madde | Konu | Revizyon | Etki ve önlem |
|---|---|---|---|
| 2.2.5 | 0xFFFF…FF yazılmış bir flash double-word'ünün üzerine 0x0000…00 yazılamıyor (PROGERR) | Hepsi, workaround yok | Kalibrasyon/ayar kaydında "geçersiz kılmak için 0 yaz" yöntemi kullanılmamalı. Sayfa silip yeniden yaz ya da erased (0xFF) kontrolü kullan. |
| 2.11.6 | I2C ilk byte'tan sonra kilitlenebiliyor (APB / I2C kernel saat oranı 1.5–3 arasıysa) | Hepsi, workaround var | I2C2 saati = PCLK (oran 1) kalmalı; CubeMX varsayılanı zaten böyle. ADS1115 ve MCP4725 için önemli. |
| 2.11.2 | Controller modunda sahte BERR | Hepsi, workaround var | BERR gelirse bayrağı temizle, transfer normal devam eder. HAL'in error callback'inde transferi iptal etme. |
| 2.11.1 | t_SU;DAT < 1 I2C kernel saat periyodu olursa yanlış örnekleme | Hepsi, kısmi | 64 MHz PCLK ile periyot 15.6 ns ≪ 100 ns; sorun yok. |
| 2.12.2 | USART RX hattında stop bitinin ikinci yarısındaki kısa glitch veri bozar | Hepsi, workaround var | CH340G → PB7 hattı kısa ve kart içinde; normalde sorun olmaz. |
| 2.6.5 | ADC örnekleme süresi 1.5 veya 3.5 cycle seçilirse 1 cycle uzun | Hepsi | LOAD VOLTAGE için zaten uzun örnekleme süresi kullanılacak (kaynak ~3 kΩ + C40). |
| 2.6.2 / 2.6.4 | ADEN = 1 iken CFGR1/CFGR2 yazılırsa RES/CKMODE sıfırlanır | Hepsi | ADC'yi ADEN'den önce yapılandır; HAL bu sırayı izliyor. |
| 2.2.6 | HSIDIV ≠ 0 iken Stop moduna girilemiyor / uyanılamıyor | Hepsi | Stop modu kullanılmıyor → etkisiz. |
| 2.2.9 | Option byte uyuşmazlığında cihaz kilitlenir (debug kapanır) | **Sadece Rev B** | Rev B çipte option byte (nBOOT_SEL, NRST modu, RDP) değiştirirken dikkatli ol. Yarıda kalan bir yazma çipi kurtarılamaz hâle getirebilir. |
| 2.2.3, 2.2.4, 2.2.7, 2.6.7, 2.8.6 | BOOT0/RDP, DMAMUX-EXTI, PCROP, ADC ofseti, TIM16/17 saat | **Sadece Rev B** | Rev Y/X'te yok. Buzzer PWM'i için TIM16 (PA6) Rev B'de yanlış saatle çalışabilir; TIM3_CH1 tercih edilebilir. |
| 2.2.8 | PC13 geçişleri LSE'yi bozar | Hepsi | LSE kullanılmıyor, PC13 boşta → etkisiz. |
| 2.7.x, 2.3.1, 2.15.x | DAC, DAC-GPIO, UCPD | — | Bu tasarımda kullanılmıyor. |

**Sonuç:** Tasarımı engelleyen kritik bir kusur yok. Dikkat gereken maddeler firmware tarafında: I2C saat oranı, sahte BERR ve flash kayıt yöntemi. Çipleri aldığında üzerindeki revizyon harfine bak ya da firmware'de REV_ID'yi oku. Rev B ise option byte işlemlerinde dikkatli ol.

### Ölçüm notu (17.10.2026) — ilk ELV1 kartı

İlk ELV1 kartında ters bağlantı uyarısı **1.71 V**'ta tetiklendi; tek numune, oda sıcaklığı. ELV1'deki devre bu revizyondan farklı:

| | ELV1 (ölçülen kart) | ELV1 revision (tasarım) |
|---|---|---|
| Koruma MOSFET'i | IRF4905, high-side P-MOS | IRFB4110, low-side N-MOS (U10) |
| LED yolu | R40 5.6k → LED (seri diyot yok) | R44 5.6k → **D7** → LED, R48 100k paralel |
| Opto çıkış yükü | R42 10k pull-up → ~230 µA | R45 47k (~55 µA) + D5 üzerinden U10 gate'i (R38 100k, ~130 µA) |
| Opto MOSFET'i kapatıyor mu | Hayır, sadece RCP | Evet, D5 ile gate'i çekiyor |

ELV1'deki ölçümden bu numunenin CTR'si yaklaşık **%190** çıkıyor (1.71 V'ta IF ≈ 0.12 mA, gereken Ic ≈ 0.23 mA). Aynı CTR ile revizyon için tahmini eşikler:
- **RCP uyarısı ~1.6–1.7 V.** Çıkış yükü daha hafif, ama D7 ~0.5 V ekliyor.
- **U10 gate'ini kapatma ~2.2 V** (IF ≈ 0.1 mA, Ic ≈ 190 µA).
- Sınıfsız LTV-817 (CTR %50) ile RCP ~2.3 V, gate kapatma ~3.8 V olur.

→ BOM'da LTV-817'nin sınıf harfini belirt (C veya D). Revizyon kartı gelince ters gerilimi yavaşça artırarak RCP ve U10 gate eşiğini ayrı ayrı ölç.

## 10. Açık sorunlar (17.10.2026)

### 10.1 AC girişteki NTC (TH1, 10 Ω)

- TH1, trafo sekonderinde, F1 ile köprü (D1) arasında. Kondansatör girişli doğrultucuda AC RMS akımı, DC akımın yaklaşık 2.2 katı olur.
- **Soğuk NTC'de kaba tahmin:** 0.4 A DC'de ~0.9 A RMS → 10 Ω'da ~9 V RMS düşüm. NTC ısındıkça direnci düşer, ama küçük akımlarda yeterince ısınmaz ve hat gerilimini aşağı çeker. LM7815'in en az 17 V ihtiyacı burada risk altında.
- **NTC olmadan ilk dolma akımı:** Sekonder direnci 0.5–1 Ω, tepe gerilim 25 V varsayımıyla:
  - Tepe akım 25–50 A, I²t ≈ 2–4 A²s.
  - KBU8M'in tek darbe dayanımı (IFSM) 200 A. T2A sigortanın erime I²t değeri bundan çok daha yüksek; tipik olarak 10 A²s'nin üzerinde, sigortanın datasheet'inden teyit et.
- **Öneri:** TH1'i 0 Ω (veya köprü teli) yap ya da 1–2.5 Ω, en az 3 A'lik bir NTC kullan. Ani akım sınırlama gerekiyorsa NTC'nin doğru yeri trafonun primer (şebeke) tarafı.

### 10.2 LOAD VOLTAGE düşük okuyor (ELV1: 5 V kaynakta 4.8 V)

ELV1'de bulunanlar:
- PA1 (LOAD VOLTAGE) koruma P-MOS'unun içinden ölçüyor ("DRAIN SIDE (IN)"; 470k/33k, C30 100 nF).
- Firmware'de bölücü 47k/3.3k olarak tanımlı; oran aynı.
- **Firmware sabit `LV_OFFSET_MV = 1500` çıkarıyor.**
- `ADC_VREF_MV = 3300` sabit; gerçek VDDA ölçülmüyor.

Hata kaynakları:
- Sabit ofset çıkarmak, kazanç hatasını sadece tek bir gerilimde düzeltir.
- VDDA ±%1–2 sapabilir (AP2112 ±%1.5); bu doğrudan okuma hatasına yansır.
- Dirençlerin toleransı da oranı etkiler.
- Ölçüm noktası koruma MOSFET'inin arkasında; MOSFET kapalıyken body diyodu üzerinden düşüm olur.

**Teşhis:** Kaynağı 2–3 farklı gerilimde (örneğin 5, 15, 25 V) BM257s ile ölç. Aynı anda ham ADC değerini ve ADC pinindeki gerilimi kaydet. Hata sabitse ofset, gerilimle orantılıysa kazanç hatasıdır.

**Revizyonda dikkat:** U10 kapalıyken bölücü akımı (5 V'ta ~0.1 mA) U10'un body diyodundan döner. Bu yüzden board GND, GND1'den ~0.4–0.5 V yukarıda kalır ve LOAD VOLTAGE o kadar düşük okur. U10 açılınca düşüm I × R_DS(on) seviyesine iner.

**Firmware'de:**
1. VREFINT + fabrika kalibrasyon değeriyle (`VREFINT_CAL`) gerçek VDDA'yı hesapla.
2. İki noktalı kalibrasyon yap ve kazanç ile ofseti flash'ta sakla (errata 2.2.5'e dikkat; bkz. Bölüm 9).
3. Hassas okumayı yalnızca U10 açıkken yap. U10 kapalıyken sadece "kaynak var / yok" kararı ver.

### 10.3 ELV1 firmware incelemesi ve revizyon kararları (17.10.2026)

Kaynak: `~/Documents/GitHub/ELV1-revision/firmware/Core/Src/main.c` (son commit f1d0f2d; Desktop'taki "ELV1 CODE FİNAL.txt" ile aynı).

**ELV1 kodunda gerilim ölçümünü bozan noktalar:**

| # | Kodda | Etki (5 V'ta) |
|---|---|---|
| 1 | `HAL_ADCEx_Calibration_Start()` **hiç çağrılmıyor** | ADC doğruluk değerleri (Tablo 60) kalibrasyon sonrası için geçerli. Kalibrasyonsuz ofset onlarca LSB olabilir; 1 LSB ≈ 12.3 mV giriş. |
| 2 | Ekranda `mv/1000 . (mv%1000)/100` → **yuvarlama yok, kesme var** | Gösterilen değer 0–99 mV aşağıda (4.89 V → "4.8") |
| 3 | `LV_OFFSET_MV = 1500` sabit çıkarılıyor (korumalı yol, PA1) | Gerçek ofset 1.5 V değilse hata aynen ekrana yansır; kazanç hatasını düzeltmez |
| 4 | `ADC_VREF_MV = 3300` sabit | VDDA ±%1.5 (AP2112) → ±75 mV |
| 5 | Korumasız yol (PB7): 470k/33k, **kondansatör yok**; Thevenin ≈ 31 kΩ | 12 bit, 79.5 cycle için izin verilen RAIN ~10 kΩ (Tablo 59). Örnekleme kondansatörü dolmaz ve **düşük okur**. |
| 6 | Bölücü direnç toleransı | %1 dirençle oran ±%1.4'e kadar → ±70 mV (30 V'ta ±0.4 V) |

Bu ölçümle 1, 2 ve 3'ü birbirinden ayıramıyorum. Ama 4.8 V okuması 1 ve 2'nin toplamıyla tutarlı.

**Dahili ADC'nin sınırı:** Kalibrasyondan sonra TUE ±4–7.5 LSB (≈ ±50–90 mV). İki noktalı kullanıcı kalibrasyonuyla en iyi ~±40 mV. 0.01 V çözünürlüklü bir ekran için yetersiz.

**Revizyon kararları — donanım:**
1. **Gerilim ölçümü için ikinci bir ADS1115** (U9'un yanına, adres 0x49: ADDR → VDD).
   - **AIN0–AIN1: DUT terminal gerilimi, diferansiyel.**
     - AIN0 ← J6.2 üzerinden 47k / 3.3k.
     - AIN1 ← GND1 (J6.1) üzerinden 47k / 3.3k.
     - Ölçüm doğrudan konnektörün iki ucu arasında yapılır. U10'un R_DS(on) ve body diyodu düşümü, GND yolundaki bakır düşümleri ölçüme girmez.
   - **PGA ±2.048 V:** 31.2 V'a kadar ölçer; 1 LSB = 62.5 µV ≈ 0.95 mV giriş.
   - **Dirençler 0.1 %, ≤ 25 ppm/°C** olmalı ve iki bacakta eşleşmeli (CMRR için).
   - **Giriş filtresi:** Akım kanallarındaki gibi 100 Ω seri + 100 nF NPO diferansiyel + 10 nF ortak mod.
   - **Ters bağlantı:** AIN0 ~−1 V'a iner. ADS1115 girişindeki akım 47k ile ~20 µA'e sınırlanıyor; yine de AIN0/AIN1'e BAT54S clamp eklenmesi önerilir.
   - **AIN2–AIN3 (öneri):** F2'nin iki ucu (J6.2 ve VCC LINE, aynı bölücülerle). Sigortanın atması ve kablo düşümü algılanır.
2. **PA3 (dahili ADC) kalsın.** Hızlı "kaynak var / yok" ve aşırı gerilim koruması için kullanılır, ekranda gösterilmez.
3. **İsteğe bağlı 4 telli ölçüm:** SENSE+ / SENSE− konnektörü. Lehim jumper'ı (JP) ile varsayılan olarak J6'ya bağlı olur. Geliştirme kartı kullanımı için faydalı.
4. **R54/R55 için 0.1 % direnç**, ya da firmware kalibrasyonuna güven.

**Revizyon kararları — firmware:**
1. Açılışta `HAL_ADCEx_Calibration_Start(&hadc1)` çağır; ADEN'den önce yap (errata 2.6.2 / 2.6.4).
2. VDDA'yı VREFINT ve `VREFINT_CAL` ile ölç (PA3 yolu için).
3. Donanım oversampling'i kullan (16×, shift 4) ya da ortalama al.
4. `LV_OFFSET_MV` sabitini kaldır. Kanal başına iki noktalı kalibrasyon (kazanç + ofset) yap ve flash'ta sakla (errata 2.2.5'e dikkat).
5. Ekranda yuvarla: `(mv + 5) / 10` gibi; kesme kullanma.
6. Ekran ve güç hesabı için ADS1115 #2'nin diferansiyel gerilimini kullan. Akım ADS #1'de olduğu için V ve I aynı anda örneklenir.

### 10.4 İkinci ADS1115 şematiğe eklendi (18.10.2026)

U9 bloğu kopyalandı; kopya U21 oldu (adres 0x49, ADDR → "ADS2 VDD").

| Giriş | Net | Kaynak |
|---|---|---|
| AIN0 (R105 100R) | LOAD VOLTAGE | J6.2 → R54 47k / R55 3.3k (+ C40), PA3 ile ortak |
| AIN1 (R106 100R) | VS- | GND1 → R101 47k / R102 3.3k |
| AIN2 (R107 100R) | VLINE DIV | VCC LINE → R103 47k / R104 3.3k |
| AIN3 (R108 100R) | LOAD VOLTAGE | AIN0 ile aynı düğüm |

- **AIN0–AIN1:** DUT terminal gerilimi, diferansiyel.
- **AIN2–AIN3:** F2 üzerindeki düşüm; sigortanın atıp atmadığı buradan izlenir.
- **PGA ±2.048 V:** 31.2 V'a kadar ölçer.
- **Bölücüler:** Altı direncin hepsi (R54/R55 dahil) "0.1 %" olarak işaretlendi. Footprint 0805.
- **Ters bağlantıda:** AIN0/AIN3 ~−2 V'a iner; akım bölücüyle ~0.5 mA'e sınırlanır.
- **Yedek:** Değişiklik öncesi şematik `ELV1 revision-backups/ELV1 revision_before_claude_ads2_2026-10-18.kicad_sch`.

### 10.5 Şema doğrulaması — ikinci ADS1115 (18.10.2026)

`kicad-cli sch export netlist` + `erc` ile kontrol edildi. **Bölüm 10.4'teki bağlantılar doğru:** AIN0−AIN1 gerçekten J6.2 ile GND1 arasında diferansiyel ölçüyor (her iki bölücünün alt ucu board GND'de), yani U10'un R_DS(on) ve body diyot düşümü ölçüme girmiyor. ADDR → /ADS2 VDD = **0x49**. Bölücü oranı 3.3/50.3 = 0.065606 → tam skala 31.2 V, 1 LSB ≈ 0.95 mV. Besleme `+3.3V → FB5 600R → C79 10 µF + C81 100 nF`.

PA3 yolundaki eski sorun da çözülmüş: bölücü Thevenin'i artık 47k‖3.3k = **3.08 kΩ** (eskiden 31 kΩ), yani 12 bit / 79.5 cycle için izin verilen R_AIN sınırının içinde (bkz. 10.3, madde 5).

**Dokümandan sapmalar — karar bekliyor:**

| # | Durum | Etki |
|---|---|---|
| 1 | Filtre kondansatörleri **X7R**, doküman NPO/C0G diyordu. Değerler de farklı: C76/C80 = 220 nF (100 nF yerine), C74/C75/C77/C78 = 22 nF (10 nF yerine) | X7R'nin DC-bias katsayısı ve dielektrik absorpsiyonu 16 bit DC ölçümde yerleşme ve doğrusallık hatası yapar. En az C76/C80 C0G olmalı; 0805'te 220 nF C0G pratikte bulunmaz, o yüzden dokümanın kendi değerlerine (100 nF + 10 nF C0G) dönmek daha gerçekçi |
| 2 | **MUX yerleşme süresi hesaba katılmamış.** Diferansiyel çevrim: 3.08k + R105 100R + C76 + R106 100R + 3.08k → R ≈ 6.37 kΩ, C = 220 nF, **τ ≈ 1.4 ms** | 16 bit için ~10τ ≈ 14 ms. 860 SPS'te kanal çiftini değiştirdikten sonra ilk ~12 çevrim atılmalı. C76/C80 = 100 nF yapmak bunu ~6 ms'ye indirir |
| 3 | **BAT54S clamp eklenmemiş** (10.3 öneriyordu, netlist'te yok) | Ters bağlamada /LOAD VOLTAGE düğümü ≈ −2 V'a iner, R54 üzerinden ~0.6 mA akar. Bu akım yalnızca U21'e değil **PA3'e de** gidiyor. STM32G0 I_INJ limiti ±5 mA olduğu için anında hasar vermez ama kalıcı ters bağlantıda riskli. Clamp R105/R108'den **önce**, /LOAD VOLTAGE ve /VS- düğümlerine konmalı — böylece PA3 de korunur |
| 4 | **ALERT/RDY iki ADS'de de boşta.** U21.2 → R109 10k pull-up, U9.2 → R32 pull-up; ikisi de MCU'ya gitmiyor | Firmware kararı 6 "V ve I aynı anda örneklenir" diyor, ama RDY olmadan bu ancak polling ile yaklaşık yapılabilir. Yedek pin bol (PB1, PB2, PB3, PB8) → iki RDY hattını almaya değer |

**ERC durumu (103 ihlal, hiçbiri yeni blokla ilgili değil):** 72 × `pin_not_connected` root sheet'teki hierarchical label'lar, 15 × MCU yedek pinleri (no-connect bayrağı yok), 7 × `power_pin_not_driven` ferrit arkası raylar (U9.8/U21.8 dahil, PWR_FLAG gerekiyor), 5 × `lib_symbol_mismatch`, 4 × `multiple_net_names` (`20V`/`FULL BRIDGE RECTIFIER OUT`, `+12V`/`Opamp VCC`, `+5V`/`5V`, `+3.3V`/`3.3V`).

### 10.6 Kütüphaneler global'e alındı (18.10.2026)

Proje-içi `sym-lib-table` / `fp-lib-table` dosyaları kaldırıldı; artık KiCad'in global tabloları kullanılıyor ve kaynak `~/Documents/Elektronik/Library/`. ELV1 revision'daki sürümler kanonik kabul edildi (sembol kümesi global'in katı üst kümesiydi).

| Değişiklik | Neden |
|---|---|
| `my-NMOS` → **`myNMOS`** | ELV1 revision'ın adlandırması kazandı; içerik üst küme (ek olarak IRFB7530PbF, IRFP260N, PJA3406) |
| `mydiodes` → **`myDiodes`** | Aynı sembol kümesi, ELV1 revision'ın adlandırması |
| `myTO247` **global'e eklendi** | Global fp tablosunda hiç yoktu, ELV1 revision PCB'si `myTO247:TO-247 Edge Castellated` kullanıyor |
| `myNTC` footprint **birleştirildi** | ElectronicLoad `myNTC:Untitled`, ELV1 revision + ELV1 `myNTC:NTC` kullanıyor. İkisi de korundu |
| `myFuse` **global sürüm korundu** | Global 20260206, repo'daki 20241229 — yani repo kopyası daha eski. ELV1 revision bu kütüphaneyi kullanmıyor |
| `xt60-IRFP260N` **silindi** | Tablodaki açıklamasında zaten "kullanilmiyor - my-NMOS ile cakisiyor" yazıyordu; `IRFP260N` artık `myNMOS` içinde |

ELV2, ESC ve ElectronicLoad şemalarındaki `my-NMOS:` / `mydiodes:` referansları yeni isimlere çevrildi (her birinin `*-backups/` klasörüne `*_before_libname_2026-10-18.kicad_sch` yedeği bırakıldı). Beş projenin hepsinde ERC ile doğrulandı: eksik kütüphane yok.

Repo'daki `hardware/libraries/` **kasıtlı olarak korundu** — sürüm kontrolündeki asıl kaynak o. Global kopyayı tazelemek için:

```
rsync -a hardware/libraries/symbols/ ~/Documents/Elektronik/Library/symbols/
rsync -a hardware/libraries/footprints/ ~/Documents/Elektronik/Library/footprints/
```

Eski global tabloların ve kütüphanelerin yedeği: `~/Documents/Elektronik/Library-yedek-2026-10-18/`.

