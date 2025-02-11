import java.io.Serializable;

public class JogoDados implements Serializable{
    private Dado[] dados;//vetor que vai ser inicializado com o numero de dados necessarios para cada jogo
    private int qtdDados;//numero de dados do jogo
    
    public JogoDados(int qtd){//construtor
        this.qtdDados = qtd;//numero de dados
        dados = new Dado[this.qtdDados];//vetor inicializado com o numero de dados
        for(int i=0; i < dados.length; i++){
            dados[i] = new Dado();//inicializar a instancia da classe Dado para cada dado no vetor
        }
    }
    
    public void rolarDados(){//metodo para rolar dados
        for(int i = 0; i<dados.length;i++){
            dados[i].roll();//rolar todos os dados
        }
    }

    public int aplicarRegraJogo(int qualJogo){
        int aplicarRegraResultado = 0;
        switch (qualJogo) {//0 jogo de azar
            case 0:
                aplicarRegraResultado = aplicarRegraJogoAzar();
                break;
            case 1://1- jogo do porquinho
                aplicarRegraResultado = aplicarRegraJogoPorquinho();
                break;
            default:
                break;
        }
        return aplicarRegraResultado;//retornar resultado
    } 

    private int aplicarRegraJogoAzar(){ // -1 se perder 0-ganhou
        
        System.out.println("\n[Primeiro Lançamento jogo de Azar]");
        int j=1; 
        int sum = 0;
        for(int i = 0; i<dados.length;i++){ //rolar os dados
            dados[i].roll();
            sum += dados[i].getSideUp();//somar
        }
        
        if( sum == 7 || sum==11 ){ //se a soma for 7 ou 11 o jogador ganha
            System.out.println("Lançamento numero: "+ j); //numero do lancamento
            System.out.println(dados[0].getSideUp() + " + "+ dados[1].getSideUp() + " = "+ sum);
            System.out.println("Ganhou");
            return 0;
        }
        do{
            System.out.println("Lançamento numero: "+ j);
            
            //se a soma for 2,3, ou 12 o jogador perde
            if(dados[0].getSideUp()+dados[1].getSideUp() == 2 || dados[0].getSideUp()+dados[1].getSideUp() == 3 || dados[0].getSideUp()+dados[1].getSideUp()==12){
                System.out.println(dados[0].getSideUp() + "+"+ dados[1].getSideUp() + "="+ (dados[0].getSideUp()+dados[1].getSideUp()));
                System.out.println("Perdeu");
                return -1; //-1 derrota
            }else{
                System.out.println(dados[0].getSideUp() + "+"+ dados[1].getSideUp() + "="+ (dados[0].getSideUp()+dados[1].getSideUp()));
                if(j==1){
                    System.out.println("Numero a Ser Buscado: "+ sum);    
                }
                for(int i = 0; i<dados.length;i++){
                    dados[i].roll();//rolar os dados
                }
                j++;
                if(sum == dados[0].getSideUp()+dados[1].getSideUp()){ //se a nova soma for igual ao valor a ser procurado
                    System.out.println("Lançamento numero: "+ j);
                    System.out.println(dados[0].getSideUp() + "+"+ dados[1].getSideUp() + "="+ (dados[0].getSideUp()+dados[1].getSideUp()));
                    System.out.println("Ganhou");
                    return 0; //0-vitoria
                }
                
            }
            
        }while(j!=-1);

        return 0;
    }
     
        

    private int aplicarRegraJogoPorquinho(){
        System.out.println("\nJogo do Porquinho");
        int multiplicacao=0; //valor da multiplicacao que precisa chegar a 300
        int j=0;//numero de rodadas
        do{
            System.out.println("Rodada Numero: " + (j+1));
            multiplicacao += dados[0].getSideUp() * dados[1].getSideUp();//multiplicacao e somar
            System.out.println(dados[0].getSideUp() + " * " + dados[1].getSideUp() + " = " + (dados[0].getSideUp() * dados[1].getSideUp()) );
            System.out.println(multiplicacao);
            if((dados[0].getSideUp() == dados[1].getSideUp()) && (dados[0].getSideUp()!=1) ){//se for um numero duplicado
                multiplicacao += dados[0].getSideUp() * dados[1].getSideUp();//Somar de novo
                System.out.println("Elemento Duplicado(x2): " + multiplicacao);
            }else if((dados[0].getSideUp() == dados[1].getSideUp()) && (dados[0].getSideUp()==1)){//se for o numero 1 duplicado
                multiplicacao +=30;//somar 30
                System.out.println("Numero 1 Duplicado(+30): "+ multiplicacao); 
            }
            for(int i = 0; i<dados.length;i++){
                dados[i].roll();//rolar os dados de novo
            }
            
            j++;//incrementar o numero da rodada
        }while(multiplicacao<300);
        System.out.println("Numero de Rodadas: " + j);
        return j;//retornar o numero de rodadas necessaria para chegar a 300
    }

    public String toString(){
        return "Numero de Dados: " + qtdDados + ", lado de cima do dado: " + dados[0].getSideUp() + " e " + dados[1].getSideUp();
    }
    
    
}