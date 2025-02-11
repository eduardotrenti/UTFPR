import java.io.Serializable;

public class Dado implements Serializable{
    
    private int sideUp;//variavel com o valor de cima do dado
    
    public Dado(){
        this.sideUp = 0;
    }
    
    public void roll(){
        this.sideUp = (int)(Math.random()*6 + 1);// uso da biblioteca math, math.random(valor double entre 0 e 0,999), multiplicar o valor por 6 , somar 1, e converter para int
    }

    //metodos getters e setters necessarios

    public int getSideUp(){
        return this.sideUp;
    }

    public String toString(){
        return "O lado de cima do dado: " + this.sideUp;
    }
    
}