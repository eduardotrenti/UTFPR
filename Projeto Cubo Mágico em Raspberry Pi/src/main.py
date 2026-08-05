import verificador_cores
import cube_solver
import cv2
import time
import drivers
from datetime import datetime

if __name__ == '__main__':
    
    display = drivers.Lcd()
    display.lcd_display_string('BEM VINDO', 1)
    display.lcd_display_string('AO RESOLVEDOR DE', 2)
    display.lcd_display_string('CUBO MAGICO', 3)
    
    time.sleep(3)
    
    cube_solver.PINOUT()
    
    display.lcd_clear()
    
    gerada = verificador_cores.verificador_cores()
    
    
    if len(gerada) == 54:
        
        display.lcd_display_string("Resolvendo...", 1)
        start_time = datetime.now()
        
        cuboC = cube_solver.convert_color_to_position(gerada)
        
        solution = cube_solver.solve(cuboC)
        
        cube_solver.resolver(solution)
        
        elapsed_time = (datetime.now() - start_time).total_seconds()
        display.lcd_clear()
        display.lcd_display_string(f"Tempo: {elapsed_time:.2f}s", 2)
        time.sleep(3)
        display.lcd_clear()
