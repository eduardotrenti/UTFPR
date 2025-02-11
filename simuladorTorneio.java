import java.util.Scanner;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.File;


public class simuladorTorneio{
    
    //Cada torneio poderá ser iniciado desde que exista ao menos dois jogadores.

    public static void main(String[] args){
        menu();
    }

    public static void menu(){
        Torneio t = new Torneio();
        Scanner teclado = new Scanner(System.in);
        int n = -1;//variavel do sistema no qual vai o scanner vai ler
        int m = -1;// variavel para saber qual jogo sera escolhido 0-jogo de azar e 1-jogo do porquinho
        int qtdeJogadores = 0;//variavel para contar o numero de jogadores
        int jogoAcabou = -1;//variavel para caso tente gravar dados de um torneio que nao foi iniciado

        do{
            n = -1;//Resetando a Variavel das opcoes do menu
            m = -1;//Resetando a Variavel do jogo escolhido
            System.out.println("============ Torneio ===========");
            System.out.println("1 - Incluir jogador");
            System.out.println("2 - Remover jogador");
            System.out.println("3 - Iniciar Torneio");
            System.out.println("4 - Placar do torneio de apostas");
            System.out.println("5 - Gravar os dados do torneio em arquivo");
            System.out.println("6 - Ler os dados do torneio em arquivo");
            System.out.println("7 - Sair da aplicacao");
            do{
                //caso tente escrever algo que nao seja um numero, para nao encerrar o programa
                try{
                    n = teclado.nextInt();
                    teclado.nextLine();
                }catch(Exception ex){
                    System.out.println("Alternativa Invalida");
                    teclado.nextLine();//consumir escrita errada
                }
            }while(n==-1);//enquanto nao for digito alguma alternativa valida

            switch (n) {
                case 1://Incluit Jogador
                    if(qtdeJogadores<10){//caso tenha menos de 10 jogador
                        qtdeJogadores++;//aumentar o numero de jogador
                        t.incluirJogador();//chamar o metodo da classe torneio
                    }
                    else{//10 jogadores ou mais
                        System.out.println("O numero de jogadores atingiu o valor maximo!");
                        break;
                    }
                    break;
                case 2:
                    if(qtdeJogadores>0){//caso tenha jogador para remover
                        double idRemover = -1;
                        do{
                            System.out.println("Qual o ID do jogador?(double)");
                            try{
                                idRemover = teclado.nextDouble();
                                teclado.nextLine();
                            }catch(Exception ex){//Caso nao seja informado um double
                                System.out.println("Erro: numero invalido");
                                teclado.nextLine();//consumir linha errada
                            }
                        }while(idRemover == -1);//enquanto nao for digitado alternativa valida

                        
                        if(t.removerJogador(idRemover) == 1){//chamar metodo da classe torneio, e se a remocao for um sucesso
                            qtdeJogadores--;//decrementar a quantidade de jogador
                        }
                    }else{//caso nao tenha jogadores o suficiente para remover
                        System.out.println("Sem Jogadores para Remover");
                    }

                    
                    
                    break;
                case 3://iniciar torneio
                if(qtdeJogadores>=2){//precisa ter jogadores suficientes, o metodo incluir jogador ja valida caso tente ter mais de 10 jogadores
                        do{
                            System.out.println("Qual jogo?");
                            System.out.println("0-Jogo de Azar ou 1-Jogo do Porquinho:");
                            try{//caso algo alem de int seja digito, para que o programa nao termine
                                m = teclado.nextInt();
                                teclado.nextLine();
                                if(m!=0 && m!=1){
                                    System.out.println("Jogo Invalido");
                                }
                            }catch(Exception ex){
                                System.out.println("Jogo Invalido");
                                teclado.nextLine(); // consumir linha invalida se for o caso
                            }
                        }while(m!=0 && m!=1);//enquanto nao for uma das opcoes

                        t.iniciarTorneio(m);//chamar metodo da classe torneio para iniciar o torneio
                        jogoAcabou = 0;//variavel para saber se o jogo acabau, para gravar dados em arquivo
                    }
                    break;
                case 4://mostrar placar do torneio
                    if(jogoAcabou!=-1){//se ocorreu um torneio para ter dados para mostrar
                        t.mostrarPlacaFinal();// chamar metodo da classe torneio para mostrar o placar
                    }else{//caso um torneio nao tenha sido iniciado antes para ter placar
                        System.out.println("Erro: Inicie Um Jogo Primeiro");
                    }
                    break;
                case 5:
                    if(jogoAcabou!=-1){//se ocorreu um torneio para ter dados para gravar
                        gravarTorneioArquivo(t);//chamar metodo para gravar dados
                    }else{
                        System.out.println("Torneio Nao Iniciado");
                    }
                    break;
                case 6://ler dados de arquivo
                    lerTorneioArquivo();//chamar leitura de dados do ultimo torneio gravado
                    
                    break;

                case 7://sair da aplicacao
                    System.out.println("Saindo da Aplicação: ....");
                    break;
                default:
                    System.out.println("Opcao Invalida. Tente Novamente");
                    break;
            }

        }while(n!=7);
        teclado.close();//fechar scanner
    }
    public static void gravarTorneioArquivo(Torneio t){
        File arq = new File("t.dat"); //arquivo "t.dat"
        try{
            FileOutputStream fout = new FileOutputStream(arq);
            ObjectOutputStream oos = new ObjectOutputStream (fout);
            oos.writeObject(t);//escrever a toString do torneio no arquivo
            oos.flush(); 
            oos.close();
            fout.close();
            System.out.println("Dados do Torneio Gravado em Arquivo");
        }catch(Exception ex){
            System.err.println("erro: " + ex.toString());
        }
    }

    public static void lerTorneioArquivo(){
        File arq = new File("t.dat");//arquivo "t.dat"
        try{
            FileInputStream fin = new FileInputStream(arq);
            ObjectInputStream oin = new ObjectInputStream (fin) ;
            Torneio torneioLerArquivo = (Torneio) oin.readObject();//novo torneio para leitura 
            oin.close();
            fin.close();
            System.out.println(torneioLerArquivo);//toString do torneio
        }catch(Exception ex){
            System.err.println("erro: " + ex.toString());
        }
    }

}

