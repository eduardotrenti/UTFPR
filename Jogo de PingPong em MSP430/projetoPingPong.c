/*
....:::: TRABALHO FINAL DA DISCIPLINA DE SISTEMAS MICROCONTROLADOS - 2025/2 ::::....

-> Clássico Jogo PONG

Materiais:
  -> Microcontrolador MSP430G2553
  -> 2x Matriz de LED 8x8 MAX7219 (circuito integrado)
  -> 2x Potenciômetro 10kohm.
  -> Protoboard.
*/

#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

#define SPI_SIMO	BIT7
#define SPI_CLK		BIT5
#define SPI_CS		BIT4

#define MAX_NOOP	0x00
#define MAX_DIGIT0	0x01
#define MAX_DIGIT1	0x02
#define MAX_DIGIT2	0x03
#define MAX_DIGIT3	0x04
#define MAX_DIGIT4	0x05
#define MAX_DIGIT5	0x06
#define MAX_DIGIT6	0x07
#define MAX_DIGIT7	0x08
#define MAX_DECODEMODE	0x09
#define MAX_INTENSITY	0x0A
#define MAX_SCANLIMIT	0x0B
#define MAX_SHUTDOWN	0x0C
#define MAX_DISPLAYTEST	0x0F

void ini_P1_P2(void);
void spi_init(void);
void spi_max(uint8_t address, uint8_t data, uint8_t address2, uint8_t data2);
void tela_inicial(void);
void ini_timerA_debouncer(void);
void ini_uCon(void);
void ini_ADC10(void);

void desenha_raquete_esq(void);
void desenha_raquete_dir(void);
void desenha_bola(void);
void atualiza_bola(void);
void loop_pong(void);
void ini_timerA_game(void);
void mostra_modo(int dificuldade);
void mostra_ponto(unsigned int ponto, unsigned int ponto2);

//Matriz de pontos para imprimir na matriz.
const uint8_t pontos[8][8] = {
  {0b00111100,0b01000010,0b01000010,0b00111100,0b00000000,0b01000010,0b00100100,0b00011000},
  {0b00000000,0b01000100,0b01111110,0b01000000,0b00000000,0b01000010,0b00100100,0b00011000},
  {0b01000100,0b01100010,0b01010010,0b01001100,0b00000000,0b01000010,0b00100100,0b00011000},
  {0b00100100,0b01000010,0b01001010,0b00111100,0b00000000,0b01000010,0b00100100,0b00011000},
  {0b00011000,0b00100100,0b01000010,0b00000000,0b00111100,0b01000010,0b01000010,0b00111100},
  {0b00011000,0b00100100,0b01000010,0b00000000,0b01000100,0b01111110,0b01000000,0b00000000},
  {0b00011000,0b00100100,0b01000010,0b00000000,0b01000100,0b01100010,0b01010010,0b01001100},
  {0b00011000,0b00100100,0b01000010,0b00000000,0b00100100,0b01000010,0b01001010,0b00111100}
};

//Matriz de LEDs que mostra os modos.
const uint8_t modos[4][8] = {
  {0b01111110,0b00001000,0b01111110,0b00000000,0b01111110,0b00001010,0b01111110,0b00000000},
  {0b01111110,0b00010010,0b01101110,0b00000000,0b01111110,0b01000010,0b00111100,0b00000000},
  {0b01111110,0b01001010,0b01001010,0b00000000,0b01111110,0b00010010,0b01111110,0b00000000},
  {0b01001110,0b01001010,0b01001010,0b01111010,0b00000000,0b01101110,0b01001000,0b01111110}
};

//Matriz de LEDs da escrita "PONG" da tela inicial.
const uint8_t IMAGES[38][8] = {
  {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
  {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
  {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
  {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b01111110},
  {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
  {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b01111110,0b00010010,0b00010010},
  {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
  {0b00000000,0b00000000,0b00000000,0b01111110,0b00010010,0b00010010,0b00001100,0b00000000},
  {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
  {0b00000000,0b01111110,0b00010010,0b00010010,0b00001100,0b00000000,0b00111100,0b01000010},
  {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b01111110},
  {0b00010010,0b00010010,0b00001100,0b00000000,0b00111100,0b01000010,0b01000010,0b00111100},
  {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b01111110,0b00010010,0b00010010},
  {0b00001100,0b00000000,0b00111100,0b01000010,0b01000010,0b00111100,0b00000000,0b01111110},
  {0b00000000,0b00000000,0b00000000,0b01111110,0b00010010,0b00010010,0b00001100,0b00000000},
  {0b00111100,0b01000010,0b01000010,0b00111100,0b00000000,0b01111110,0b00000100,0b00011000},
  {0b00000000,0b01111110,0b00010010,0b00010010,0b00001100,0b00000000,0b00111100,0b01000010},
  {0b01000010,0b00111100,0b00000000,0b01111110,0b00000100,0b00011000,0b00100000,0b01111110},
  {0b00010010,0b00010010,0b00001100,0b00000000,0b00111100,0b01000010,0b01000010,0b00111100},
  {0b00000000,0b01111110,0b00000100,0b00011000,0b00100000,0b01111110,0b00000000,0b00111100},
  {0b00001100,0b00000000,0b00111100,0b01000010,0b01000010,0b00111100,0b00000000,0b01111110},
  {0b00000100,0b00011000,0b00100000,0b01111110,0b00000000,0b00111100,0b01000010,0b01010010},
  {0b00111100,0b01000010,0b01000010,0b00111100,0b00000000,0b01111110,0b00000100,0b00011000},
  {0b00100000,0b01111110,0b00000000,0b00111100,0b01000010,0b01010010,0b00110100,0b00000000},
  {0b01000010,0b00111100,0b00000000,0b01111110,0b00000100,0b00011000,0b00100000,0b01111110},
  {0b00000000,0b00111100,0b01000010,0b01010010,0b00110100,0b00000000,0b00000000,0b00000000},
  {0b00000000,0b01111110,0b00000100,0b00011000,0b00100000,0b01111110,0b00000000,0b00111100},
  {0b01000010,0b01010010,0b00110100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
  {0b00000100,0b00011000,0b00100000,0b01111110,0b00000000,0b00111100,0b01000010,0b01010010},
  {0b00110100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
  {0b00100000,0b01111110,0b00000000,0b00111100,0b01000010,0b01010010,0b00110100,0b00000000},
  {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
  {0b00000000,0b00111100,0b01000010,0b01010010,0b00110100,0b00000000,0b00000000,0b00000000},
  {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
  {0b01000010,0b01010010,0b00110100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
  {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
  {0b00110100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
  {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000}
};

unsigned int botaoPres = 1, jogo_pausado = 0, jogo_comecando = 1;
unsigned int ADC10_vetor[8];
uint8_t canal_adc = 1;   // 1 = A1 (raquete esq), 2 = A2 (raquete dir)
unsigned int i, row, framecounter, dificuldade = 1, jogo_ativo = 0, pontos_1 = 0, pontos_2 = 0;
uint16_t soma = 0, media = 0;

// Vetores que controlam o display
uint8_t tela_esq[8] = {0};
uint8_t tela_dir[8] = {0};

// posição das raquetes (topo) -> recebem o valor lido no ADC10
uint8_t raquete_esq_y = 2;
uint8_t raquete_dir_y = 2;

// posição da bolinha (coordenada global)
uint8_t bola_x = 7;
uint8_t bola_y = 3;

// velocidade/direção da bolinha
int8_t vx = -1;       // esquerda/direita
int8_t vy =  1;       // cima/baixo

// Função main -> apenas chama as funções que regem o jogo
int main(void)
{
    ini_P1_P2();
    ini_uCon();
    ini_timerA_debouncer();
    spi_init();
    ini_ADC10();
    tela_inicial();

    while(1)
    {
      //default
    }
}

// spi_max -> responsável por...
void spi_max(uint8_t address, uint8_t data, uint8_t address2, uint8_t data2)
{
    P1OUT &= ~(SPI_CS); // força o pino do chip select para nivel baixo, selecionando o dispositivo spi, informa que a transmissao vai comecar
	UCB0TXBUF = address2 & 0b00001111;
	//UCB0TXBUF buffer de transmissão SPI do usci_b0, escreve o valor no buffer de transm, msp gera clock spi
	//O byte é enviado bit a bit no pino SIMO
    // 0b00001111 garante que vai ser usado os 4 bits menos significativos

	while (UCB0STAT & UCBUSY);//espera a transmissao terminar, UCB0STAT registrador de estados
	UCB0TXBUF = data2;// envia o dado do segundo dispositivo, referente ao endereco address2

	while (UCB0STAT & UCBUSY);
    UCB0TXBUF = address & 0b00001111;//em cascata o primeiro byte vai para o ultimo CI da cadeia, o ultimo byte pra matriz mais perto

	while (UCB0STAT & UCBUSY);
	UCB0TXBUF = data;// transmite o dado do address

	while (UCB0STAT & UCBUSY);
    P1OUT |= SPI_CS;//pino cs em nivel alto, finaliza a comunicacao SPI
}

// mostra_modo -> mostra os arrays de bits dos dois modos de jogo existentes
void mostra_modo(int dificuldade)
{
  if(dificuldade == 1) // MODO HARD
  {
    for(i = 0; i < 8; i++)
    {
        spi_max(MAX_DIGIT0+i, modos[0][i], MAX_DIGIT0+i, modos[1][i]);
    }
  }
  else // MODO EASY
  {
    for(i = 0; i < 8; i++)
    {
        spi_max(MAX_DIGIT0+i, modos[2][i], MAX_DIGIT0+i, modos[3][i]);
    }
  }
}

// mostra_ponto -> mostra o placar final do jogo de acordo com a pontuação dos jogadores
void mostra_ponto(unsigned int ponto, unsigned int ponto2)
{
  for (row=0; row<8; row++){
            spi_max(MAX_DIGIT0+row, pontos[ponto][row], MAX_DIGIT0+row, pontos[ponto2+4][row]);
  }
  __delay_cycles(2000000);

  // zerar os pontos para o próximo jogo
  pontos_1 = 0;
  pontos_2 = 0;

}

// ini_uCon -> inicializa o microControlador
void ini_uCon(void)
{
    // configurações default
    WDTCTL = WDTPW + WDTHOLD;
    DCOCTL = CALDCO_16MHZ;
    BCSCTL1 = CALBC1_16MHZ;
    BCSCTL2 = DIVS1;
    BCSCTL3 = XCAP0 + XCAP1;

    while(BCSCTL3 & LFXT1OF);

    __enable_interrupt();
}

//ini_P1_P2 -> Inicializa as portas de ES
void ini_P1_P2(void)
{
  P1DIR |= SPI_CS; // BIT4 como saída para o MAX7219
  P1DIR &= ~(BIT3); // BIT3 configurado como entrada (Botão S2)
  P1OUT |= SPI_CS + BIT3; //Saída em nível alto e botão pull-up
  P1REN = BIT3; // Habilita resistor no pino de entrada
  P1IES = BIT3; // Borda de descida
  P1IFG = 0; // Flag começa zerada
  P1IE = BIT3; // Habilita interrupção em S2

  //Mudar as funções dos pinos P1.5 e P1.7
  P1SEL |= SPI_SIMO + SPI_CLK;
  P1SEL2 |= SPI_SIMO + SPI_CLK;
}

// Rotina de Interrupção do botão S2
#pragma vector=PORT1_VECTOR
__interrupt void RTI_PORTA_1(void)
{
  P1IE &= ~BIT3; // Desabilita interrupção
  botaoPres = 0; // Botao foi pressionado -> 0 (Lógica invertida)
  TA0CTL |= MC0; // Habilita o timer0 para debouncer do botão

  // Verificações para possíveis interrupções durante o looping do jogo
  if(jogo_ativo == 2) //Se está no jogo, e não no loop "PONG" ou na tela de seleção de modo
  {
    if(jogo_comecando == 1)// Se a interrupção ocorre quando o jogo acabou de começar
    {
      jogo_pausado = 0;
      jogo_comecando = 0;
    }
    else if(jogo_comecando == 0)// Se a interrupção acontecer quando o jogo já começou
    {
      // Alterna a variável que controla o pause do jogo, através de cada clique de S2
      // Pausa e despausa o jogo
      if(jogo_pausado == 0)
      {
        jogo_pausado = 1;
      }
      else if(jogo_pausado == 1)
      {
        jogo_pausado = 0;
      }
    }
  }
}

//ini_timerA_debouncer -> inicializa o timer0A para o debouncer do botão S2
void ini_timerA_debouncer(void)
{
    TA0CTL = TASSEL1 + ID0 + ID1 + MC0;
    TA0CCTL0 = CCIE;
    TA0CCR0 = 49999;
}

//Rotina de interrupção do timer0A para o debouncer de S2
#pragma vector=TIMER0_A0_VECTOR
__interrupt void RTI_Mod_0_Timer_0(void)
{
  TA0CTL &= ~MC0; //desabilita o timer 0
  P1IFG &= ~BIT3; //limpa a flag do BIT3
  P1IE |= BIT3; // reabilita interrupção no BIT3
}

// ini_timerA_game -> inicializa o timer1A para realizar as interrupções e rodar o jogo
void ini_timerA_game(void)
{
  TA1CTL = TASSEL1 + ID0 + ID1 + MC0 + TACLR; // SMCLK, /8, up mode

  // seleciona a dificuldade
  if(dificuldade == 1){
    TA1CCR0 = 24999; // 50 ms (MODO HARD)
  }
  else{
    TA1CCR0 = 49999; // 100ms (MODO EASY)
  }
  TA1CCTL0 = CCIE;   // interrupção
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void RTI_Timer_Game(void)
{
  ADC10CTL0 |= ENC + ADC10SC; // Habilita a conversão do sinal no ADC10

  // Implementa a lógica do jogo, além de enviar para a matriz para ser mostrado
  loop_pong();

  // Pausa o timer se o jogo está começando ou se o jogo for pausado
  // Interrupção causada pelo botão S2 ou se o jogo está começando

  if(jogo_pausado == 1 || jogo_comecando == 1)
    TA1CTL &= ~MC0;
}

// ini_ADC10 -> Inicializa o ADC10
void ini_ADC10(void)
{
  // Vr+ = Vcc, Vr- = Vss
  // 16xADC10CLKs
  // Multiple sample and conversion (MSC)
  // Interrupção habilitada e ADC10ON -> default
  ADC10CTL0 = ADC10SHT1 + MSC + ADC10IE + ADC10ON;
  // Input channel: A1
  // Clock: SMCLK/2
  // CONSEQ1 -> repeat single channel
  ADC10CTL1 = INCH0 + ADC10SSEL1 + ADC10SSEL0 + ADC10DIV0 + CONSEQ1;
  //Habilita as entradas analógicas nos bits
  ADC10AE0 = BIT1 + BIT2;

  //Default
  ADC10DTC0 = 0;
  ADC10DTC1 = 8; // 8 amostras
  ADC10SA = &ADC10_vetor[0]; //Default

  //Habilita conversão
  ADC10CTL0 |= ENC + ADC10SC;
}

// Rotina de interrupção do ADC10
#pragma vector=ADC10_VECTOR
__interrupt void RTI_ADC10(void)
{
    // Desabilita conversão
    ADC10CTL0 &= ~ENC;

    // atualiza as variáveis usadas na conversão
    soma = 0;
    media = 0;

    // Soma dos valores das 8 amostras
    for(i = 0; i < 8; i++){
        soma += ADC10_vetor[i];
    }

    // Média dos valores
    media = soma / 8;

    // Conversão
    // POTENCIÔMETRO ESQUERDO (A1)
    if(canal_adc == 1){
      if(dificuldade == 1){
        raquete_esq_y = (media * 6) / 1023;
      }
      else{
        raquete_esq_y = (media * 5) / 1023;
      }
        // troca para A2
        canal_adc = 2;
        ADC10CTL1 &= ~(INCH0+INCH1+INCH2);
        ADC10CTL1 |= INCH1;
    }

    // POTENCIÔMETRO DIREITO (A2)
    else{
      if(dificuldade == 1){
        raquete_dir_y = (media * 6) / 1023;
      }
      else{
        raquete_dir_y = (media * 5) / 1023;
      }
        // volta para A1
        canal_adc = 1;
        ADC10CTL1 &= ~(INCH0+INCH1+INCH2);
        ADC10CTL1 |= INCH0;
    }

    ADC10SA = &ADC10_vetor[0];
}

// spi_init -> Inicialização do MODO SPI
void spi_init(void)
{
	UCB0CTL1 |= UCSWRST;//software reset, quando UCSWRST = 1 módulo usci fica parado
	UCB0CTL0 = UCCKPH + UCMSB + UCMST + UCSYNC;// UCCKPH - clock phase select o dado é amostrado em uma borda ,ucmSB bit mais significativo, ucmst - master, ucsync - modo sincrono( sem isso funcionaria como UART)
	UCB0CTL1 |= UCSSEL1;//SMCL

	//f_spi = f_smlck/1, porque ucb0br = 0x0001 -> 1
	UCB0BR0 |= 0x01; //8 bits menos significativos
	UCB0BR1 = 0; //8 bits mais significativos
	UCB0CTL1 &= ~UCSWRST; // Desativa software reset
}

// tela_inicial -> Função onde ocorre todo o loop mostrado na matriz de LED
// máquina de estados
void tela_inicial(void)
{
  // Início da máquina de estados
  while(1)
  {
    if(jogo_ativo == 0) // jogo_ativo == 0 -> loop do PONG
    {
      while(1)
      {
        //impressão dos frames do PONG em movimento
        for (framecounter=0; framecounter<38; framecounter = framecounter+2)
        {
          for (row=0; row<8; row++)
          {
            spi_max(MAX_DIGIT0+row, IMAGES[framecounter][row], MAX_DIGIT0+row, IMAGES[framecounter+1][row]);
          }
          __delay_cycles(2000000); //delay para melhor visualização
        }

        if(botaoPres == 0)// botao foi pressionado -> passa para a tela de modo de jogo
        {
          jogo_ativo = 1; // passa para o próximo estado
          botaoPres = 1; // reseta a variável que diz se o botão foi ou não clicado
          break; // Sai do loop PONG
        }
      }
    }

    else if(jogo_ativo == 1) // jogo_ativo == 1 -> modo de seleção de dificuldade
    {
      mostra_modo(dificuldade);

      while(1){ //loop de aguardo para o jogador selecionar a dificuldade
        dificuldade = ~dificuldade; //alterna a dificuldade do jogo
        mostra_modo(dificuldade);

        //delay para que seja possível o jogador ler e selecionar a dificuldade desejada
        __delay_cycles(12000000);

        // Jogador pressiona o botao -> seleciona a dificuldade que está mostrada na tela
        if(botaoPres == 0)
        {
          jogo_ativo = 2; // passa para o próximo estado
          botaoPres = 1; // reseta a variável que diz se o botão foi ou não clicado
          break; // sai do loop de escolha de dificuldade
        }
      }
    }
    else if(jogo_ativo == 2) // jogo_ativo == 2 -> começa a lógica do jogo
    {
      // setam a bolinha no centro da tela
      bola_x = 3;
      bola_y = 7;
      vx = -1;
      vy = 1;

      ini_timerA_game(); // Função que inicializa o timer1A, onde o jogo acontece.

      // Após o término do jogo
      while(1) // Aguarda botao ser pressionado para voltar ao inicio
      {
        if(botaoPres == 0) // botão pressionado
        {
          botaoPres = 1; // reseta a variável que diz se o botão foi ou não clicado
          break; // Volta para o início da máquina de estados (loop PONG)
        }
      }
    }
  }
}

// EASY → 3 LEDs horizontais (verticais na matriz visível, pois ela está rotacionada)
void desenha_raquete_esq(void)
{
    tela_esq[0] |= (1 << raquete_esq_y);
    tela_esq[0] |= (1 << raquete_esq_y + 1);
    tela_esq[0] |= (1 << raquete_esq_y + 2);
}

void desenha_raquete_dir(void)
{
    tela_dir[7] |= (1 << raquete_dir_y);
    tela_dir[7] |= (1 << raquete_dir_y + 1);
    tela_dir[7] |= (1 << raquete_dir_y + 2);
}

// HARD → 2 LEDs horizontais (verticais na matriz visível, pois ela está rotacionada)
void desenha_raquete_esq_hard(void)
{
    tela_esq[0] |= (1 << raquete_esq_y);
    tela_esq[0] |= (1 << raquete_esq_y + 1);
}

void desenha_raquete_dir_hard(void)
{
    tela_dir[7] |= (1 << raquete_dir_y);
    tela_dir[7] |= (1 << raquete_dir_y + 1);
}

// desenha_bola -> desenha a bola de acordo com sua posição na matriz
void desenha_bola(void)
{
    if(bola_y < 8) // matriz esquerda
    {
        tela_esq[bola_y] |= (1 << bola_x);
    }
    else // matriz direita
    {
        tela_dir[bola_y - 8] |= (1 << bola_x);
    }
}

// atualiza_bola -> atualiza a posição da bolinha na matriz após um ciclo do timer
void atualiza_bola(void)
{
    bola_x += vx; // linha
    bola_y += vy; // coluna total

    // colisão topo/fundo (agora linhas)
    if(bola_x == 0 || bola_x == 7)
        vx = -vx;

    // Tratamento de colisões com as raquetes

    if(dificuldade == 1) // HARD (2 LEDs)
    {
        // raquete esquerda
        if(bola_y == 1)
        {
          // verificação se a bolinha colidiu com um LED aceso (raquete)
            if(bola_x >= raquete_esq_y && bola_x <= raquete_esq_y + 1)
                vy = -vy;
        }

        // raquete direita
        if(bola_y == 14)
        {
          // verificação se a bolinha colidiu com um LED aceso (raquete)
            if(bola_x >= raquete_dir_y && bola_x <= raquete_dir_y + 1)
                vy = -vy;
        }
    }
    else // EASY (3 LEDs)
    {
        // raquete esquerda
        if(bola_y == 1)
        {
          // verificação se a bolinha colidiu com um LED aceso (raquete)
            if(bola_x >= raquete_esq_y &&
               bola_x <= raquete_esq_y + 2)
                vy = -vy;
        }

        // raquete direita
        if(bola_y == 14)
        {
          // verificação se a bolinha colidiu com um LED aceso (raquete)
            if(bola_x >= raquete_dir_y && bola_x <= raquete_dir_y + 2)
                vy = -vy;
        }
    }

    // ponto → reinicia bola
    if(bola_y == 0)
    {
      // Realoca a bolinha no centro da tela
      bola_x = 3;
      bola_y = 7;
      vy = -vy;
      // Atribui os pontos ao jogador do lado oposto
      pontos_2++;
    }
    else if(bola_y == 15)
    {
      // Realoca a bolinha no centro da tela
      bola_x = 3;
      bola_y = 7;
      vy = -vy;
      // Atribui os pontos ao jogador do lado oposto
      pontos_1++;
    }
}

// loop_pong -> realiza a impressão da bolinha na tela a cada interrupção do timer
void loop_pong(void)
{
    if(jogo_pausado == 0)// Verifica se o jogo não está pausado
    {
        for(i = 0; i < 8; i++) // Limpa os vetores para uma nova rodada.
        {
            tela_esq[i] = 0;
            tela_dir[i] = 0;
        }

        // desenha raquetes com base na dificuldade
        if(dificuldade == 1)
        {
            desenha_raquete_esq_hard();
            desenha_raquete_dir_hard();
        }
        else
        {
            desenha_raquete_esq();
            desenha_raquete_dir();
        }

        desenha_bola();

        // imprime a bolinha na matriz de acordo com as posições passadas pelos vetores
        for(i = 0; i < 8; i++)
        {
            spi_max(MAX_DIGIT0 + i, tela_esq[i], MAX_DIGIT0 + i, tela_dir[i]);
        }

        // atualiza a posição da bolinha
        atualiza_bola();

        // Verifica se o jogo foi finalizado neste instante -> um dos 2 jogadores com 3+ pontos
        if(pontos_1 >= 3 || pontos_2 >= 3)
        {
            jogo_ativo = 0;
            TA1CTL &= ~MC0;
            jogo_comecando = 1;
            mostra_ponto(pontos_1, pontos_2);
        }
    }
    else if(botaoPres == 0)//Jogo foi pausado -> nada acontece
    {
        jogo_pausado = 1;
        botaoPres = 1;
    }
}
