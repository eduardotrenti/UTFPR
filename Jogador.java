
import java.util.Scanner;
import java.io.Serializable;


public class Jogador implements Serializable{
    private final double id; // atribut final pois nao ira mudar, ja que o id sempre sera o mesmo
    private JogoDados jogo;//instancia da classe JogoDados para chamar o algoritmo de azar
    private double saldo;//saldo do jogador, no inicio de cada torneio fica 100 por padrao
    private double valorAposta; //valor da aposta na rodada
    private int qtdDados;//variavel para saber se e 0-azar e 1-porquinho
    private boolean tipo;  // 1-true- jogador   0-false-maquina
    //Cada jogador possuirá id (literal ou numérico), tipo (humano ou máquina), saldo, valor de aposta e uma instância de jogo.

    public Jogador(){//construtor vazio
        this.id = 0;
        this.saldo = 100;
        this.tipo = true;
        this.jogo = new JogoDados(2);
        this.qtdDados = 2;
    }

    public Jogador(double id, boolean tipo){//construtor com parametros
        this.id = id;//id informado 
        this.saldo = 100;//saldo padrao 100
        this.tipo = tipo;//jogo de azar ou porquinho
        this.jogo = new JogoDados(2);//2 dados pois os dois jogos escolhidos usam dois dados
        this.qtdDados = 2;//2 dados pois os dois jogos escolhidos usam dois dados;
    }
    
    public void rolarDados(int qualJogo, double somaAposta){//metodo que usa a instancia da classe JogoDados para rolar os dados
        jogo.rolarDados();
    }

    public double apostar(){
        if(tipo){//tipo true-jogador(requisitar o valor da aposta) false-maquina(saldo/5)

            Scanner teclado = new Scanner(System.in);
            double qtdAposta = 0;//variavel para armazenar o valor apostado
            do{
                System.out.println("Qual o valor da aposta do jogador "+ id +"?");
                try{//caso haja escrita de algo que nao seja um numero
                    qtdAposta = teclado.nextDouble();
                    teclado.nextLine();
                    if(qtdAposta<=0 || qtdAposta>saldo){//valor ou 0 ou maior que o saldo
                        System.out.println("Valor invalido");
                    }
                }catch(Exception ex){
                    System.out.println("Valor invalido");
                    teclado.nextLine();//consumo da linha que nao eh um numero
                }
            }while(qtdAposta<=0 || qtdAposta>this.saldo);//enquanto o valor nao for valido
            this.valorAposta = qtdAposta;//atualizar o valor da aposta
        }else{//caso seja maquina
            this.valorAposta = this.saldo/5;//atualizar o valor, com o padrao da divisao da maquinha
        }
        
        return valorAposta;//retornar o valor da aposta para que tenha a soma
    }

    public int mostrarLancamentosJogo(int qualJogo,double somaAposta){//metodo que vai usar a instancia da classe JogoDados para conector com algoritmo dos jogos de azar
        jogo.rolarDados();
        int resultado = jogo.aplicarRegraJogo(qualJogo);//armazenar resultado, -1-jogo de azar(derrota),0-jogo de azar(vitoria), 1+jogo do porquinho(numero de rodadas)                           
        if( (resultado == -1) && (qualJogo == 0)){//jogo de azar-derrota
            return -1;
        }else if((resultado == 0) && (qualJogo == 0)){//jogo de azar-vitoria                    
            return 0;
        }else{
            return resultado;//jogo do porquinho-numero de rodadas
        }
    }

    //metodos getters e setters necessarios
    
    public int getQtdDados(){
        return qtdDados;
    }

    public void setQtdDados(int qtdDados) {
        this.qtdDados = qtdDados;
    }

    public void setSaldo(double saldo) {
        this.saldo = saldo;
    }

    public double getSaldo() {
        return saldo;
    }

    public double getId() {
        return id;
    }

    public void setValorAposta(double valorAposta) {
        this.valorAposta = valorAposta;
    }
    public double getValorAposta() {
        return valorAposta;
    }

    public boolean getTipo() {
        return this.tipo;
    }
    
    
    public String toString(){
        return "Jogador: "+ this.id + "\nsaldo: " + this.saldo + "\nvalor da ultima aposta: " + this.valorAposta;
    }

}