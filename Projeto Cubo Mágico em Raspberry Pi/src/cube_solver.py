import RPi.GPIO as GPIO
import time
import kociemba
import time
import drivers
from datetime import datetime

# Dicionario usado para decodificar a cor central para posicao 
COLOR_TO_POSITION = {
    'Y':'U',
    'R':'F',
    'W':'D',
    'G':'R',
    'B':'L',
    'O':'B'
}

# Traduzir cores registradas no verificador
COLOR_TRANSLATED = {
    'Amarelo':'Y',
    'Vermelho':'R',
    'Branco':'W',
    'Verde': 'G',
    'Azul':'B',
    'Laranja':'O'
}

# Pinagem Raspberry Pi 4
DIR = [20, 8, 15, 9, 27, 5]
STEP = [16, 25, 14, 10, 17, 0]
ENABLE = [21, 7, 18, 11, 22, 6]

# Parametros para motor de passo
steps = 50
delay = 0.001

# Seta os pinos de entrada e saida
def PINOUT():
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)

    for i in range(6):
        GPIO.setup(DIR[i], GPIO.OUT)
        GPIO.setup(STEP[i], GPIO.OUT)
        GPIO.setup(ENABLE[i], GPIO.OUT)
        

# Gira o motor
def girar(b: bool, motor):
	if motor >= 0 and motor <= 5:
		GPIO.output(ENABLE[motor], GPIO.LOW)
		if b:
			GPIO.output(DIR[motor], GPIO.LOW)  
		else:
			GPIO.output(DIR[motor], GPIO.HIGH)  
			
		for _ in range(steps):
			GPIO.output(STEP[motor], GPIO.HIGH)
			time.sleep(delay)
			GPIO.output(STEP[motor], GPIO.LOW)
			time.sleep(delay)
		GPIO.output(ENABLE[motor], GPIO.HIGH)
		
# Seleciona o motor a ser girado
def g2(mov, b): # 0 É ANTI-HORARIO
	if mov == 'R':
		girar(b, 2)
		
	elif mov == 'U':
		girar(b, 3)	
			
	elif mov == 'F':
		girar(b, 1)
		
	elif mov == 'D': # Motor de baixo (Esta invertido no circuito)
		girar(not(b), 4)
		
	elif mov == 'L':
		girar(b, 0)
		
	elif mov == 'B':
		girar(b, 5)

# Decodifica a string de comandos para movimento do motor
def resolver(solution):
        
    moves = solution.split()

    for move in moves:
        face = move[0]  # Ex: 'F', 'R', etc.
        

        if len(move) == 1:
            # Movimento simples: F
            g2(face, 1)
        elif len(move) == 2:
            if move[1] == '2':
                # Movimento duplo: F2
                g2(face, 1)
                g2(face, 1)
            elif move[1] == "'":
                # Movimento anti-horário: F'
                g2(face, 0)
            else:
                print("ERRO: movimento inválido:", move)
                break
        else:
            print("ERRO: movimento inválido:", move)
            break
            
    

        	


def convert_color_to_position(cubo: str) -> str:
    for chave, valor in COLOR_TO_POSITION.items():
        cubo = cubo.replace(chave, valor)
    return cubo

def solve(cubo: str) -> str:
    return kociemba.solve(cubo)
    

