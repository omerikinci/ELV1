/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c2;

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C2_Init(void);
/* USER CODE BEGIN PFP */
static uint8_t adc_read_channel(uint32_t channel, uint16_t *raw_value);
static uint8_t adc_read_channel_avg(uint32_t channel, uint8_t samples, uint16_t *raw_avg);
static uint32_t adc_raw_to_millivolt(uint16_t raw);
static uint32_t lv_adc_to_input_millivolt(uint16_t raw);
static void delay_us_init(void);
static void delay_us(uint32_t us);
static uint8_t ds18b20_crc8(const uint8_t *data, uint8_t len);
static uint8_t ds18b20_start_conversion(void);
static uint8_t ds18b20_read_temp_centi(int16_t *temp_centi);
static uint8_t ads1115_detect_address(void);
static uint8_t ads1115_read_channel_raw_cfg(uint8_t channel, uint16_t pga_cfg, int16_t *raw_value);
static uint8_t ads1115_read_avg_microvolt_cfg(uint8_t channel, uint16_t pga_cfg, int32_t fs_uv, uint32_t *avg_uv);
static uint8_t ads1115_read_avg_microvolt(uint32_t *avg_uv);
static uint8_t lcd_detect_address(void);
static void lcd_init(void);
static void lcd_set_cursor(uint8_t row, uint8_t col);
static void lcd_print(const char *text);
static void lcd_print_centered(uint8_t row, const char *text);
static void lcd_startup_loading_until_ds_ready(void);
static void shdn_softstart(void);
static void buzzer_tone(uint16_t freq_hz, uint16_t duration_ms, uint16_t duty_permille);
static void buzzer_pattern_startup(void);
static void buzzer_pattern_halt(void);
static void buzzer_pattern_system_on(void);
static void buzzer_pattern_system_off(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define LCD_RS_BIT        0x01U
#define LCD_EN_BIT        0x04U
#define LCD_BL_BIT        0x08U
#define LCD_CMD_MODE      0x00U
#define LCD_DATA_MODE     LCD_RS_BIT
#define LCD_I2C_TIMEOUT   5U
#define LCD_DEFAULT_ADDR  (0x27U << 1)
#define ADC_VREF_MV       3300UL
#define ADC_MAX           4095UL
#define LV_DIV_TOP_OHM    47000UL
#define LV_DIV_BOT_OHM    3300UL
#define LV_OFFSET_MV      1500UL
#define LV_PROTECT_MV     30000UL
#define RV_REVERSE_TRIP_MV 3100UL
#define RESET_ADC_TRIP_MV 1500UL
#define ADS1115_DEFAULT_ADDR (0x48U << 1)
#define ADS1115_REG_CONV     0x00U
#define ADS1115_REG_CONFIG   0x01U
#define ADS1115_CFG_OS_SINGLE      0x8000U
#define ADS1115_CFG_PGA_512MV      0x0800U
#define ADS1115_CFG_MODE_SINGLE    0x0100U
#define ADS1115_CFG_DR_128SPS      0x0080U
#define ADS1115_CFG_COMP_DISABLE   0x0003U
#define ADS1115_FS_UV_CURRENT      512000L
#define ADS1115_SAMPLES_PER_CH     8U
#define CURRENT_ADS_CHANNEL        0U
#define CURRENT_UV_PER_A           29586UL
#define CURRENT_TRIP_MA            10000UL
#define POWER_TRIP_MW              160000UL
#define OCP_CONFIRM_COUNT          3U
#define POWER_CONFIRM_COUNT        3U
#define POWER_CHECK_DELAY_MS       500UL
#define PROT_PRESENT_THRESHOLD_MV  2500UL
#define UNPROT_PRESENT_THRESHOLD_MV 2000UL
#define PROT_MIN_MV                8000UL
#define SET_V_MAX_MV               30000UL
#define SET_A_MAX_MA               10000UL
#define SET_V_ZERO_DEADBAND_MV     1000UL
#define SET_A_ZERO_DEADBAND_MA     350UL
#define ISET_SCALED_MIN            30UL
#define ISET_SCALED_MAX            118UL
#define ISET_MAX_PERCENT           100UL
#define ISET_ZERO_DEADBAND_PERCENT 1UL
#define DS_GPIO_Port      GPIOB
#define DS_Pin            GPIO_PIN_0
#define FAN_GPIO_Port     GPIOA
#define FAN_Pin           GPIO_PIN_6
#define BUZZER_GPIO_Port  GPIOA
#define BUZZER_Pin        GPIO_PIN_7
#define BUZZER_DUTY_PERMILLE  140U
#define TEMP_FAN_ON_CENTI 5000
#define TEMP_FAN_OFF_CENTI 4000
#define DS18B20_CONV_WAIT_MS 850UL
#define DS18B20_RETRY_COUNT  3U
#define DS18B20_FAIL_FILTER  3U
#define TEMP_TRIP_CENTI   9000
#define SHDN_GPIO_Port    GPIOB
#define SHDN_Pin          GPIO_PIN_3
#define SHDN_SOFTSTART_ENABLE      1U
#define SHDN_SOFTSTART_STEPS       60U
#define SHDN_SOFTSTART_PERIOD_US   2000U
#define SHDN_SOFTSTART_PRIME_US    500U
#define RST_BTN_GPIO_Port GPIOA
#define RST_BTN_Pin       GPIO_PIN_5
#define PROTECT_NONE      0U
#define PROTECT_LV_OV     1U
#define PROTECT_REVERSE   2U
#define PROTECT_TEMP      3U
#define PROTECT_OC        4U
#define PROTECT_LOW_V     5U
#define PROTECT_POWER     6U

static uint8_t lcd_addr = LCD_DEFAULT_ADDR;
static uint8_t ads_addr = ADS1115_DEFAULT_ADDR;
static uint8_t us_timer_ready = 0U;

static uint32_t irq_save(void)
{
  uint32_t primask = __get_PRIMASK();
  __disable_irq();
  return primask;
}

static void irq_restore(uint32_t primask)
{
  if (primask == 0U)
  {
    __enable_irq();
  }
}

static void delay_us_init(void)
{
  if (us_timer_ready != 0U)
  {
    return;
  }

  RCC->APBENR2 |= RCC_APBENR2_TIM16EN;
  TIM16->CR1 = 0U;
  TIM16->PSC = (uint16_t)((SystemCoreClock / 1000000U) - 1U);
  TIM16->ARR = 0xFFFFU;
  TIM16->EGR = TIM_EGR_UG;
  TIM16->CNT = 0U;
  TIM16->CR1 = TIM_CR1_CEN;
  us_timer_ready = 1U;
}

static void delay_us(uint32_t us)
{
  uint16_t start;

  if (us == 0U)
  {
    return;
  }

  if (us_timer_ready == 0U)
  {
    delay_us_init();
  }

  start = (uint16_t)TIM16->CNT;
  while ((uint16_t)(TIM16->CNT - start) < (uint16_t)us)
  {
  }
}

static void onewire_drive_low(void)
{
  HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_RESET);
}

static void onewire_release(void)
{
  HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_SET);
}

static uint8_t onewire_read_pin(void)
{
  return (HAL_GPIO_ReadPin(DS_GPIO_Port, DS_Pin) == GPIO_PIN_SET) ? 1U : 0U;
}

static uint8_t onewire_reset_pulse(void)
{
  uint32_t irq_state;
  uint8_t presence_low;
  uint8_t line_recovered_high;

  onewire_release();
  delay_us(5U);
  if (onewire_read_pin() == 0U)
  {
    return 0U;
  }

  irq_state = irq_save();
  onewire_drive_low();
  delay_us(500U);
  onewire_release();
  delay_us(70U);
  presence_low = (onewire_read_pin() == 0U) ? 1U : 0U;
  delay_us(430U);
  line_recovered_high = (onewire_read_pin() != 0U) ? 1U : 0U;
  irq_restore(irq_state);

  return (uint8_t)((presence_low != 0U) && (line_recovered_high != 0U));
}

static void onewire_write_bit(uint8_t bit)
{
  uint32_t irq_state = irq_save();
  onewire_drive_low();
  if (bit != 0U)
  {
    delay_us(6U);
    onewire_release();
    delay_us(64U);
  }
  else
  {
    delay_us(60U);
    onewire_release();
    delay_us(10U);
  }
  irq_restore(irq_state);
}

static uint8_t onewire_read_bit(void)
{
  uint32_t irq_state = irq_save();
  uint8_t bit;

  onewire_drive_low();
  delay_us(6U);
  onewire_release();
  delay_us(9U);
  bit = onewire_read_pin();
  delay_us(55U);
  irq_restore(irq_state);
  return bit;
}

static void onewire_write_byte(uint8_t data)
{
  uint8_t i;
  for (i = 0U; i < 8U; i++)
  {
    onewire_write_bit((uint8_t)(data & 0x01U));
    data >>= 1;
  }
}

static uint8_t onewire_read_byte(void)
{
  uint8_t i;
  uint8_t data = 0U;

  for (i = 0U; i < 8U; i++)
  {
    data >>= 1;
    if (onewire_read_bit() != 0U)
    {
      data |= 0x80U;
    }
  }
  return data;
}

static uint8_t ds18b20_start_conversion(void)
{
  uint8_t try_idx;

  for (try_idx = 0U; try_idx < DS18B20_RETRY_COUNT; try_idx++)
  {
    if (onewire_reset_pulse() != 0U)
    {
      onewire_write_byte(0xCCU); /* Skip ROM */
      onewire_write_byte(0x44U); /* Convert T */
      return 1U;
    }
    HAL_Delay(2U);
  }

  return 0U;
}

static uint8_t ds18b20_crc8(const uint8_t *data, uint8_t len)
{
  uint8_t crc = 0U;
  uint8_t i;
  uint8_t j;

  for (i = 0U; i < len; i++)
  {
    crc ^= data[i];
    for (j = 0U; j < 8U; j++)
    {
      if ((crc & 0x01U) != 0U)
      {
        crc = (uint8_t)((crc >> 1U) ^ 0x8CU);
      }
      else
      {
        crc >>= 1U;
      }
    }
  }

  return crc;
}

static uint8_t ds18b20_read_temp_centi(int16_t *temp_centi)
{
  uint8_t sp[9];
  uint8_t try_idx;
  uint8_t i;
  uint8_t all_zero;
  uint8_t all_ff;
  int16_t raw;

  for (try_idx = 0U; try_idx < DS18B20_RETRY_COUNT; try_idx++)
  {
    all_zero = 1U;
    all_ff = 1U;

    if (onewire_reset_pulse() == 0U)
    {
      HAL_Delay(1U);
      continue;
    }

    onewire_write_byte(0xCCU); /* Skip ROM */
    onewire_write_byte(0xBEU); /* Read scratchpad */

    for (i = 0U; i < 9U; i++)
    {
      sp[i] = onewire_read_byte();
      if (sp[i] != 0x00U)
      {
        all_zero = 0U;
      }
      if (sp[i] != 0xFFU)
      {
        all_ff = 0U;
      }
    }

    if ((all_zero != 0U) || (all_ff != 0U))
    {
      HAL_Delay(1U);
      continue;
    }

    if (ds18b20_crc8(sp, 8U) != sp[8])
    {
      HAL_Delay(1U);
      continue;
    }

    raw = (int16_t)(((uint16_t)sp[1] << 8) | sp[0]);
    *temp_centi = (int16_t)((raw * 100) / 16);

    if ((*temp_centi < -5500) || (*temp_centi > 12500))
    {
      HAL_Delay(1U);
      continue;
    }

    return 1U;
  }

  return 0U;
}

static void lcd_delay_short(void)
{
  volatile uint32_t wait = 120U;
  while (wait-- > 0U)
  {
    __NOP();
  }
}

static void lcd_expander_write(uint8_t data)
{
  (void)HAL_I2C_Master_Transmit(&hi2c2, lcd_addr, &data, 1U, LCD_I2C_TIMEOUT);
}

static void lcd_write4(uint8_t nibble, uint8_t mode)
{
  uint8_t data = LCD_BL_BIT | mode | ((uint8_t)(nibble & 0x0FU) << 4);

  lcd_expander_write((uint8_t)(data | LCD_EN_BIT));
  lcd_delay_short();
  lcd_expander_write((uint8_t)(data & (uint8_t)~LCD_EN_BIT));
  lcd_delay_short();
}

static void lcd_send(uint8_t value, uint8_t mode)
{
  lcd_write4((uint8_t)(value >> 4), mode);
  lcd_write4((uint8_t)(value & 0x0FU), mode);
  lcd_delay_short();
}

static void lcd_cmd(uint8_t cmd)
{
  lcd_send(cmd, LCD_CMD_MODE);
}

static void lcd_data(uint8_t data)
{
  lcd_send(data, LCD_DATA_MODE);
}

static uint8_t lcd_detect_address(void)
{
  uint8_t addr;

  for (addr = 0x20U; addr <= 0x27U; addr++)
  {
    if (HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(addr << 1), 2U, LCD_I2C_TIMEOUT) == HAL_OK)
    {
      return (uint8_t)(addr << 1);
    }
  }

  for (addr = 0x38U; addr <= 0x3FU; addr++)
  {
    if (HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(addr << 1), 2U, LCD_I2C_TIMEOUT) == HAL_OK)
    {
      return (uint8_t)(addr << 1);
    }
  }

  return LCD_DEFAULT_ADDR;
}

static void lcd_init(void)
{
  HAL_Delay(50U);
  lcd_write4(0x03U, LCD_CMD_MODE);
  HAL_Delay(5U);
  lcd_write4(0x03U, LCD_CMD_MODE);
  HAL_Delay(5U);
  lcd_write4(0x03U, LCD_CMD_MODE);
  HAL_Delay(1U);
  lcd_write4(0x02U, LCD_CMD_MODE);
  HAL_Delay(1U);

  lcd_cmd(0x28U); /* 4-bit, 2-line mode */
  lcd_cmd(0x08U); /* Display off */
  lcd_cmd(0x01U); /* Clear */
  HAL_Delay(2U);
  lcd_cmd(0x06U); /* Entry mode */
  lcd_cmd(0x0CU); /* Display on, cursor off */
}

static void lcd_set_cursor(uint8_t row, uint8_t col)
{
  static const uint8_t row_offsets[4] = {0x00U, 0x40U, 0x14U, 0x54U};

  if (row > 3U)
  {
    row = 3U;
  }
  if (col > 19U)
  {
    col = 19U;
  }

  lcd_cmd((uint8_t)(0x80U + row_offsets[row] + col));
}

static void lcd_print(const char *text)
{
  while ((*text) != '\0')
  {
    lcd_data((uint8_t)(*text));
    text++;
  }
}

static void lcd_print_centered(uint8_t row, const char *text)
{
  char out[21];
  uint8_t len = 0U;
  uint8_t start;
  uint8_t i;

  if (text == 0)
  {
    text = "";
  }

  while ((text[len] != '\0') && (len < 20U))
  {
    len++;
  }

  for (i = 0U; i < 20U; i++)
  {
    out[i] = ' ';
  }
  out[20] = '\0';
  start = (uint8_t)((20U - len) / 2U);
  for (i = 0U; i < len; i++)
  {
    out[start + i] = text[i];
  }

  lcd_set_cursor(row, 0U);
  lcd_print(out);
}

static void lcd_startup_loading_until_ds_ready(void)
{
  uint8_t step;
  uint8_t bar_fill;
  uint8_t i;
  char bar[21];

  lcd_cmd(0x01U);
  HAL_Delay(2U);
  lcd_print_centered(0U, "ELECTRONIC LOAD V1.0");
  lcd_print_centered(1U, "SYSTEM STARTING");
  lcd_set_cursor(3U, 0U);
  lcd_print("                    ");

  /* 20 adim x 100ms = 2000ms */
  for (step = 0U; step < 20U; step++)
  {
    bar_fill = (uint8_t)(step + 1U);
    for (i = 0U; i < 20U; i++)
    {
      bar[i] = (i < bar_fill) ? (char)0xFF : ' ';
    }
    bar[20] = '\0';
    lcd_set_cursor(2U, 0U);
    lcd_print(bar);
    HAL_Delay(100U);
  }

  buzzer_pattern_startup();

  lcd_cmd(0x01U);
  HAL_Delay(2U);
}

static void shdn_softstart(void)
{
#if (SHDN_SOFTSTART_ENABLE != 0U)
  uint8_t step;
  uint32_t on_us;
  uint32_t off_us;

  /* Start from fully disabled load path. */
  HAL_GPIO_WritePin(SHDN_GPIO_Port, SHDN_Pin, GPIO_PIN_SET);
  delay_us(SHDN_SOFTSTART_PRIME_US);

  for (step = 1U; step <= SHDN_SOFTSTART_STEPS; step++)
  {
    on_us = ((uint32_t)SHDN_SOFTSTART_PERIOD_US * (uint32_t)step) / (uint32_t)SHDN_SOFTSTART_STEPS;
    off_us = (uint32_t)SHDN_SOFTSTART_PERIOD_US - on_us;

    if (on_us > 0U)
    {
      HAL_GPIO_WritePin(SHDN_GPIO_Port, SHDN_Pin, GPIO_PIN_RESET);
      delay_us(on_us);
    }
    if (off_us > 0U)
    {
      HAL_GPIO_WritePin(SHDN_GPIO_Port, SHDN_Pin, GPIO_PIN_SET);
      delay_us(off_us);
    }
  }

  HAL_GPIO_WritePin(SHDN_GPIO_Port, SHDN_Pin, GPIO_PIN_RESET);
#else
  HAL_GPIO_WritePin(SHDN_GPIO_Port, SHDN_Pin, GPIO_PIN_RESET);
#endif
}

static void buzzer_tone(uint16_t freq_hz, uint16_t duration_ms, uint16_t duty_permille)
{
  uint32_t period_us;
  uint32_t on_us;
  uint32_t off_us;
  uint32_t cycles;
  uint32_t i;

  if ((freq_hz == 0U) || (duration_ms == 0U))
  {
    return;
  }

  period_us = 1000000UL / (uint32_t)freq_hz;
  if (period_us < 2UL)
  {
    period_us = 2UL;
  }

  if (duty_permille > 900U)
  {
    duty_permille = 900U;
  }

  on_us = (period_us * (uint32_t)duty_permille) / 1000UL;
  if (on_us == 0U)
  {
    on_us = 1U;
  }
  if (on_us >= period_us)
  {
    on_us = period_us - 1UL;
  }
  off_us = period_us - on_us;
  cycles = (((uint32_t)duration_ms * 1000UL) + (period_us / 2UL)) / period_us;

  for (i = 0U; i < cycles; i++)
  {
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
    delay_us(on_us);
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
    delay_us(off_us);
  }

  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
}

static void buzzer_pattern_startup(void)
{
  buzzer_tone(1200U, 70U, BUZZER_DUTY_PERMILLE);
  HAL_Delay(25U);
  buzzer_tone(1700U, 90U, BUZZER_DUTY_PERMILLE);
}

static void buzzer_pattern_halt(void)
{
  buzzer_tone(1900U, 55U, BUZZER_DUTY_PERMILLE);
  HAL_Delay(25U);
  buzzer_tone(1400U, 70U, BUZZER_DUTY_PERMILLE);
  HAL_Delay(25U);
  buzzer_tone(1000U, 90U, BUZZER_DUTY_PERMILLE);
}

static void buzzer_pattern_system_on(void)
{
  buzzer_tone(1700U, 65U, BUZZER_DUTY_PERMILLE);
}

static void buzzer_pattern_system_off(void)
{
  buzzer_tone(900U, 80U, BUZZER_DUTY_PERMILLE);
}

static uint8_t adc_read_channel(uint32_t channel, uint16_t *raw_value)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  sConfig.Channel = channel;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    return 0U;
  }

  /* Dummy conversion after channel switch for settling */
  if (HAL_ADC_Start(&hadc1) != HAL_OK)
  {
    return 0U;
  }
  if (HAL_ADC_PollForConversion(&hadc1, 10U) != HAL_OK)
  {
    (void)HAL_ADC_Stop(&hadc1);
    return 0U;
  }
  (void)HAL_ADC_GetValue(&hadc1);
  (void)HAL_ADC_Stop(&hadc1);

  if (HAL_ADC_Start(&hadc1) != HAL_OK)
  {
    return 0U;
  }

  if (HAL_ADC_PollForConversion(&hadc1, 10U) != HAL_OK)
  {
    (void)HAL_ADC_Stop(&hadc1);
    return 0U;
  }

  *raw_value = (uint16_t)HAL_ADC_GetValue(&hadc1);
  (void)HAL_ADC_Stop(&hadc1);
  return 1U;
}

static uint8_t adc_read_channel_avg(uint32_t channel, uint8_t samples, uint16_t *raw_avg)
{
  uint8_t i;
  uint16_t raw = 0U;
  uint32_t sum = 0U;

  if ((samples == 0U) || (raw_avg == NULL))
  {
    return 0U;
  }

  for (i = 0U; i < samples; i++)
  {
    if (adc_read_channel(channel, &raw) == 0U)
    {
      return 0U;
    }
    sum += raw;
  }

  *raw_avg = (uint16_t)(sum / (uint32_t)samples);
  return 1U;
}

static uint32_t adc_raw_to_millivolt(uint16_t raw)
{
  return ((uint32_t)raw * ADC_VREF_MV) / ADC_MAX;
}

static uint32_t lv_adc_to_input_millivolt(uint16_t raw)
{
  uint32_t adc_mv = adc_raw_to_millivolt(raw);
  return ((adc_mv * (LV_DIV_TOP_OHM + LV_DIV_BOT_OHM)) + (LV_DIV_BOT_OHM / 2UL)) / LV_DIV_BOT_OHM;
}

static uint8_t ads1115_detect_address(void)
{
  uint8_t addr;

  for (addr = 0x48U; addr <= 0x4BU; addr++)
  {
    if (HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(addr << 1), 2U, LCD_I2C_TIMEOUT) == HAL_OK)
    {
      return (uint8_t)(addr << 1);
    }
  }

  return ADS1115_DEFAULT_ADDR;
}

static uint8_t ads1115_read_channel_raw_cfg(uint8_t channel, uint16_t pga_cfg, int16_t *raw_value)
{
  uint8_t tx[3];
  uint8_t rx[2];
  uint16_t config;
  uint16_t cfg_read = 0U;
  uint8_t i;

  if (channel > 3U)
  {
    return 0U;
  }

  config = (uint16_t)(ADS1115_CFG_OS_SINGLE |
                      ((uint16_t)(0x04U + channel) << 12) |
                      pga_cfg |
                      ADS1115_CFG_MODE_SINGLE |
                      ADS1115_CFG_DR_128SPS |
                      ADS1115_CFG_COMP_DISABLE);

  tx[0] = ADS1115_REG_CONFIG;
  tx[1] = (uint8_t)(config >> 8);
  tx[2] = (uint8_t)(config & 0xFFU);
  if (HAL_I2C_Master_Transmit(&hi2c2, ads_addr, tx, 3U, 20U) != HAL_OK)
  {
    return 0U;
  }

  for (i = 0U; i < 20U; i++)
  {
    tx[0] = ADS1115_REG_CONFIG;
    if (HAL_I2C_Master_Transmit(&hi2c2, ads_addr, tx, 1U, 20U) != HAL_OK)
    {
      return 0U;
    }
    if (HAL_I2C_Master_Receive(&hi2c2, ads_addr, rx, 2U, 20U) != HAL_OK)
    {
      return 0U;
    }
    cfg_read = (uint16_t)(((uint16_t)rx[0] << 8) | rx[1]);
    if ((cfg_read & 0x8000U) != 0U)
    {
      break;
    }
    HAL_Delay(1U);
  }

  tx[0] = ADS1115_REG_CONV;
  if (HAL_I2C_Master_Transmit(&hi2c2, ads_addr, tx, 1U, 20U) != HAL_OK)
  {
    return 0U;
  }
  if (HAL_I2C_Master_Receive(&hi2c2, ads_addr, rx, 2U, 20U) != HAL_OK)
  {
    return 0U;
  }

  *raw_value = (int16_t)(((uint16_t)rx[0] << 8) | rx[1]);
  return 1U;
}

static uint8_t ads1115_read_avg_microvolt_cfg(uint8_t channel, uint16_t pga_cfg, int32_t fs_uv, uint32_t *avg_uv)
{
  int16_t raw = 0;
  int32_t sum = 0;
  uint8_t n;
  int32_t avg_raw;

  for (n = 0U; n < ADS1115_SAMPLES_PER_CH; n++)
  {
    if (ads1115_read_channel_raw_cfg(channel, pga_cfg, &raw) == 0U)
    {
      return 0U;
    }

    if (raw < 0)
    {
      raw = 0;
    }
    sum += raw;
  }

  avg_raw = sum / (int32_t)ADS1115_SAMPLES_PER_CH;
  *avg_uv = (uint32_t)(((uint64_t)avg_raw * (uint64_t)fs_uv) / 32767ULL);
  return 1U;
}

static uint8_t ads1115_read_avg_microvolt(uint32_t *avg_uv)
{
  return ads1115_read_avg_microvolt_cfg(CURRENT_ADS_CHANNEL, ADS1115_CFG_PGA_512MV, ADS1115_FS_UV_CURRENT, avg_uv);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
  lcd_addr = lcd_detect_address();
  lcd_init();
  lcd_cmd(0x01U);
  HAL_Delay(2U);
  ads_addr = ads1115_detect_address();
  delay_us_init();
  HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_SET);
  lcd_startup_loading_until_ds_ready();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    static uint8_t protect_latched = 0U;
    static uint8_t protect_reason = PROTECT_NONE;
    static uint32_t protect_trip_value = 0U;
    static uint8_t reset_btn_prev_above = 0U;
    static uint8_t ds_state = 0U;
    static uint32_t ds_tick_ms = 0U;
    static int16_t temp_centi = 0;
    static uint8_t temp_valid = 0U;
    static uint8_t ds_fail_count = 0U;
    static uint8_t fan_on = 0U;
    static uint8_t oc_over_count = 0U;
    static uint8_t power_over_count = 0U;
    static uint8_t startup_lock = 1U;
    static uint8_t system_off = 0U;
    static uint8_t protect_latched_prev = 0U;
    static uint32_t system_on_tick_ms = 0U;
    uint16_t raw_reset = 0U;
    uint16_t raw_rev = 0U;
    uint16_t raw_lv = 0U;
    uint16_t raw_unprot = 0U;
    uint16_t raw_pot = 0U;
    uint16_t raw_set_v = 0U;
    uint16_t raw_set_a = 0U;
    uint32_t reset_mv = 0U;
    uint32_t rev_mv = 0U;
    uint32_t lv_in_mv;
    uint32_t set_v_mv;
    uint32_t set_a_ma;
    uint32_t low_v_trip_mv;
    uint32_t current_trip_ma;
    uint32_t pot_scaled;
    uint32_t iset_percent;
    uint32_t unprot_in_mv = 0U;
    uint32_t act_v_mv = 0U;
    uint32_t ads_avg_uv = 0U;
    uint32_t current_ma = 0U;
    uint32_t power_mw = 0U;
    uint8_t ads_ok = 0U;
    uint8_t unprot_ok = 0U;
    uint8_t prot_present = 0U;
    uint8_t unprot_present = 0U;
    uint8_t input_present = 0U;
    uint8_t source_protected = 1U;
    uint8_t set_pots_zero = 0U;
    const char *system_text;
    uint8_t reset_btn_above = 0U;
    uint8_t reset_edge = 0U;
    uint8_t run_softstart = 0U;
    uint32_t now_ms;
    int16_t t_abs;
    char line[21];

    /* Reset button by ADC threshold: 1.5V ustu = basildi */
    if (adc_read_channel(ADC_CHANNEL_5, &raw_reset) != 0U)
    {
      reset_mv = adc_raw_to_millivolt(raw_reset);
      reset_btn_above = (reset_mv >= RESET_ADC_TRIP_MV) ? 1U : 0U;
      if ((reset_btn_prev_above == 0U) && (reset_btn_above != 0U))
      {
        HAL_Delay(20U);
        if (adc_read_channel(ADC_CHANNEL_5, &raw_reset) != 0U)
        {
          reset_mv = adc_raw_to_millivolt(raw_reset);
          if (reset_mv >= RESET_ADC_TRIP_MV)
          {
            reset_edge = 1U;
          }
        }
      }
      reset_btn_prev_above = reset_btn_above;
    }
    now_ms = HAL_GetTick();

    if (ds_state == 0U)
    {
      if (ds18b20_start_conversion() != 0U)
      {
        ds_state = 1U;
        ds_tick_ms = now_ms;
      }
      else
      {
        if (ds_fail_count < 255U)
        {
          ds_fail_count++;
        }
        if (ds_fail_count >= DS18B20_FAIL_FILTER)
        {
          temp_valid = 0U;
        }
      }
    }
    else if ((now_ms - ds_tick_ms) >= DS18B20_CONV_WAIT_MS)
    {
      if (ds18b20_read_temp_centi(&temp_centi) != 0U)
      {
        temp_valid = 1U;
        ds_fail_count = 0U;
      }
      else
      {
        if (ds_fail_count < 255U)
        {
          ds_fail_count++;
        }
        if (ds_fail_count >= DS18B20_FAIL_FILTER)
        {
          temp_valid = 0U;
        }
      }
      ds_state = 0U;
    }

    if ((adc_read_channel(ADC_CHANNEL_0, &raw_rev) == 0U) ||
        (adc_read_channel_avg(ADC_CHANNEL_1, 4U, &raw_lv) == 0U) ||
        (adc_read_channel_avg(ADC_CHANNEL_11, 4U, &raw_unprot) == 0U) ||
        (adc_read_channel(ADC_CHANNEL_2, &raw_pot) == 0U) ||
        (adc_read_channel(ADC_CHANNEL_3, &raw_set_v) == 0U) ||
        (adc_read_channel(ADC_CHANNEL_4, &raw_set_a) == 0U))
    {
      lcd_set_cursor(0U, 0U);
      lcd_print("ADC READ ERROR      ");
      HAL_Delay(20U);
      continue;
    }

    lv_in_mv = lv_adc_to_input_millivolt(raw_lv);
    if (lv_in_mv > LV_OFFSET_MV)
    {
      lv_in_mv -= LV_OFFSET_MV;
    }
    else
    {
      lv_in_mv = 0U;
    }
    rev_mv = adc_raw_to_millivolt(raw_rev);
    set_v_mv = (((uint32_t)raw_set_v * SET_V_MAX_MV) + (ADC_MAX / 2UL)) / ADC_MAX;
    set_a_ma = (((uint32_t)raw_set_a * SET_A_MAX_MA) + (ADC_MAX / 2UL)) / ADC_MAX;
    if (set_v_mv <= SET_V_ZERO_DEADBAND_MV)
    {
      set_v_mv = 0U;
    }
    else
    {
      set_v_mv = ((set_v_mv - SET_V_ZERO_DEADBAND_MV) * SET_V_MAX_MV + ((SET_V_MAX_MV - SET_V_ZERO_DEADBAND_MV) / 2UL))
                 / (SET_V_MAX_MV - SET_V_ZERO_DEADBAND_MV);
    }
    if (set_a_ma <= SET_A_ZERO_DEADBAND_MA)
    {
      set_a_ma = 0U;
    }
    else
    {
      set_a_ma = ((set_a_ma - SET_A_ZERO_DEADBAND_MA) * SET_A_MAX_MA + ((SET_A_MAX_MA - SET_A_ZERO_DEADBAND_MA) / 2UL))
                 / (SET_A_MAX_MA - SET_A_ZERO_DEADBAND_MA);
    }
    pot_scaled = (((uint32_t)raw_pot * 1000UL) + (ADC_MAX / 2UL)) / ADC_MAX;
    if (pot_scaled <= ISET_SCALED_MIN)
    {
      iset_percent = 0U;
    }
    else if (pot_scaled >= ISET_SCALED_MAX)
    {
      iset_percent = ISET_MAX_PERCENT;
    }
    else
    {
      iset_percent = ((pot_scaled - ISET_SCALED_MIN) * ISET_MAX_PERCENT + ((ISET_SCALED_MAX - ISET_SCALED_MIN) / 2UL))
                     / (ISET_SCALED_MAX - ISET_SCALED_MIN);
    }
    if (iset_percent <= ISET_ZERO_DEADBAND_PERCENT)
    {
      iset_percent = 0U;
    }

    unprot_in_mv = lv_adc_to_input_millivolt(raw_unprot);
    unprot_ok = 1U;
    prot_present = (lv_in_mv >= PROT_PRESENT_THRESHOLD_MV) ? 1U : 0U;
    unprot_present = ((unprot_ok != 0U) && (unprot_in_mv >= UNPROT_PRESENT_THRESHOLD_MV)) ? 1U : 0U;
    input_present = (uint8_t)(((prot_present != 0U) || (unprot_present != 0U)) ? 1U : 0U);
    if ((prot_present != 0U) && (unprot_present != 0U))
    {
      source_protected = 1U;
    }
    else if ((prot_present == 0U) && (unprot_present != 0U))
    {
      source_protected = 0U;
    }
    else if (prot_present != 0U)
    {
      source_protected = 1U;
    }
    else
    {
      source_protected = 0U;
    }
    if ((prot_present == 0U) && (unprot_present == 0U))
    {
      act_v_mv = 0U;
    }
    else
    {
      act_v_mv = (source_protected != 0U) ? lv_in_mv : unprot_in_mv;
    }

    low_v_trip_mv = set_v_mv;
    if ((source_protected != 0U) && (low_v_trip_mv < PROT_MIN_MV))
    {
      /* Protected input must stay above hardware safe minimum. */
      low_v_trip_mv = PROT_MIN_MV;
    }
    current_trip_ma = set_a_ma;
    if (current_trip_ma > CURRENT_TRIP_MA)
    {
      current_trip_ma = CURRENT_TRIP_MA;
    }
    set_pots_zero = (uint8_t)(((iset_percent == 0U) && (set_v_mv == 0U) && (set_a_ma == 0U)) ? 1U : 0U);

    if (startup_lock != 0U)
    {
      HAL_GPIO_WritePin(SHDN_GPIO_Port, SHDN_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_RESET);
      if ((set_pots_zero != 0U) && (reset_edge != 0U))
      {
        startup_lock = 0U;
        protect_latched = 0U;
        protect_reason = PROTECT_NONE;
        protect_trip_value = 0U;
        oc_over_count = 0U;
        power_over_count = 0U;
        system_off = 0U;
        system_on_tick_ms = now_ms;
        run_softstart = 1U;
        buzzer_pattern_system_on();
        reset_edge = 0U;
      }
      else
      {
        lcd_set_cursor(0U, 0U);
        lcd_print("STARTUP LOCK        ");
        if (set_pots_zero != 0U)
        {
          lcd_set_cursor(1U, 0U);
          lcd_print("PRESS RESET TO RUN  ");
        }
        else
        {
          lcd_set_cursor(1U, 0U);
          lcd_print("SET POTS TO ZERO    ");
        }
        snprintf(line, sizeof(line), "ISET:%3lu V:%2lu.%01lu   ",
                 iset_percent,
                 set_v_mv / 1000UL,
                 (set_v_mv % 1000UL) / 100UL);
        lcd_set_cursor(2U, 0U);
        lcd_print(line);
        snprintf(line, sizeof(line), "A:%2lu.%02lu WAIT RESET  ",
                 set_a_ma / 1000UL,
                 (set_a_ma % 1000UL) / 10UL);
        lcd_set_cursor(3U, 0U);
        lcd_print(line);
        HAL_Delay(80U);
        continue;
      }
    }

    if (reset_edge != 0U)
    {
      if (protect_latched != 0U)
      {
        protect_latched = 0U;
        protect_reason = PROTECT_NONE;
        protect_trip_value = 0U;
        oc_over_count = 0U;
        power_over_count = 0U;
        system_off = 0U;
        system_on_tick_ms = now_ms;
        run_softstart = 1U;
        buzzer_pattern_system_on();
      }
      else
      {
        if (system_off != 0U)
        {
          system_off = 0U;
          system_on_tick_ms = now_ms;
          run_softstart = 1U;
          buzzer_pattern_system_on();
        }
        else
        {
          system_off = 1U;
          power_over_count = 0U;
          buzzer_pattern_system_off();
        }
      }
    }

    if ((system_off == 0U) && (protect_latched == 0U) && (rev_mv <= RV_REVERSE_TRIP_MV))
    {
      /* Reverse detection uses only PA0 optocoupler voltage. */
      protect_latched = 1U;
      protect_reason = PROTECT_REVERSE;
      protect_trip_value = rev_mv;
      HAL_GPIO_WritePin(SHDN_GPIO_Port, SHDN_Pin, GPIO_PIN_SET);
    }

    if ((run_softstart != 0U) && (system_off == 0U) && (protect_latched == 0U))
    {
      shdn_softstart();
    }

    if ((system_off == 0U) && (protect_latched == 0U))
    {
      if ((input_present != 0U) && (act_v_mv < low_v_trip_mv))
      {
        protect_latched = 1U;
        protect_reason = PROTECT_LOW_V;
        protect_trip_value = act_v_mv;
      }
      else if (act_v_mv > LV_PROTECT_MV)
      {
        protect_latched = 1U;
        protect_reason = PROTECT_LV_OV;
        protect_trip_value = act_v_mv;
      }
    }

    if (protect_latched == 0U)
    {
      ads_ok = ads1115_read_avg_microvolt(&ads_avg_uv);
      if (ads_ok != 0U)
      {
        current_ma = (uint32_t)(((ads_avg_uv * 1000UL) + (CURRENT_UV_PER_A / 2UL)) / CURRENT_UV_PER_A);
        if ((system_off == 0U) && (protect_latched == 0U) && (input_present != 0U) && (current_ma > current_trip_ma))
        {
          if (oc_over_count < 255U)
          {
            oc_over_count++;
          }
          if (oc_over_count >= OCP_CONFIRM_COUNT)
          {
            protect_latched = 1U;
            protect_reason = PROTECT_OC;
            protect_trip_value = current_ma;
          }
        }
        else
        {
          oc_over_count = 0U;
        }
      }
      else
      {
        current_ma = 0U;
        oc_over_count = 0U;
      }
    }
    else
    {
      ads_ok = 0U;
      current_ma = 0U;
      oc_over_count = 0U;
    }
    power_mw = (act_v_mv * current_ma) / 1000UL;

    if ((system_off == 0U) && (protect_latched == 0U) &&
        (input_present != 0U) && (ads_ok != 0U) &&
        ((now_ms - system_on_tick_ms) >= POWER_CHECK_DELAY_MS) &&
        (power_mw > POWER_TRIP_MW))
    {
      if (power_over_count < 255U)
      {
        power_over_count++;
      }
      if (power_over_count >= POWER_CONFIRM_COUNT)
      {
        protect_latched = 1U;
        protect_reason = PROTECT_POWER;
        protect_trip_value = power_mw;
      }
    }
    else
    {
      power_over_count = 0U;
    }

    if (temp_valid != 0U)
    {
      if ((fan_on == 0U) && (temp_centi >= TEMP_FAN_ON_CENTI))
      {
        fan_on = 1U;
      }
      else if ((fan_on != 0U) && (temp_centi <= TEMP_FAN_OFF_CENTI))
      {
        fan_on = 0U;
      }
    }

    if ((system_off == 0U) && (temp_valid != 0U) && (protect_latched == 0U) && (temp_centi >= TEMP_TRIP_CENTI))
    {
      protect_latched = 1U;
      protect_reason = PROTECT_TEMP;
      protect_trip_value = (uint32_t)temp_centi;
    }

    if ((protect_latched != 0U) && (protect_latched_prev == 0U))
    {
      buzzer_pattern_halt();
    }
    protect_latched_prev = protect_latched;

    HAL_GPIO_WritePin(SHDN_GPIO_Port, SHDN_Pin,
                      ((protect_latched != 0U) || (system_off != 0U)) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, (fan_on != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    if (protect_latched != 0U)
    {
      lcd_set_cursor(0U, 0U);
      lcd_print("                    ");
      lcd_print_centered(1U, "SYSTEM HALTED");
      if (protect_reason == PROTECT_LV_OV)
      {
        snprintf(line, sizeof(line), "HIGH VOLTAGE %2lu.%01luV",
                 protect_trip_value / 1000UL,
                 (protect_trip_value % 1000UL) / 100UL);
      }
      else if (protect_reason == PROTECT_OC)
      {
        snprintf(line, sizeof(line), "OVER CURRENT %2lu.%02luA",
                 protect_trip_value / 1000UL,
                 (protect_trip_value % 1000UL) / 10UL);
      }
      else if (protect_reason == PROTECT_TEMP)
      {
        snprintf(line, sizeof(line), "HIGH TEMP %2lu.%01luC",
                 protect_trip_value / 100UL,
                 (protect_trip_value % 100UL) / 10UL);
      }
      else if (protect_reason == PROTECT_REVERSE)
      {
        snprintf(line, sizeof(line), "REVERSE VOLTAGE");
      }
      else if (protect_reason == PROTECT_LOW_V)
      {
        snprintf(line, sizeof(line), "LOW VOLTAGE %2lu.%01luV",
                 protect_trip_value / 1000UL,
                 (protect_trip_value % 1000UL) / 100UL);
      }
      else if (protect_reason == PROTECT_POWER)
      {
        snprintf(line, sizeof(line), "OVER POWER %3lu.%01luW",
                 protect_trip_value / 1000UL,
                 (protect_trip_value % 1000UL) / 100UL);
      }
      else
      {
        snprintf(line, sizeof(line), "PROTECTION ACTIVE");
      }
      lcd_print_centered(2U, line);
      lcd_set_cursor(3U, 0U);
      lcd_print("                    ");
      HAL_Delay(80U);
      continue;
    }

    snprintf(line, sizeof(line), "SET:%2lu.%01luV %2lu.%02luA   ",
             set_v_mv / 1000UL,
             (set_v_mv % 1000UL) / 100UL,
             set_a_ma / 1000UL,
             (set_a_ma % 1000UL) / 10UL);
    lcd_set_cursor(0U, 0U);
    lcd_print(line);

    if (ads_ok != 0U)
    {
      snprintf(line, sizeof(line), "ACT:%2lu.%01luV %2lu.%02luA   ",
               act_v_mv / 1000UL,
               (act_v_mv % 1000UL) / 100UL,
               current_ma / 1000UL,
               (current_ma % 1000UL) / 10UL);
    }
    else
    {
      snprintf(line, sizeof(line), "ACT:%2lu.%01luV  ---.--A   ",
               act_v_mv / 1000UL,
               (act_v_mv % 1000UL) / 100UL);
    }
    lcd_set_cursor(1U, 0U);
    lcd_print(line);

    snprintf(line, sizeof(line), "PWR:%3lu.%01luW ISET:%3lu ",
             power_mw / 1000UL,
             (power_mw % 1000UL) / 100UL,
             iset_percent);
    lcd_set_cursor(2U, 0U);
    lcd_print(line);

    system_text = (system_off != 0U) ? "OFF" : "ON";
    if (temp_centi < 0)
    {
      t_abs = (int16_t)(-temp_centi);
    }
    else
    {
      t_abs = temp_centi;
    }
    if (temp_valid != 0U)
    {
      snprintf(line, sizeof(line), "DS:%c%2d.%01dC SYSTEM:%-3s",
               (temp_centi < 0) ? '-' : '+',
               t_abs / 100,
               (t_abs % 100) / 10,
               system_text);
    }
    else
    {
      snprintf(line, sizeof(line), "DS:  SE   SYSTEM:%-3s", system_text);
    }
    lcd_set_cursor(3U, 0U);
    lcd_print(line);

    HAL_Delay(40U);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_79CYCLES_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_79CYCLES_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00303D5B;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
  /* Reset button ADC threshold read on PA5 */
  GPIO_InitStruct.Pin = RST_BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RST_BTN_GPIO_Port, &GPIO_InitStruct);

  /* Unprotected input voltage sense on PB7 (ADC1_IN11) */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* DS18B20 one-wire on PB0 (open-drain, released high) */
  GPIO_InitStruct.Pin = DS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DS_GPIO_Port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_SET);

  /* Buzzer on PA7 */
  GPIO_InitStruct.Pin = BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
