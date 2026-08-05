import RPi.GPIO as GPIO
import cube_solver
import time

GPIO.setmode(GPIO.BCM)

btn = 4

GPIO.setup(btn, GPIO.IN, pull_up_down=GPIO.PUD_UP)

cube_solver.PINOUT()

i = 1
while True:
    if GPIO.input(btn) == GPIO.LOW:
        if i == 0:
            cube_solver.g2('B', 0)
            i = 1
        else:
            cube_solver.g2('B', 1)
            i = 0
        time.sleep(0.1)
