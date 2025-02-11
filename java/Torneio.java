import java.util.Scanner;
import java.io.Serializable;


public class Torneio implements Serializable{
    private Jogador[] jogadores;//vetor dos jogadores
    private int qtdeJogadores;//quantidade de jogadores

    private int[] ganhadores; // vetor para caso seja o jogo do porquinho, consiga pegar o jogador com o menor numero de rodadas
    private int rodadas;//variavel do numero de rodadas

    public Torneio(){
        jogadores = new Jogador[10];//inicializando o vetor dos jogadores como 10(maximo)
        ganhadores = new int[10];//inicializando o vetor dos resultados
        for(int i=0;i<10;i++){ 
            ganhadores[i] = 0;
        }
        qtdeJogadores = 0;
        rodadas = 0;
    } 

    public void incluirJogador(){
        int m=-1;//variavel para saber se o jogador vai ser maquina ou humano
        Scanner teclado = new Scanner(System.in);
        do{
            System.out.println("Tipo Maquina(0) ou Humano(1)?");
            try{//caso tenha tentativa de digitar algo que nao seja numero o programa nao encerrar
                m = teclado.nextInt();
                teclado.nextLine();
                if(m!=0 && m!=1){
                    System.out.println("Erro: Escolha Invalida");
                }
            }catch(Exception ex){
                System.out.println("Erro: Escolha Invalida");
                teclado.nextLine();//consumir linha invalida
            }

        }while(m!=0 && m!=1);//transformar em boolean

        boolean tipo;
        if(m==0){//se for maquina o valor muda para false
            tipo = false;
        }else{//se for humano o valar muda para true
            tipo = true;
        }


        double id = -1;//variavel do id
        do{//primeiro do-while para verificar se nao existe um jogaor com esse id ja 
            do{//segundo do-while para validar se o id nao é um numero
                System.out.println("Qual o ID?(double)");
                try{
                    id = teclado.nextDouble();
                    teclado.nextLine();
                }catch(Exception ex){
                    System.out.println("Erro: Alternativa Invalida(somente double)");   
                    teclado.nextLine();
                }
            }while(id == -1);//ate o id ser um numero
            
            for(int i=0;i<10;i++){
                if((jogadores[i]!=null) && (id == jogadores[i].getId())){//se ja existe um jogadore com esse id continuar do loop
                    System.out.println("Id ja existe, tente novamente");
                    id = -1;
                }
            }
        }while(id == -1);
        
        for(int i = 0;i<10;i++){//adicionar jogador na posicao do primeiro null, caso tenha uma remocao de um jogador
            if(jogadores[i] == null){
                jogadores[i] = new Jogador(id,tipo);//criar jogador com o construtor de dois parametros
                qtdeJogadores++;//incrementar a variavel da quantidade de jogador
                break;
            }
        }

    }

    public int removerJogador(double idRemover){
        int j=0;
        for( j=0;j<10;j++){
            if( (jogadores[j] != null) && (jogadores[j].getId() == idRemover) ){//se o id do jogador for igual ao desejado para remover
                System.out.println("Jogador de id = " + jogadores[j].getId() + " removido");
                this.jogadores[j] = null;//jogador removido fica null
                this.qtdeJogadores--;//decremento da variavel da quantidade de jogadores
                return 1;// 1 - operacao concluida
            }
        }
        System.out.println("Jogador Procurado nao existente!");//caso nao tenha jogador com o id informado
        return 0;// 0 - operacao nao concluida
        
    }

    public void iniciarTorneio(int qtdDados){
        System.out.println("Iniciando Torneio");
        double valorApostaTotal = 0; // variavel para somar as apostas
        int numPlayer = qtdeJogadores; // criterio de parada (numero de jogadores com saldo positivo)
        int j;

        for(int i = 0;i<10;i++){
            if(jogadores[i]!=null){//para todos os jogadores que irao jogador o torneio, resetar os valores caso ocorra mais de um torneio
                jogadores[i].setSaldo(100); //Caso tenha mais de um torneio resetar o valor do saldo para 100
                jogadores[i].setValorAposta(0);//Caso tenha mais de um torneio resetar o valor da ultima aposta para 0
            }
        }
        
        for( j=0;numPlayer>1;j++){//enquanto o numero de jogadores com saldo positivo for > 1 
            this.rodadas++;//encremento no numero de rodadas
            valorApostaTotal = 0;   // resetando o valor da soma das apostas para cada rodada
            System.out.println("\n========= Rodada Numero: " + (j+1)+ " ============");
            int i;
            for(i=0;i<10;i++){                                                   
                if((jogadores[i]!=null) && (((int)jogadores[i].getSaldo())>0) ){//definir as apostas e somar para cada jogador ainda com saldo positivo      
                    System.out.println(jogadores[i]);                           // Definindo o Valor da Aposta  
                    valorApostaTotal += jogadores[i].apostar();                 //  e Somar para atribuir na variavel
                }
            }

            if(qtdDados == 0){ // 0-jogo de azar
                torneioAzar(qtdDados,valorApostaTotal);
            }else{// 1-jogo do porquinho
                torneioPorco(qtdDados, valorApostaTotal);
            }
            
            
            System.out.println("=========== Rodada Numero : " + (j+1) + " finalizada =============\n");
            
            mostrarFinalizaoRodada(); // mostra os dados dos jogadores com saldo positivo

            numPlayer = qtdeJogadores;//fazer a verificacao de jogadores com saldo positivo e negativo
            for(i=0;i<10;i++){
                if((jogadores[i]!=null) && ( ((int)jogadores[i].getSaldo()) <= 0)){//mostrar jogadores com saldo negativo
                    System.out.println("\njogador de id: " + jogadores[i].getId()+ " ficou com saldo negativo");
                    numPlayer--;//diminuir a variavel
                }   

            }

        }

        //Se quiser ver o placar final, chamar na opcao 4 do sistema
        //mostrarPlacaFinal(j+1);

        //mostrarPlacarFinal
    }

    private void torneioAzar(int qtdDados, double valorApostaTotal){//torneio de azar
        int contGanhadores=0; // variavel para saber em quantos dividor o valor da aposta caso ocorra mais de um vencedor
        
        for( int i=0;i<10;i++){
            if( (jogadores[i]!=null) && (((int)jogadores[i].getSaldo())>0) ){//fazer os lancamentos para os jogadores com saldo positivo
                System.out.println("\n");
                System.out.println(jogadores[i]);//mostrar dados dos jogadores antes do jogo
                ganhadores[i] = jogadores[i].mostrarLancamentosJogo(qtdDados,valorApostaTotal);
                System.out.println(jogadores[i]);//mostrar dados os jogadores depois do jogo
                if(ganhadores[i] == 0){ //se o jogador ganhou aumenta o numero de ganhadores
                    contGanhadores++;
                }
            }
        }
        if(contGanhadores!=0){//nao ter divisao por 0
            valorApostaTotal /= contGanhadores;//dividindo o valor da aposta entre o numero de vencedores
        }
        
        System.out.println("Jogadores Vencedores da Rodada: ");
        for(int  i=0; i<10 ;i++){
            if((jogadores[i]!=null) && (ganhadores[i]==0) && (((int)jogadores[i].getSaldo())>0) ){ // se o jogador ganhou, ganhadores = 0
                System.out.println(jogadores[i].getId()); // printar id dos jogadores vencedores         
                jogadores[i].setSaldo(jogadores[i].getSaldo() - jogadores[i].getValorAposta() + valorApostaTotal);//atualizar o valor os jogadores vencedores, novo saldo = saldo antigo - valor apostado + valor ganho
            }else if( (jogadores[i]!= null) && (((int)jogadores[i].getSaldo())>0)){//se o jogador tem saldo positivo e perdeu
                jogadores[i].setSaldo(jogadores[i].getSaldo() - jogadores[i].getValorAposta());//atualizar o valor dos jogadores perdedores com saldo positivo
            }
        }
        
    }
    
    private void torneioPorco(int qtdDados, double valorApostaTotal){//torneio do porco
        int contGanhadores = 0; // variavel para saber em quantos dividor o valor da aposta caso ocorra mais de um vencedor
        int menosRodadas = 0;//variavel para pegar o(s) jogador(s) com menor numero de rodadas
        int primeirocaso = 0;//variavel para saber o primeiro jogador que jogou e usar como o primeiro valor para o menor numero de rodadas
        for( int i=0;i<10;i++){
            if( (jogadores[i]!=null) && (((int)jogadores[i].getSaldo()) > 0 ) ){//jogador com saldo positivo
                System.out.println("\n");
                System.out.println(jogadores[i]);//mostrar dados dos jogadores com saldo positvo antes do jogo
                ganhadores[i] = jogadores[i].mostrarLancamentosJogo(qtdDados,valorApostaTotal); //armazenar o numero de rodadas para chegar a 300 no jogo do porquinho
                primeirocaso++;//saber o primeiro caso de jogador que jogou
                if(primeirocaso==1){ // usar o valor do primeiro jogador que jogou para usar como referencia 
                    menosRodadas = ganhadores[i];
                }else if(ganhadores[i]<menosRodadas){ // achar o menor numero de rodadas no jogo do porquinho
                    menosRodadas = ganhadores[i];//atualizar o menor numero de rodadas
                }
            }
 
        }

        for( int i=0;i<10;i++){
            if((jogadores[i]!=null )&& (ganhadores[i] == menosRodadas) && ( ((int)jogadores[i].getSaldo()) > 0 )){ 
                contGanhadores++;// saber o numero de ganahdores com saldo positivo para dividir o premio igualmente caso tenha empate
            }
        }

        if(contGanhadores != 0){ //para nao ter divisao por 0
            valorApostaTotal /= contGanhadores;//divir o valor igualmente no caso de empate
        }

        
        System.out.println("Jogadores Vencedores da Rodada:");
        for(int i=0; i<10 ;i++){
            if((jogadores[i]!=null) && ((ganhadores[i]==menosRodadas)) &&  ( ((int)jogadores[i].getSaldo()) > 0 ) ){ //se o jogador fez o jogo do porquinho com o menor numero de rodadas
                System.out.println(jogadores[i].getId());//mostrar vencedores
                jogadores[i].setSaldo(jogadores[i].getSaldo() - jogadores[i].getValorAposta() + valorApostaTotal);// incrementando o valor da aposta no jogador vencedor, novo saldo = saldo antigo - valor apostado + valor ganho
            }else if((jogadores[i]!= null) &&  ( ((int)jogadores[i].getSaldo()) > 0 )){//se o jogador perdeu
                jogadores[i].setSaldo(jogadores[i].getSaldo() - jogadores[i].getValorAposta()); //decrementar valor, novo saldo = saldo antigo - valor apostado
            }
        }

    }

    // funcao para mostrar o jogador que venceu, e em quantas rodadas    
    public void mostrarPlacaFinal(){
        int i,m=0;
        for(i=0;i<10;i++){
            if((jogadores[i]!=null) && (((int)jogadores[i].getSaldo())>0) ){//mostrar jogador que venceu e em quantas rodadas
                System.out.println("\n ====== ID DO JOGADOR VENCEDOR: " + jogadores[i].getId() + ", NUMERO DE RODADAS "+ this.rodadas + " ======");
                m=1;
            }
        }
        if(m==0){
            System.out.println("\nNenhum Vencedor!"); //caso os jogadores restantes percam na mesma rodada no jogo do azar

        }
    }

    //SOBREGARGA no metodo mostarFinalizaoRodada da classe Torneio, com o numero da rodada ou sem

    // mostrar os dados dos jogadores ainda no torneio e o numero da rodada, com o numero de rodadas
    public void mostrarFinalizaoRodada(int rodadas){
        System.out.println("===== Rodada " + rodadas + " finalizada ======");//metodo da sobrecarga mostrarndo o numero da rodada
        for(int i=0;i<10;i++){
            if( (jogadores[i]!= null) && ( ((int)jogadores[i].getSaldo()) > 0 ) ){
                System.out.println(jogadores[i]);
            }
        }
    }

    //mostrar os dados dos jogadores ainda no torneio, sem o numero de rodadas
    public void mostrarFinalizaoRodada(){
        for(int i=0;i<10;i++){
            if( (jogadores[i]!= null) && ( ((int)jogadores[i].getSaldo()) > 0 ) ){
                System.out.println(jogadores[i]);
            }
        }
    }

    //metodos getters e setters necessarios

    public int getRodadas() {
        return rodadas;
    }
    

    //metodo toString para usar na hora de gravar os dados do torneio em arquivo 
    public String toString() {
        String dadoTorneio = " ";
        for(int i = 0;i<10;i++){
            if(jogadores[i]!=null){
                dadoTorneio = dadoTorneio + jogadores[i].toString() + "\n" + "\n";
            }
        }
        return dadoTorneio;
    }

}

