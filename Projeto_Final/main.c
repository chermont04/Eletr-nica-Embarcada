#include <msp430G2553.h>
#include <msp430.h>

// Definindo os LEDs e botões usados para parar o timer e pegar o número aleatório.
#define RED_LED         BIT1                        // Red LED          (P1.1)
#define GREEN_LED       BIT2                        // Green LED        (P1.2)
#define BLUE_LED        BIT4                        // Blue LED         (P1.4)
#define YELLOW_LED      BIT5                        // Yellow LED       (P1.5)

#define RED_BTN         BIT1                        // Red button       (P2.1)
#define GREEN_BTN       BIT2                        // Green button     (P2.2)
#define BLUE_BTN        BIT4                        // Blue button      (P2.4)
#define YELLOW_BTN      BIT5                        // Yellow button    (P2.5)

// Tamanhos de pausas diferentes para a função wait()
#define TEN_MS      1
#define CENTI_SEC   12
#define QUART_SEC   30
#define HALF_SEC    60
#define ONE_SEC     120
#define BLINK       50
#define PAUSE       30

// Timers
#define ENABLE_PINS 0xFFFE
#define ACLK        0x0100
#define SMCLK       0x0200
#define CONTINUOUS  0x0020
#define UP          0x0010

// Numero de rounds para acabar o jogo de acordo com a dificuldade
#define EASY            8
#define NORMAL          10
#define HARD            12
#define EXTREME         16

// Funções para o funcionamento do game
void Reset(void);
int ChooseDifficulty(void);
void Wait(int t);

int GetFirstNumber(void);
int GetSecondNumber(void);
void MakeSequence(int sequence[16], int first_number, int second_number);

void BlinkLeds(int sequence[16], int round);
int GetAnswer(int sequence[16], int round);
void CorrectAnswer(void);

void Win(void);
void Loss(void);

void main(void)
{
    // desativa o watch dog timer.
    WDTCTL  = WDTPW | WDTHOLD;

    // inicia o timerA0 com ACLK, contando até 10ms.
    TA0CTL |= ACLK | UP;
    TA0CCR0 = 400;

    // Inicia o Timer A1 com SMCLK.
    TA0CTL |= ACLK | CONTINUOUS; // aqui
    TA1CTL |= SMCLK | CONTINUOUS;

    // habilita os leds como saída e os botões como entrada.
    P1DIR |= BLUE_LED | YELLOW_LED | RED_LED | GREEN_LED;
    P1OUT &= (~RED_LED);
    P1OUT &= (~BLUE_LED);
    P1OUT &= (~GREEN_LED);
    P1OUT &= (~YELLOW_LED);
    P2OUT |= YELLOW_BTN;
    P2REN |= YELLOW_BTN;
    P2OUT |= RED_BTN;
    P2REN |= RED_BTN;
    P2OUT |= GREEN_BTN;
    P2REN |= GREEN_BTN;
    P2OUT |= BLUE_BTN;
    P2REN |= BLUE_BTN;

    while(1)
    {
        // espera o jogador iniciar o jogo para então gerar o numero randomico.
        int first_number = 0;
        Reset();
        first_number = GetFirstNumber();
        Wait(QUART_SEC);
        // espera o jogador selecionar a dificuldade.
        int difficulty;
        int second_number = 0;
        difficulty = ChooseDifficulty();
        second_number = GetSecondNumber();
        // Preenche o array com uma combinação de dois números aleatórios.
        int sequence[16] = {0.0};
        MakeSequence(sequence, first_number, second_number);
        int game_state = 1;
        int round = 0;
        while(game_state == 1)
        {
            Wait(ONE_SEC);
            BlinkLeds(sequence, round);

            // Espera o jogador prescionar o botão e verifica se é o correto.
            Wait(TEN_MS);
            game_state = GetAnswer(sequence, round);
            Wait(TEN_MS);

            // Se a resposta for correta, pisca o verde e segue para a proxima.
            if (game_state == 1)
            {
                CorrectAnswer();
                round++;
            }
            Wait(TEN_MS);

            // Ao se terminar o maximo de rounds permitidos pela dificuldade, os leds piscaram indicando vitoria.
            if (round == difficulty)
            {
                game_state = 2;
            }
            Wait(TEN_MS);
        }
        if (game_state == 2)
        {
            Win();
        }
        // If not, then the loop quit because game_state == 0; show a loss
        else
        {
            Loss();
        }
    }
}

void Reset(void)
{
    P1OUT |= RED_LED;
    int x = 0;
    while (x < 1)
    {
        if ((P2IN & RED_BTN) == 0)
        {
            P1OUT &= (~RED_LED);
            x = 1;
        }
    }
}

/******************************************************************************
/   Mostra para o jogador 3 cores de LEDs (Verdem, azul e amarelo); Eles podem prescionar
/   o botão correspondente para selecionar a dificuldade, que decide o número de
/   rounds que devem ser cumpridos para ganhar o jogo, assim como a velocidade de amostragem.
******************************************************************************/
int ChooseDifficulty(void)
{
    P1OUT |= (BLUE_LED | YELLOW_LED | GREEN_LED);
    int x = 0;
    int i;
    while (x < 1)
    {
        if ((P2IN & GREEN_BTN) == 0) // -------------------------------------------------------------------------------------
        {
            P1OUT &= (~GREEN_LED);
            P1OUT &= (~(BLUE_LED | YELLOW_LED));
            x = 3;
            for (i = 0; i < 8; i++)
            {
                P1OUT = (P1OUT ^ GREEN_LED);
                Wait(CENTI_SEC);
            }
        }
        if ((P2IN & BLUE_BTN) == 0) // -------------------------------------------------------------------------------------
        {
            P1OUT &= (~GREEN_LED);
            P1OUT &= (~(BLUE_LED | YELLOW_LED));
            x = 4;
            for (i = 0; i < 8; i++)
            {
                P1OUT = (P1OUT ^ BLUE_LED);
                Wait(CENTI_SEC);
            }
        }
        if ((P2IN & YELLOW_BTN) == 0) // -------------------------------------------------------------------------------------
        {
            P1OUT &= (~GREEN_LED);
            P1OUT &= (~(BLUE_LED | YELLOW_LED));
            x = 5;
            for (i = 0; i < 8; i++)
            {
                P1OUT = (P1OUT ^ YELLOW_LED);
                Wait(CENTI_SEC);
            }
        }
        if ((P2IN & RED_BTN) == 0) // -------------------------------------------------------------------------------------
        {
            P1OUT &= (~GREEN_LED);
            P1OUT &= (~(BLUE_LED | YELLOW_LED));
            x = 9;
            for (i = 0; i < 8; i++)
            {
                P1OUT = (P1OUT ^ RED_LED);
                Wait(CENTI_SEC);
            }
        }
    }
    return x;
}

void Wait(int t)
{
    int i = 0;
    // enquanto a contagem não atingir o limite:
    while (i <= t)
    {   // quando outros 10 ms tiverem passado.
        if (TA0CTL & TAIFG)
        {
            // aumenta o contador e conta novamente 10 ms.
            i++;
            TA0CTL &= (~TAIFG);
        }
    }
}

int GetFirstNumber(void)
{
    int first_num = 0;
    first_num = TA0R;
    return first_num;
}

int GetSecondNumber(void)
{
    int second_num = 0;
    second_num = TA1R;
    return second_num;
}

// cira uma sequencia semi aleatória usando os dois arrays criados anteriormente.
void MakeSequence(int sequence[8], int first_number, int second_number)
{
    int i;
    int first_array[16] = {0.0};
    int second_array[16] = {0.0};
    for (i = 0; i < 16; i++)
    {
        first_array[(15 - i)] = ((first_number >> i) & 0x01);
        second_array[(15 - i)] = ((second_number >> i) & 0x01);
    }
    for (i = 0; i < 2; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 1);
    }
    for (i = 2; i < 3; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 3);
    }
    for (i = 3; i < 4; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 4);
    }
    for (i = 4; i < 5; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 1);
    }
    for (i = 5; i < 6; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 3);
    }
    for (i = 6; i < 7; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 4);
    }
    for (i = 7; i < 8; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 4);
    }
    for (i = 8; i < 9; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 1);
    }
    for (i = 9; i < 10; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 3);
    }
    for (i = 10; i < 11; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 4);
    }
    for (i = 11; i < 12; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 3);
    }
    for (i = 12; i < 13; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 1);
    }
    for (i = 13; i < 14; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 3);
    }
    for (i = 14; i < 15; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 1);
    }
    for (i = 15; i < 16; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 4);
    }
}

void BlinkLeds(int sequence[16], int round)
{
    int i = 0;
    do
    {
        switch (sequence[i])
        {
        case (0):   P1OUT |= RED_LED;
                    Wait(BLINK);
                    P1OUT &= (~RED_LED);
                    Wait(PAUSE);
                    break;

        case (1):   P1OUT |= GREEN_LED;
                    Wait(BLINK);
                    P1OUT &= (~GREEN_LED);
                    Wait(PAUSE);
                    break;

        case (3):   P1OUT |= BLUE_LED;
                    Wait(BLINK);
                    P1OUT &= (~BLUE_LED);
                    Wait(PAUSE);
                    break;

        case (4):   P1OUT |= YELLOW_LED;
                    Wait(BLINK);
                    P1OUT &= (~YELLOW_LED);
                    Wait(PAUSE);
                    break;
        }

        i = i + 1;

    }
    while (i <= round);
}

/******************************************************************************
/ Essa função espera o jogador aplicar uma resposta para depois julga-la.
/
/ Isto é feito esperando o jogador aplicar uma resposta e depois verificando se
/ se é a entrada certa para o elemento i da sequencia do array, em que i é o
/ n-nesimo LED mostrado.
******************************************************************************/
int GetAnswer(int sequence[16], int round)
{
    int i = 0;
    int game_over = 0;
    // Permanece no loop até que uma resposta errada seja emitida.
    while (i <= round && game_over == 0)
    {
    //    Wait(HALF_SEC);
        if ((P2IN & RED_BTN) == 0)
        {
            P1OUT |= RED_LED;
            if (sequence[i] == 0)
                i++;
            else
                game_over = 1;
            Wait(QUART_SEC);
            P1OUT &= (~RED_LED);
        }
        if ((P2IN & GREEN_BTN) == 0)
        {
            P1OUT |= GREEN_LED;
            if (sequence[i] == 1)
                i++;
            else
                game_over = 1;
            Wait(QUART_SEC);
            P1OUT &= (~GREEN_LED);
        }
        if ((P2IN & BLUE_BTN) == 0)
        {
            P1OUT |= BLUE_LED;
            if (sequence[i] == 3)
                i++;
            else
                game_over = 1;
            Wait(QUART_SEC);
            P1OUT &= (~BLUE_LED);
        }
        if ((P2IN & YELLOW_BTN) == 0)
        {
            P1OUT |= YELLOW_LED;
            if (sequence[i] == 4)
                i++;
            else
                game_over = 1;
            Wait(QUART_SEC);
            P1OUT &= (~YELLOW_LED);
        }
    }
    // Se a resposta for errada, envia um sinal para a função main() para mostrar
    // Loss().
    if (game_over == 0)
        return 1;
    // Caso contrário, envia um sinal para a função main() para indicar que a
    // resposta foi correta e continuar.
    else
        return 0;
}


/******************************************************************************
/   Essa função pisca o LED verde 8 vezes bem rapido indicando que a sequencia
/   inserida é correta.
******************************************************************************/
void CorrectAnswer(void)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        P2OUT = (P2OUT ^ GREEN_LED);
        Wait(CENTI_SEC);
    }
}

/******************************************************************************
/   Essa função faz com que os LEDs pisquem rapido indicando a vitória
******************************************************************************/
void Win(void)
{
    int i;
    for (i = 0; i < 3; i++)
    {
        P1OUT |= RED_LED;
        Wait(CENTI_SEC);
        P1OUT &= (~RED_LED);
        Wait(CENTI_SEC);
        P1OUT |= GREEN_LED;
        Wait(CENTI_SEC);
        P1OUT &= (~GREEN_LED);
        Wait(CENTI_SEC);
        P1OUT |= BLUE_LED;
        Wait(CENTI_SEC);
        P1OUT &= (~BLUE_LED);
        Wait(CENTI_SEC);
        P1OUT |= YELLOW_LED;
        Wait(CENTI_SEC);
        P1OUT &= (~YELLOW_LED);
        Wait(CENTI_SEC);
    }
}

/******************************************************************************
/ Essa função faz com que o LED vermelho pisque 3 vezes devagar indicando a derrota
******************************************************************************/
void Loss(void)
{
    int i;
    for (i = 0; i < 3; i++)
    {
        P1OUT |= RED_LED;
        Wait(QUART_SEC);
        P1OUT &= (~RED_LED);
        Wait(QUART_SEC);
    }
    Wait(ONE_SEC);
}
