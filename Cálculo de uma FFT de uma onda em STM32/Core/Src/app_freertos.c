/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "arm_math.h"   /* CMSIS-DSP: FFT */
#include "adc.h"        /* hadc1 */
#include "tim.h"        /* htim2 */
#include "dac.h"
extern uint16_t sine_table[100];
#include "usart.h"    /* hlpuart1 */
#include <string.h>   /* strcmp */
#include "ssd1306.h"
#include "stream_buffer.h"   /* buffer de fluxo para a impressao centralizada */
#include "timers.h"          /* software timers (FreeRTOS puro) */
#include "semphr.h"          /* semaforos e mutex (FreeRTOS puro) */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE BEGIN PD */
#define FFT_LENGTH         (512U)
#define ADC_BUFFER_LENGTH  (2U * FFT_LENGTH)
#define SAMPLE_RATE_HZ     (10000.0f)
#define SHELL_BUFFER_SIZE  (64U)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* Buffer do ADC, preenchido pelo DMA circular (ping-pong). */
static uint16_t adc_buffer[ADC_BUFFER_LENGTH] = { 0U };
/* Periodo do pisca do LED em ms (alterado pelo botao). */
static volatile uint32_t led_period_ms = 500U;

/* Buffers da FFT. */
static float32_t fft_input[FFT_LENGTH]          = { 0.0f };
static float32_t fft_output[FFT_LENGTH]         = { 0.0f };
static float32_t fft_magnitude[FFT_LENGTH / 2U] = { 0.0f };

/* Instancia da RFFT. */
static arm_rfft_fast_instance_f32 fft_instance;

/* Qual metade esta pronta (0 = baixa, 1 = alta). */
static volatile uint32_t adc_half_ready = 0U;

/* Resultados dos picos, para ver no debugger / shell / display.
 * Pico 1 = menor frequencia, Pico 2 = maior (ordenados para a
 * exibicao ficar estavel, sem trocar de lugar entre iteracoes). */
volatile uint32_t  g_peak_bin      = 0U;
volatile float32_t g_peak_freq_hz  = 0.0f;
volatile uint32_t  g_peak2_bin     = 0U;
volatile float32_t g_peak2_freq_hz = 0.0f;

/* Shell: recepcao por interrupcao, 1 caractere por vez. */
static uint8_t           rx_char = 0U;
static char              shell_line[SHELL_BUFFER_SIZE] = { 0 };
static volatile uint32_t shell_index = 0U;

/* Semaforo binario: a ISR de fim de TX (DMA da LPUART1) libera; a
 * Shell_Print bloqueia (dorme) ate a transmissao terminar. Sem polling. */
static SemaphoreHandle_t uart_tx_sem = NULL;

/* Software timer que pisca o LED (substitui a antiga LedTask). O callback
 * roda na task de timers do FreeRTOS, sem gastar uma task dedicada.     */
static TimerHandle_t     blink_timer = NULL;

/* Software timer one-shot para o debounce do botao (substitui o osDelay).
 * Enquanto button_locked == 1, novos cliques sao ignorados (repiques).  */
static TimerHandle_t     debounce_timer = NULL;
static volatile uint8_t  button_locked  = 0U;

/* Impressao centralizada (estilo do professor):
 *  - qualquer task chama Shell_Print(), que apenas empurra o texto para
 *    um STREAM BUFFER (protegido por um MUTEX contra escritas simultaneas);
 *  - uma unica print_task drena o buffer e transmite pela UART (DMA),
 *    esperando o fim da TX no semaforo. Assim os produtores nao bloqueiam
 *    na UART e as mensagens nunca se embaralham.                         */
static TaskHandle_t         printTaskHandle = NULL;
static StreamBufferHandle_t print_stream    = NULL;
static SemaphoreHandle_t    print_mutex     = NULL;   /* mutex com heranca de prioridade */
/* USER CODE END Variables */
/* Handles das tasks (FreeRTOS puro: xTaskCreate).
 * Prioridades e stacks passados direto na criacao (MX_FREERTOS_Init),
 * entao nao ha mais os structs osThreadAttr_t.                          */
TaskHandle_t defaultTaskHandle;
TaskHandle_t FFTTaskHandle;
TaskHandle_t ButtonTaskHandle;
TaskHandle_t ShellTaskHandle;
TaskHandle_t DisplayTaskHandle;
TaskHandle_t LedTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void U32_ToStr(uint32_t value, char *out);
static void Shell_Print(const char *text);
static void BlinkTimer_Callback(TimerHandle_t xTimer);
static void DebounceTimer_Callback(TimerHandle_t xTimer);
void StartPrintTask(void *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartFFTTask(void *argument);
void StartButtonTask(void *argument);
void StartShellTask(void *argument);
void StartDisplayTast(void *argument);
void StartLedTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* Mutex que protege as escritas no stream buffer de impressao.
   * xSemaphoreCreateMutex ja tem heranca de prioridade embutida.  */
  print_mutex = xSemaphoreCreateMutex();
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* Binario, inicia "vazio": o 1o take so libera quando a ISR de fim de
   * TX fizer o 1o give.                                                 */
  uart_tx_sem = xSemaphoreCreateBinary();
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* Timer periodico que pisca o LED (no lugar da LedTask). Comeca a
   * 500 ms; o botao troca o periodo (100/500 ms) via xTimerChangePeriod.
   * pdTRUE = auto-reload (periodico).                                    */
  blink_timer = xTimerCreate("blink", pdMS_TO_TICKS(led_period_ms),
                             pdTRUE, NULL, BlinkTimer_Callback);
  if (blink_timer != NULL)
  {
      (void)xTimerStart(blink_timer, 0U);
  }

  /* One-shot do debounce (pdFALSE = sem auto-reload); criado parado, o
   * clique e que o dispara com xTimerStart.                             */
  debounce_timer = xTimerCreate("debounce", pdMS_TO_TICKS(200U),
                                pdFALSE, NULL, DebounceTimer_Callback);
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* Stream buffer de 256 bytes; nivel de disparo 1 (acorda a print_task
   * assim que houver ao menos 1 byte).                                  */
  print_stream = xStreamBufferCreate(256U, 1U);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) -- FreeRTOS puro.
   * xTaskCreate(func, nome, stack EM PALAVRAS, param, prioridade, &handle).
   * Prioridades preservam a ordem do .ioc (8..40); stack em palavras = o
   * numero que antes vinha antes do "*4".                                */
  (void)xTaskCreate(StartDefaultTask, "defaultTask", 128, NULL, 24, &defaultTaskHandle);
  (void)xTaskCreate(StartFFTTask,     "FFTTask",     512, NULL, 40, &FFTTaskHandle);
  (void)xTaskCreate(StartButtonTask,  "ButtonTask",  128, NULL, 32, &ButtonTaskHandle);
  (void)xTaskCreate(StartShellTask,   "ShellTask",   256, NULL, 16, &ShellTaskHandle);
  (void)xTaskCreate(StartDisplayTast, "DisplayTask", 256, NULL,  8, &DisplayTaskHandle);

  /* LedTask removida: o LED e piscado pelo software timer blink_timer. */
  (void)LedTaskHandle;

  /* USER CODE BEGIN RTOS_THREADS */
  /* print_task: unica dona da UART de saida; drena o stream buffer. */
  (void)xTaskCreate(StartPrintTask, "printTask", 128, NULL, 8, &printTaskHandle);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    vTaskDelay(pdMS_TO_TICKS(1));
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartFFTTask */
/**
* @brief Function implementing the FFTTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartFFTTask */
void StartFFTTask(void *argument)
{
  /* USER CODE BEGIN StartFFTTask */
	  uint32_t   i         = 0U;
	  uint32_t   offset    = 0U;
	  uint32_t   idx1      = 0U;   /* bin do pico mais forte           */
	  uint32_t   idx2      = 0U;   /* bin do 2o pico mais forte        */
	  uint32_t   bin_lo    = 0U;   /* pico de menor frequencia         */
	  uint32_t   bin_hi    = 0U;   /* pico de maior frequencia         */
	  uint32_t   dist      = 0U;   /* distancia (em bins) ate idx1     */
	  float32_t  max1      = 0.0f;
	  float32_t  max2      = 0.0f;
	  arm_status status    = ARM_MATH_SUCCESS;

	  /* Inicializa a FFT (uma vez). */
	  status = arm_rfft_fast_init_f32(&fft_instance, (uint16_t)FFT_LENGTH);
	  if (status != ARM_MATH_SUCCESS)
	  {
	      Error_Handler();
	  }
	  else
	  {
	      /* Liga TIM2 e ADC em DMA circular (kernel ja rodando). */
	      if (HAL_TIM_Base_Start(&htim2) != HAL_OK)
	      {
	          Error_Handler();
	      }
	      else if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
	      {
	    	  Error_Handler();
	      }
	      else if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer,
								   (uint32_t)ADC_BUFFER_LENGTH) != HAL_OK)
	      {
	    	  Error_Handler();
	      }
	      else
	      {
	          /* ok */
	      }
	  }

	  if (HAL_DAC_Start(&hdac1, DAC_CHANNEL_1) != HAL_OK)
	  {
	      Error_Handler();
	  }

	  if (HAL_TIM_Base_Start_IT(&htim6) != HAL_OK)
	  {
	      Error_Handler();
	  }

	  for(;;)
	  {
	    /* Espera o callback (sem polling). */
	    (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	    /* Escolhe a metade pronta. */
	    if (adc_half_ready == 0U)
	    {
	        offset = 0U;
	    }
	    else
	    {
	        offset = FFT_LENGTH;
	    }

	    /* Converte uint16 -> float. */
	    for (i = 0U; i < FFT_LENGTH; i++)
	    {
	        fft_input[i] = (float32_t)adc_buffer[offset + i];
	    }

	    /* FFT + magnitude. */
	    arm_rfft_fast_f32(&fft_instance, fft_input, fft_output, (uint8_t)0U);
	    arm_cmplx_mag_f32(fft_output, fft_magnitude, (FFT_LENGTH / 2U));

	    /* 1o pico: maior magnitude, ignorando o bin 0 (DC). */
	    max1 = 0.0f;
	    idx1 = 0U;
	    for (i = 1U; i < (FFT_LENGTH / 2U); i++)
	    {
	        if (fft_magnitude[i] > max1)
	        {
	            max1 = fft_magnitude[i];
	            idx1 = i;
	        }
	        else
	        {
	            /* nao e novo maximo */
	        }
	    }

	    /* 2o pico: maior magnitude fora de uma janela de +-2 bins em
	     * volta do 1o, para nao pegar um bin vizinho do mesmo tom.   */
	    max2 = 0.0f;
	    idx2 = 0U;
	    for (i = 1U; i < (FFT_LENGTH / 2U); i++)
	    {
	        dist = (i > idx1) ? (i - idx1) : (idx1 - i);
	        if ((dist > 2U) && (fft_magnitude[i] > max2))
	        {
	            max2 = fft_magnitude[i];
	            idx2 = i;
	        }
	        else
	        {
	            /* dentro da janela do 1o pico ou nao e maximo */
	        }
	    }

	    /* Ordena por frequencia (menor primeiro) para exibicao estavel,
	     * MAS so troca se o 2o pico existir de fato (max2 > 0). Assim, se
	     * o 2o pico nao for encontrado (idx2 == 0, ex.: durante uma queda
	     * de sinal), o pico principal (idx1) nunca aparece como 0 Hz.   */
	    if ((max2 > 0.0f) && (idx2 < idx1))
	    {
	        bin_lo = idx2;
	        bin_hi = idx1;
	    }
	    else
	    {
	        bin_lo = idx1;
	        bin_hi = idx2;
	    }

	    g_peak_bin      = bin_lo;
	    g_peak_freq_hz  = ((float32_t)bin_lo * SAMPLE_RATE_HZ)
	                      / (float32_t)FFT_LENGTH;
	    g_peak2_bin     = bin_hi;
	    g_peak2_freq_hz = ((float32_t)bin_hi * SAMPLE_RATE_HZ)
	                      / (float32_t)FFT_LENGTH;
	  }
  /* USER CODE END StartFFTTask */
}

/* USER CODE BEGIN Header_StartButtonTask */
/**
* @brief Function implementing the ButtonTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartButtonTask */
void StartButtonTask(void *argument)
{
  /* USER CODE BEGIN StartButtonTask */
	  /* Espera o clique (notificado pela ISR do EXTI). Sem polling. */
	  for(;;)
	  {
	    (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	    /* Debounce por timer one-shot: durante a janela (button_locked),
	     * ignora os repiques do clique. Substitui o antigo osDelay(200),
	     * entao a task nao fica mais bloqueada esperando o debounce.     */
	    if (button_locked == 0U)
	    {
	        button_locked = 1U;

	        /* Alterna a velocidade do pisca do LED e reprograma o timer. */
	        if (led_period_ms == 500U)
	        {
	            led_period_ms = 100U;
	        }
	        else
	        {
	            led_period_ms = 500U;
	        }
	        if (blink_timer != NULL)
	        {
	            /* Troca o periodo e reinicia o timer do LED. */
	            (void)xTimerChangePeriod(blink_timer,
	                                     pdMS_TO_TICKS(led_period_ms), 0U);
	        }

	        /* Arma o one-shot de 200 ms (periodo ja definido na criacao).
	         * Ao expirar, o callback zera button_locked.                 */
	        if (debounce_timer != NULL)
	        {
	            (void)xTimerStart(debounce_timer, 0U);
	        }
	    }
	    else
	    {
	        /* dentro da janela de debounce: ignora (repique) */
	    }
	  }
  /* USER CODE END StartButtonTask */
}

/* USER CODE BEGIN Header_StartShellTask */
/**
* @brief Function implementing the ShellTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartShellTask */
void StartShellTask(void *argument)
{
  /* USER CODE BEGIN StartShellTask */
	  char numbuf[12];

	  (void)HAL_UART_Receive_IT(&hlpuart1, &rx_char, 1U);
	  Shell_Print("\r\nShell pronto. Digite 'help'.\r\n> ");

	  for(;;)
	  {
	    (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	    if (strcmp(shell_line, "help") == 0)
	    {
	    	Shell_Print("Comandos: help, fft, heap, tasks, runtime\r\n");
	    }
	    else if (strcmp(shell_line, "fft") == 0)
	    {
	        Shell_Print("Pico 1: bin=");
	        U32_ToStr(g_peak_bin, numbuf);
	        Shell_Print(numbuf);
	        Shell_Print("  freq=");
	        U32_ToStr((uint32_t)g_peak_freq_hz, numbuf);
	        Shell_Print(numbuf);
	        Shell_Print(" Hz\r\n");

	        Shell_Print("Pico 2: bin=");
	        U32_ToStr(g_peak2_bin, numbuf);
	        Shell_Print(numbuf);
	        Shell_Print("  freq=");
	        U32_ToStr((uint32_t)g_peak2_freq_hz, numbuf);
	        Shell_Print(numbuf);
	        Shell_Print(" Hz\r\n");
	    }
	    else if (strcmp(shell_line, "heap") == 0)
	    {
	        Shell_Print("Heap livre: ");
	        U32_ToStr((uint32_t)xPortGetFreeHeapSize(), numbuf);
	        Shell_Print(numbuf);
	        Shell_Print(" bytes\r\n");
	    }
	    else if (strcmp(shell_line, "tasks") == 0)
		{
			Shell_Print("Tarefas instaladas:\r\n");
			Shell_Print(" FFTTask     (hard, prio alta)\r\n");
			Shell_Print(" ButtonTask  (prio media)\r\n");
			Shell_Print(" ShellTask   (soft)\r\n");
			Shell_Print(" DisplayTask (soft)\r\n");
			Shell_Print(" LedTask     (guia)\r\n");
			Shell_Print("Num de tarefas: ");
			U32_ToStr((uint32_t)uxTaskGetNumberOfTasks(), numbuf);
			Shell_Print(numbuf);
			Shell_Print("\r\n");
		}
	    else if (strcmp(shell_line, "runtime") == 0)
		{
			TaskStatus_t status[8];
			UBaseType_t  count;
			UBaseType_t  k;
			uint32_t     total;

			count = uxTaskGetSystemState(status, 8U, &total);

			Shell_Print("Tarefa\t\tTempo CPU\r\n");
			for (k = 0U; k < count; k++)
			{
				Shell_Print(" ");
				Shell_Print(status[k].pcTaskName);
				Shell_Print("\t");
				U32_ToStr(status[k].ulRunTimeCounter, numbuf);
				Shell_Print(numbuf);
				Shell_Print("\r\n");
			}
		}
	    else if (shell_line[0] == '\0')
	    {
	        /* linha vazia */
	    }
	    else
	    {
	        Shell_Print("Comando desconhecido. Digite 'help'.\r\n");
	    }

	    shell_index = 0U;
	    Shell_Print("> ");
	  }
  /* USER CODE END StartShellTask */
}

/* USER CODE BEGIN Header_StartDisplayTast */
/**
* @brief Function implementing the DisplayTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDisplayTast */
void StartDisplayTast(void *argument)
{
  /* USER CODE BEGIN StartDisplayTast */
	  char freq_str[8];

	  /* Inicia o display uma vez. */
	  SSD1306_Init();
	  SSD1306_Clear();
	  SSD1306_WriteString(0U, 0U, "FFT:");

	  for(;;)
	  {
	    /* Pico 1 (menor frequencia) na pagina 2. */
	    U32_ToStr((uint32_t)g_peak_freq_hz, freq_str);
	    SSD1306_WriteString(2U, 0U, "        ");   /* limpa a linha */
	    SSD1306_WriteString(2U, 0U, freq_str);
	    SSD1306_WriteString(2U, 48U, "Hz");

	    /* Pico 2 (maior frequencia) na pagina 4. */
	    U32_ToStr((uint32_t)g_peak2_freq_hz, freq_str);
	    SSD1306_WriteString(4U, 0U, "        ");   /* limpa a linha */
	    SSD1306_WriteString(4U, 0U, freq_str);
	    SSD1306_WriteString(4U, 48U, "Hz");

	    vTaskDelay(pdMS_TO_TICKS(500U));
	  }
  /* USER CODE END StartDisplayTast */
}

/* USER CODE BEGIN Header_StartLedTask */
/**
* @brief Function implementing the LedTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLedTask */
void StartLedTask(void *argument)
{
  /* USER CODE BEGIN StartLedTask */
	  /* O LED agora e piscado pelo software timer blink_timer, nao mais por
	   * esta task. Ela nao e criada (ver MX_FREERTOS_Init). Se por acaso for
	   * criada de novo (ex.: apos regenerar no CubeMX), apenas se encerra
	   * para nao conflitar com o timer.                                     */
	  vTaskDelete(NULL);
  /* USER CODE END StartLedTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* Callback do software timer: inverte o LED. Roda na task de timers do
 * FreeRTOS (contexto de task, nao ISR). Substitui a antiga LedTask.    */
static void BlinkTimer_Callback(TimerHandle_t xTimer)
{
    (void)xTimer;
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

/* Callback do one-shot de debounce: a janela acabou, aceita o proximo
 * clique. Roda na task de timers do FreeRTOS.                          */
static void DebounceTimer_Callback(TimerHandle_t xTimer)
{
    (void)xTimer;
    button_locked = 0U;
}

/* Converte um uint32 em string decimal. Retorna o tamanho. */
static void U32_ToStr(uint32_t value, char *out)
{
    char     tmp[11];
    uint32_t i = 0U;
    uint32_t j = 0U;

    if (value == 0U)
    {
        out[0] = '0';
        out[1] = '\0';
    }
    else
    {
        while (value > 0U)
        {
            tmp[i] = (char)('0' + (value % 10U));
            value = value / 10U;
            i++;
        }
        while (i > 0U)
        {
            i--;
            out[j] = tmp[i];
            j++;
        }
        out[j] = '\0';
    }
}

/* Task de impressao: unica dona da UART de saida. Fica bloqueada lendo o
 * stream buffer; quando chega texto, transmite por DMA e dorme no semaforo
 * ate a ISR sinalizar o fim. Serializa toda a saida do sistema.          */
void StartPrintTask(void *argument)
{
    static uint8_t txbuf[128];
    size_t         n = 0U;

    (void)argument;

    for (;;)
    {
        /* Bloqueia ate haver bytes (sem polling). Le ate 128 por vez. */
        n = xStreamBufferReceive(print_stream, txbuf, sizeof(txbuf),
                                 portMAX_DELAY);
        if (n > 0U)
        {
            if (HAL_UART_Transmit_DMA(&hlpuart1, txbuf, (uint16_t)n) == HAL_OK)
            {
                /* Espera dormindo o fim da TX (timeout de seguranca). */
                (void)xSemaphoreTake(uart_tx_sem, pdMS_TO_TICKS(100U));
            }
            else
            {
                /* periferico ocupado/erro: descarta este pedaco */
            }
        }
    }
}

/* Enfileira texto para impressao: apenas escreve no stream buffer, sob o
 * mutex (produtores nunca se embaralham). NAO bloqueia na UART -> a task
 * chamadora segue em frente enquanto a print_task transmite.
 * Fallback: se o buffer/mutex nao existirem, transmite direto (bloqueante). */
static void Shell_Print(const char *text)
{
    size_t len = strlen(text);

    if (len == 0U)
    {
        return;
    }

    if ((print_stream != NULL) && (print_mutex != NULL) &&
        (printTaskHandle != NULL))
    {
        (void)xSemaphoreTake(print_mutex, portMAX_DELAY);
        (void)xStreamBufferSend(print_stream, text, len, portMAX_DELAY);
        (void)xSemaphoreGive(print_mutex);
    }
    else
    {
        (void)HAL_UART_Transmit(&hlpuart1, (uint8_t *)text,
                                (uint16_t)len, 100U);
    }
}

/* Fim da transmissao DMA da LPUART1: libera a print_task. Contexto de ISR
 * -> xSemaphoreGiveFromISR + portYIELD_FROM_ISR.                        */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t higher_woken = pdFALSE;

    if (huart == &hlpuart1)
    {
        (void)xSemaphoreGiveFromISR(uart_tx_sem, &higher_woken);
        portYIELD_FROM_ISR(higher_woken);
    }
    else
    {
        /* outra UART: ignora */
    }
}

/* Recebe 1 caractere. Monta a linha e, no Enter, acorda a ShellTask. */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t higher_woken = pdFALSE;

    if (huart == &hlpuart1)
    {
        if ((rx_char == (uint8_t)'\r') || (rx_char == (uint8_t)'\n'))
        {
            shell_line[shell_index] = '\0';
            vTaskNotifyGiveFromISR(ShellTaskHandle, &higher_woken);
        }
        else
        {
            if (shell_index < (SHELL_BUFFER_SIZE - 1U))
            {
                shell_line[shell_index] = (char)rx_char;
                shell_index++;
            }
            else
            {
                /* buffer cheio: ignora */
            }
        }

        (void)HAL_UART_Receive_IT(&hlpuart1, &rx_char, 1U);
        portYIELD_FROM_ISR(higher_woken);
    }
    else
    {
        /* outra UART: ignora */
    }
}

/* Clique do botao (PC13): acorda a ButtonTask. Contexto de ISR. */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    BaseType_t higher_woken = pdFALSE;

    if (GPIO_Pin == BTN_Pin)
    {
        vTaskNotifyGiveFromISR(ButtonTaskHandle, &higher_woken);
        portYIELD_FROM_ISR(higher_woken);
    }
    else
    {
        /* outro pino: ignora */
    }
}

/* Primeira metade do buffer pronta: acorda a FFTTask. */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    BaseType_t higher_woken = pdFALSE;

    if (hadc == &hadc1)
    {
        adc_half_ready = 0U;
        vTaskNotifyGiveFromISR(FFTTaskHandle, &higher_woken);
        portYIELD_FROM_ISR(higher_woken);
    }
    else
    {
        /* outro ADC: ignora */
    }
}

/* Segunda metade pronta. */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    BaseType_t higher_woken = pdFALSE;

    if (hadc == &hadc1)
    {
        adc_half_ready = 1U;
        vTaskNotifyGiveFromISR(FFTTaskHandle, &higher_woken);
        portYIELD_FROM_ISR(higher_woken);
    }
    else
    {
        /* outro ADC: ignora */
    }
}
/* USER CODE END Application */

