#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int eh_numero(char *s){
    if (s==NULL||*s=='\0') return 0;
    for (int i=0;s[i] != '\0'; i++){
        if(s[i] <'0' || s[i] > '9') return 0;
    }
    return 1;
}

int eh_seguro(int *available, int **allocation, int **need, int quantidade_de_clientes, int quantidade_de_recursos){
    int *trabalho = (int*) malloc(quantidade_de_recursos *sizeof(int));
    int *terminado = (int*) calloc(quantidade_de_clientes, sizeof(int));

    for (int i=0;i<quantidade_de_recursos;i++){
        trabalho[i] = available[i];
    }

    int contador = 0;
    while (contador < quantidade_de_clientes){
        int encontrou = 0;

        for (int p=0; p<quantidade_de_clientes; p++){
            if (terminado[p] ==0){
                int j;
                for (j=0; j<quantidade_de_recursos; j++){
                    if (need[p][j]>trabalho[j]){
                        break;
                    }
                }
                if (j==quantidade_de_recursos){
                    for (int k=0;k<quantidade_de_recursos;k++){
                        trabalho[k]+=allocation[p][k];
                    }

                    terminado[p]=1;
                    encontrou=1;
                    contador++;
        
                }
            }
        }
        if (encontrou==0){
            free(trabalho);
            free(terminado);
            return 0;
        }
    }
    
    free(trabalho);
    free(terminado);
    return 1;
}

int main(int argc, char *argv[]){
    if (argc < 2){
        printf("Quantidade de argumentos insuficiente.\n");
        return 1;
    }

    int quantidade_de_recursos = argc - 1;
    int *requisicoes = (int*) malloc(quantidade_de_recursos * sizeof(int)); 
    if (requisicoes == NULL){
        printf("Erro ao alocar memoria.\n");
        return 1;
    }

    for (int i=0; i< quantidade_de_recursos; i++){
       //i+1 porque o argv[0] é o executável 
        if (!eh_numero(argv[i+1])){
            printf("O argumento deve ser numerico.\n");
            free(requisicoes);
            return 1;
        }
        requisicoes[i] = atoi(argv[i+1]);
    }
        //abrir o custumer.txt para contar as linhas dos clientes
        FILE *arquivo_costumizado = fopen("customer.txt", "r");
        if (arquivo_costumizado ==NULL){
            printf("nao foi possivel abrir customer.txt.\n");
            free(requisicoes);
            return 1;
        }

        int quantidade_de_clientes = 0;
        char linha[1024]; //buffer temporario para ler cada linha 2 elevado a 10

        while(fgets(linha, sizeof(linha), arquivo_costumizado) != NULL){
            if (linha[0]== '\n' || linha[0] == '\r' || linha[0] == '\0'){
                continue;
            }
            quantidade_de_clientes++;
        }

        if (quantidade_de_clientes ==0){
            printf("customer.txt vazio\n");
            fclose(arquivo_costumizado);
            return 1;
        }

        int **maximum = (int**) malloc(quantidade_de_clientes * sizeof(int*));
        int **allocation = (int**) malloc(quantidade_de_clientes * sizeof(int*));
        int **need = (int**) malloc(quantidade_de_clientes * sizeof(int*));

        for (int i=0; i<quantidade_de_clientes; i++){
            maximum[i] = (int*) malloc(quantidade_de_recursos*sizeof(int));
            allocation[i] = (int*) calloc(quantidade_de_recursos,sizeof(int));
            need[i] = (int*) malloc(quantidade_de_recursos*sizeof(int));
        }

        rewind(arquivo_costumizado); //voltar para o inicio do arquivo
        int cliente = 0;
        while(fgets(linha, sizeof(linha), arquivo_costumizado) != NULL && cliente < quantidade_de_clientes){
            if (linha[0] == '\n' || linha[0] == '\r' || linha[0] == '\0') continue;
            int recurso = 0;

            char *uso = strtok(linha, ",\n\r");

            while (uso != NULL){

                if (recurso >= quantidade_de_recursos){
                    printf("customer.txt possui mais recursos do que o informado na execucao\n");
                    return 1;
                }
                if (!eh_numero(uso)){
                    printf("erro de formatacao em custommer.txt, valor nao numerico\n");
                    return 1;
                }

                int valor = atoi(uso);

                maximum[cliente][recurso] = valor;
                need[cliente][recurso] = valor;

                recurso++;
                uso=strtok(NULL, ",\n\r");
        }

        if (recurso != quantidade_de_recursos){
            printf("customer.txt possui menos recursos do que o informado na execucao\n");
            return 1;
        }

        cliente++;
    }

    fclose(arquivo_costumizado);

    FILE *arquivo_comandos = fopen("commands.txt", "r");
    if (arquivo_comandos ==NULL){
        printf("Nao foi possivel abrri commands.txt\n");
        return 1;
    }

    FILE *arquivo_saida = fopen("result.txt", "w");
    if(arquivo_saida ==NULL){
        printf("Nao foi possivel criar result.txt\n");
        fclose(arquivo_comandos);
        return 1;
    }

    int *pedido = (int*) malloc(quantidade_de_recursos * sizeof(int));

    while (fgets(linha, sizeof(linha), arquivo_comandos) != NULL){
        if (linha[0] == '\n' || linha[0] == '\r' || linha[0] == '\0') continue;
        char *comando = strtok(linha, " \t\n\r");
        if (comando == NULL) continue;

        if (strcmp(comando, "*") == 0){
            fprintf(arquivo_saida, "MAXIMUM     |   ALLOCATION |    NEED\n");
            for (int i=0; i<quantidade_de_clientes; i++){
                for (int j=0;j<quantidade_de_recursos;j++){
                    fprintf(arquivo_saida, "%d ", maximum[i][j]);
                }

                fprintf(arquivo_saida, "    |   ");
                for (int j=0; j<quantidade_de_recursos;j++){
                    fprintf(arquivo_saida, "%d ", allocation[i][j]);
                }
                fprintf(arquivo_saida, "    |   ");
                for (int j=0; j<quantidade_de_recursos;j++){
                    fprintf(arquivo_saida, "%d ", need[i][j]);
                }
                fprintf(arquivo_saida, "\n");
            }
            fprintf(arquivo_saida, "AVAILABLE ");
            for (int j=0;j<quantidade_de_recursos; j++){
                fprintf(arquivo_saida, "%d ", requisicoes[j]);
            }
            fprintf(arquivo_saida, "\n");     
    }

    else if (strcmp(comando, "RQ") == 0 || strcmp(comando, "RL") == 0) {
            char *str_cliente = strtok(NULL, " \t\n\r");
            if (str_cliente == NULL || !eh_numero(str_cliente)) {
                printf("Erro de formatacao em commands.txt\n");
                return 1;
            }
            int id_cliente = atoi(str_cliente);

            if (id_cliente < 0 || id_cliente >= quantidade_de_clientes) {
                printf("cliente invalido em commands.txt\n");
                return 1;
            }

            int r = 0;
            char *tok_rec = strtok(NULL, " \t\n\r");

            while (tok_rec != NULL) {
                if (r >= quantidade_de_recursos) {
                    printf("mais recursos do que o esperado em commands.txt\n");
                    return 1;
                }
                
                if (!eh_numero(tok_rec)) {
                    printf("erro de formatacao em commands.txt\n");
                    return 1;
                }
                
                pedido[r] = atoi(tok_rec);
                if (pedido[r] < 0) {
                    printf("valor negativo em commands.txt\n");
                    return 1;
                }

                r++;
                tok_rec = strtok(NULL, " \t\n\r");
            }

            if (r != quantidade_de_recursos) {
                printf("menos recursos do que o esperado em commands.txt\n");
                return 1;
            }
        
        if (strcmp(comando, "RQ") == 0){
            int excede_need = 0;
            for (int j=0; j<quantidade_de_recursos; j++){
                if (pedido[j] > need[id_cliente][j]) excede_need = 1;
            }
            if (excede_need){
                fprintf(arquivo_saida, "The customer %d request ", id_cliente);
                for (int j=0; j<quantidade_de_recursos;j++) fprintf(arquivo_saida, "%d ", pedido[j]);
                fprintf(arquivo_saida, "was denied because exceed its maximum allocation\n");
                continue;
            }

            int falta_recurso = 0;
            for (int j=0;j<quantidade_de_recursos; j++){
                if (pedido[j] > requisicoes[j]) falta_recurso = 1;
            }
            if (falta_recurso){
                fprintf(arquivo_saida, "The resources ");
                for (int j=0; j<quantidade_de_recursos; j++) fprintf(arquivo_saida,"%d ",requisicoes[j]);
                fprintf(arquivo_saida, "are not enough to customer %d request ", id_cliente);
                for(int j=0;j<quantidade_de_recursos;j++)fprintf(arquivo_saida, "%d ", pedido[j]);
                fprintf(arquivo_saida, "\n");
                continue;
            }
            for (int j=0; j<quantidade_de_recursos; j++){
                requisicoes[j] -= pedido[j];
                allocation[id_cliente][j] +=pedido[j];
                need[id_cliente][j] -= pedido[j];
            }
            if (eh_seguro(requisicoes, allocation, need, quantidade_de_clientes, quantidade_de_recursos)){
                fprintf(arquivo_saida, "Allocate to customer %d the resources", id_cliente);
                for (int j=0; j<quantidade_de_recursos;j++) fprintf(arquivo_saida, " %d", pedido[j]);
                fprintf(arquivo_saida, "\n");
            }else{
                for(int j=0;j<quantidade_de_recursos;j++){
                    requisicoes[j]+=pedido[j];
                    allocation[id_cliente][j]-=pedido[j];
                    need[id_cliente][j]+=pedido[j];
                }
                fprintf(arquivo_saida, "The customer %d request ", id_cliente);
                for (int j=0; j<quantidade_de_recursos; j++) fprintf(arquivo_saida, "%d ", pedido[j]);
                fprintf(arquivo_saida, "was denied because result in an unsafe state\n");
            }
        }else{
            for(int j=0;j<quantidade_de_recursos;j++){
                if(pedido[j] > allocation[id_cliente][j]){
                    printf("cliente tentanto devolver mais do que possui\n");
                    return 1;
                }
            }
            for(int j=0; j<quantidade_de_recursos; j++){
                requisicoes[j] += pedido[j];
                allocation[id_cliente][j] -= pedido[j];
                need[id_cliente][j] +=pedido[j];
            }
            fprintf(arquivo_saida, "Release from customer %d the resources", id_cliente);
            for (int j=0; j<quantidade_de_recursos; j++) fprintf(arquivo_saida, " %d", pedido[j]);
            fprintf(arquivo_saida, "\n");
        }
    }else{
        printf("comando desconhecido em commands.txt\n");
        return 1;
    }
}

    fclose(arquivo_comandos);
    fclose(arquivo_saida);

    free(requisicoes);
    free(pedido);
    for(int i=0;i<quantidade_de_clientes;i++){
        free(maximum[i]);
        free(allocation[i]);
        free(need[i]);
    }

    free(maximum);
    free(allocation);
    free(need);

    return 0;

}
