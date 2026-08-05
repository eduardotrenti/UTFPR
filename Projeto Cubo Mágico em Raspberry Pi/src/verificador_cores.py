import cv2
import numpy as np
from picamera2 import Picamera2
import time
import drivers
from datetime import datetime
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)
btn1 = 4
btn2 = 26

GPIO.setup(btn1, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(btn2, GPIO.IN, pull_up_down=GPIO.PUD_UP)

def verificador_cores(cam: int=0) -> str:
    # Inicializacao e configuracao da camera
    display = drivers.Lcd()
    camera = Picamera2()
    camera.preview_configuration.main.size = (640, 480)
    camera.preview_configuration.main.format = "RGB888"
    camera.configure("preview")
    camera.start()
    time.sleep(2)
    

    # Definir posições dos 9 quadradinhos (x, y, tamanho)
    size = 5
    offset = 115
    start_x, start_y = 200, 120

    number_faces = 1
    confirmed_colors = []
    positions = []
    for row in range(3):
        for col in range(3):
            x = start_x + col * offset
            y = start_y + row * offset
            positions.append((x, y))
            
    i = 0
    
    # Captura de cores, pela camera
    while True:
        frame = camera.capture_array()
        

        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

        detected_colors = []

        # Desenhar os quadrados e capturar cores
        for (x, y) in positions:
            roi = hsv[y:y+size, x:x+size]
            mean_color = cv2.mean(roi)[:3]  # pega média (H,S,V)
            mean_color = tuple(map(int, mean_color))

            color_name = identify_color(mean_color)
            detected_colors.append(color_name)
            
            
            # Desenha o quadrado na tela
            cv2.rectangle(frame, (x, y), (x+size, y+size), (255, 255, 255), 2)
            cv2.putText(frame, color_name, (x, y-5), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,255,255), 1)
        
        cv2.imshow("Deteccao de Cubo Magico", frame) # Mostra a camera (NAO DEIXAR NOME COM CARACTERE ESPECIAL)
        display.lcd_display_string('  '+detected_colors[0]+' '+detected_colors[1]+' '+detected_colors[2], 1)
        display.lcd_display_string('  '+detected_colors[3]+' '+detected_colors[4]+' '+detected_colors[5], 2)
        display.lcd_display_string('  '+detected_colors[6]+' '+detected_colors[7]+' '+detected_colors[8], 3)
            

        key = cv2.waitKey(1) # chave
        
        # Finaliza captura ao pressionar chave
        if GPIO.input(btn2) == GPIO.LOW:
            break
            
        # Captura individual do lado ao pressionar chave
        elif GPIO.input(btn1) == GPIO.LOW:
            confirmed_colors.append(detected_colors)
            feedback = frame.copy()
            cv2.putText(feedback, f"FACE {number_faces}!", (300,30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255,255,0), 5, cv2.LINE_AA)
            
            number_faces += 1
            cv2.imshow("webcam", feedback) # Mostra a captura (NAO DEIXAR NOME COM CARACTERE ESPECIAL)
            time.sleep(0.1)
            
    print(confirmed_colors) # Verificacao
    
    # Ajuste dos quadrados centrais
    confirmed_colors[0][4] = "Y"
    confirmed_colors[1][4] = "G"
    confirmed_colors[2][4] = "R"
    confirmed_colors[3][4] = "W"
    confirmed_colors[4][4] = "B"
    confirmed_colors[5][4] = "O"
    
    print(confirmed_colors) # Verificacao
    
    display.lcd_clear()
    
    
        
            
                
    # Imprime o cubo no terminal
    number_faces = 1
    print(f"-------------- FACE {number_faces} --------------")
    print()
    for face in confirmed_colors:
        counter = 0
        for color in face:
            print(f"| {color} |", end='')
            counter += 1
            if counter == 3:
                print()
                counter = 0
        number_faces += 1
        print()
        print(f"-------------- FACE {number_faces} --------------")
        print()


    print(confirmed_colors)

    # Grava o cubo no arquivo
    with open('cores_capturadas.txt', 'w+', encoding='utf-8') as f:
        for face in confirmed_colors:
            for color in face:
                f.write(color)

    
    cv2.destroyAllWindows() #Fecha a janela de captura
    
    # Le o arquivo do cubo
    with open('cores_capturadas.txt', 'r+', encoding='utf-8') as f:
        colorString = f.readline()
    
    return colorString

# Função para identificar cor pelo valor HSV
def identify_color(hsv_pixel):
    h, s, v = hsv_pixel

    if s < 70 and v > 150:
        return "W"      # White - Branco
    if v < 50:
        return "P"      # Preto

    if (h >= 160 and h <= 180):
        return "R"      # Red - Vermelho
    if 0 < h <= 20:
        return "O"      # Orange - Laranja
    if 20 < h <= 35:
        return "Y"      # Yellow - Amarelo
    if 35 < h <= 85:
        return "G"      # Green - Verde
    if 85 < h <= 125:
        return "B"      # Blue - Azul

    return "D"          # Desconhecido

