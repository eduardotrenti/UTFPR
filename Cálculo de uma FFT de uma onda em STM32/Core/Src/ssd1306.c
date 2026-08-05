#include "ssd1306.h"
#include "i2c.h"
#include "FreeRTOS.h"    /* FreeRTOS puro */
#include "semphr.h"      /* semaforo: TX I2C por interrupcao, sem polling */


volatile HAL_StatusTypeDef i2c_ret;
#define SSD1306_ADDR  (0x3CU << 1)

/* Semaforo binario: a ISR de fim de TX do I2C1 libera; a i2c_write
 * bloqueia (dorme) ate a transferencia terminar. Sem busy-wait. */
static SemaphoreHandle_t i2c_tx_sem = NULL;

/* Envia 'len' bytes ao display por interrupcao e dorme a task ate o
 * fim. Como so retorna quando a TX acaba, o buffer do chamador (mesmo
 * na pilha) continua valido durante a transferencia. Enquanto o
 * semaforo nao existe (ou se falhar ao iniciar), cai para o modo
 * bloqueante antigo como fallback seguro.                            */
static void i2c_write(uint8_t *buf, uint16_t len)
{
    if (i2c_tx_sem != NULL)
    {
        if (HAL_I2C_Master_Transmit_IT(&hi2c1, SSD1306_ADDR, buf, len)
            == HAL_OK)
        {
            /* Timeout (100 ms) evita travar a DisplayTask se a TX falhar. */
            (void)xSemaphoreTake(i2c_tx_sem, pdMS_TO_TICKS(100U));
        }
        else
        {
            /* periferico ocupado/erro: ignora este quadro */
        }
    }
    else
    {
        i2c_ret = HAL_I2C_Master_Transmit(&hi2c1, SSD1306_ADDR,
                                          buf, len, 50U);
    }
}

/* Fonte 6x8: apenas os caracteres usados ("FFT: 1250 Hz").
 * Ordem do indice na funcao get_font.                       */
static const uint8_t font_space[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
static const uint8_t font_colon[6] = {0x00,0x00,0x36,0x36,0x00,0x00};
static const uint8_t font_F[6]     = {0x7F,0x09,0x09,0x09,0x01,0x00};
static const uint8_t font_T[6]     = {0x01,0x01,0x7F,0x01,0x01,0x00};
static const uint8_t font_H[6]     = {0x7F,0x08,0x08,0x08,0x7F,0x00};
static const uint8_t font_z[6]     = {0x62,0x52,0x4A,0x46,0x42,0x00};
static const uint8_t font_dig[10][6] = {
    {0x3E,0x51,0x49,0x45,0x3E,0x00}, /* 0 */
    {0x00,0x42,0x7F,0x40,0x00,0x00}, /* 1 */
    {0x42,0x61,0x51,0x49,0x46,0x00}, /* 2 */
    {0x21,0x41,0x45,0x4B,0x31,0x00}, /* 3 */
    {0x18,0x14,0x12,0x7F,0x10,0x00}, /* 4 */
    {0x27,0x45,0x45,0x45,0x39,0x00}, /* 5 */
    {0x3C,0x4A,0x49,0x49,0x30,0x00}, /* 6 */
    {0x01,0x71,0x09,0x05,0x03,0x00}, /* 7 */
    {0x36,0x49,0x49,0x49,0x36,0x00}, /* 8 */
    {0x06,0x49,0x49,0x29,0x1E,0x00}  /* 9 */
};

static const uint8_t *get_font(char c)
{
    const uint8_t *f;

    if ((c >= '0') && (c <= '9'))
    {
        f = font_dig[(uint8_t)(c - '0')];
    }
    else if (c == 'F') { f = font_F; }
    else if (c == 'T') { f = font_T; }
    else if (c == 'H') { f = font_H; }
    else if (c == 'z') { f = font_z; }
    else if (c == ':') { f = font_colon; }
    else               { f = font_space; }

    return f;
}


static void ssd1306_cmd(uint8_t c)
{
	uint8_t buf[2];
	buf[0] = 0x00U;
	buf[1] = c;
	i2c_write(buf, 2U);
}

static void ssd1306_data(const uint8_t *data, uint16_t len)
{
    uint8_t buf[16];
    uint16_t i = 0U;

    buf[0] = 0x40U;
    for (i = 0U; i < len; i++)
    {
        buf[i + 1U] = data[i];
    }
    i2c_write(buf, (uint16_t)(len + 1U));
}

void SSD1306_Init(void)
{
    /* Cria o semaforo antes do 1o comando (binario, inicia vazio). */
    if (i2c_tx_sem == NULL)
    {
        i2c_tx_sem = xSemaphoreCreateBinary();
    }

    ssd1306_cmd(0xAEU);
    ssd1306_cmd(0xD5U); ssd1306_cmd(0x80U);
    ssd1306_cmd(0xA8U); ssd1306_cmd(0x3FU);
    ssd1306_cmd(0xD3U); ssd1306_cmd(0x00U);
    ssd1306_cmd(0x40U);
    ssd1306_cmd(0x8DU); ssd1306_cmd(0x14U);
    ssd1306_cmd(0x20U); ssd1306_cmd(0x00U);
    ssd1306_cmd(0xA1U);
    ssd1306_cmd(0xC8U);
    ssd1306_cmd(0xDAU); ssd1306_cmd(0x12U);
    ssd1306_cmd(0x81U); ssd1306_cmd(0x7FU);
    ssd1306_cmd(0xA4U);
    ssd1306_cmd(0xA6U);
    ssd1306_cmd(0xAFU);
}

void SSD1306_Clear(void)
{
    uint8_t page = 0U;
    uint8_t col  = 0U;
    uint8_t zero[8] = {0U,0U,0U,0U,0U,0U,0U,0U};

    for (page = 0U; page < 8U; page++)
    {
        ssd1306_cmd((uint8_t)(0xB0U + page));
        ssd1306_cmd(0x00U);
        ssd1306_cmd(0x10U);
        for (col = 0U; col < 16U; col++)
        {
            ssd1306_data(zero, 8U);
        }
    }
}

void SSD1306_WriteString(uint8_t page, uint8_t col, const char *str)
{
    uint16_t i = 0U;
    uint8_t  c = (uint8_t)(col);

    ssd1306_cmd((uint8_t)(0xB0U + page));
    ssd1306_cmd((uint8_t)(0x00U + (c & 0x0FU)));
    ssd1306_cmd((uint8_t)(0x10U + (c >> 4U)));

    while (str[i] != '\0')
    {
        ssd1306_data(get_font(str[i]), 6U);
        i++;
    }
}

/* Fim da transmissao I2C (master): libera a task que espera em i2c_write.
 * Contexto de ISR -> xSemaphoreGiveFromISR + portYIELD_FROM_ISR.       */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    BaseType_t higher_woken = pdFALSE;

    if (hi2c == &hi2c1)
    {
        (void)xSemaphoreGiveFromISR(i2c_tx_sem, &higher_woken);
        portYIELD_FROM_ISR(higher_woken);
    }
    else
    {
        /* outro I2C: ignora */
    }
}

/* Erro no I2C (ex.: NACK): tambem libera, para a DisplayTask nao travar.
 * O proximo quadro tenta de novo no ciclo seguinte de 500 ms.          */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    BaseType_t higher_woken = pdFALSE;

    if (hi2c == &hi2c1)
    {
        (void)xSemaphoreGiveFromISR(i2c_tx_sem, &higher_woken);
        portYIELD_FROM_ISR(higher_woken);
    }
    else
    {
        /* outro I2C: ignora */
    }
}
