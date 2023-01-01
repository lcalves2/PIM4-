#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <conio.h>
#include <locale.h>

//Programa desenvolvido por Lucas Correa Alves.

/*
struct tm (time.h)
int tm_sec; //representa os segundos de 0 a 59
int tm_min; //representa os minutos de 0 a 59
int tm_hour; //representa as horas de 0 a 24
int tm_mday: //dia do mês de 1 a 31
int tm_mon; //representa os meses do ano de 0 a 11
int tm_year; //representa o ano a partir de 1900
int tm_wday; //dia da semana de 0 (domingo) até 6 (sábado)
int tm_yday; // dia do ano de 1 a 365
int tm_isdst; //indica horário de verão se for diferente de zero
*/

#define USUARIO_ADM "ADMIN"
#define SENHA_ADM "ADMIN"
struct login usuario_logado;
struct lista inicio_gdr;
int contador_grupo_de_risco;

typedef struct login{
    char usuario_login[25];
    char usuario_senha[25];
    char usuario_nome[100];
    char usuario_cpf[12];
}login;

typedef struct endereco{
    char cep[9];
    char rua[50];
    char numero[10];
    char bairro[50];
    char cidade[50];
    char estado[3];
}endereco;

typedef struct log{
    char cadastrado_por[100];
    struct tm data_cadastro;
    char alterado_por[100];
    struct tm data_alteracao;
    bool usuario_alterado;
}log;

typedef struct pessoa{
    char cpf[12];
    char nome[100];
    char email[50];
    char telefone[15];
    struct endereco moradia;
    struct tm nascimento;
    int idade;
    char comorbidades[10][25];
    bool grupo_de_risco;
    struct log log_usuario;
}pessoa;

typedef struct No{
    pessoa paciente;
    struct No *proximo;
}No;

typedef struct lista{
    No *lista_gdr;
    int quantidade_gdr;
}lista;

int checar_digitos(char *str)
{
    if(!str)
        return 0;     /* Se string for NULL, retorna 0 */
    if(!(*str))
        return 0;  /* Se string for VAZIA, retorna 0 */

    while(*str)
        if(!isdigit(*str++))
            return 0;

    return 1;
}

void atualizar_log_alteracao(No *paciente_alterado, struct tm *data_atual)
{
    paciente_alterado->paciente.log_usuario.usuario_alterado = true;
    strcpy(paciente_alterado->paciente.log_usuario.alterado_por, usuario_logado.usuario_nome);
    paciente_alterado->paciente.log_usuario.data_alteracao.tm_mday = data_atual->tm_mday;
    paciente_alterado->paciente.log_usuario.data_alteracao.tm_mon = data_atual->tm_mon;
    paciente_alterado->paciente.log_usuario.data_alteracao.tm_year = data_atual->tm_year;
    return;
}

int checar_bisexto(int tmp_mes, int tmp_dia, int tmp_ano)
{
    if (tmp_mes == 2 && tmp_dia == 29)
    {
        if(tmp_ano % 4 == 0)
        {
            if(tmp_ano % 100 == 0)
            {
                if(tmp_ano % 400 == 0)
                {
                    printf("\n  Ano Bisexto, data valida!!\n");
                    return tmp_dia;
                }
                else
                {
                    do
                    {
                    system("cls");
                    printf("\n\n----------------------------------------------------------\n");
                    printf("\n\n\tData invalida!!\n");
                    setbuf(stdin, 0);
                    printf("\n  Digite o dia de nascimento do paciente (dd):\n\t(dd):");
                    scanf("%d", &tmp_dia);
                    }
                    while((tmp_dia < 1) || (tmp_dia >= 29));
                    return tmp_dia;
                }
            }
            else
            {
                printf("\n  Ano Bisexto, data valida!!\n");
                return tmp_dia;
            }
        }
        else
        {
            do
            {
            printf("\n  Data invalida!!\n");
            setbuf(stdin, 0);
            printf("\n  Digite o dia de nascimento do paciente (dd):\n\t(dd):");
            scanf("%d", &tmp_dia);
            }
            while((tmp_dia < 1) || (tmp_dia >= 29));
            return tmp_dia;
        }
    }
return 0;
}

int checar_idade(int tmp_mes, int tmp_dia,int tmp_ano, struct tm *data_atual)
{
    int idade;
    tmp_ano -= 1900;
    tmp_mes -= 1;
    idade = data_atual->tm_year - tmp_ano;


    if (tmp_mes < data_atual->tm_mon)
    {
        return idade;
    }
    else if ((tmp_mes <= data_atual->tm_mon) && (tmp_dia <= data_atual->tm_mday))
    {
        return idade;
    }
    else
    {
        idade --;
        return idade;
    }
}

int obter_indice(long long cpf)
{
    int indice = (cpf % 100);
    if(indice > 100 || indice < 0)
    {
    printf("erro ao gerar indice\n");
    }
    return indice;
}

char* converter_string_minusculo(char *string)
{
    int comprimento_string = strlen(string);
    for (int i = 0; i < comprimento_string; i++)
    {
        string[i] = tolower(string[i]);
    }
    return string;
}

char* converter_string_maiusculo(char *string)
{
    int comprimento_string = strlen(string);
    for (int i = 0; i < comprimento_string; i++)
    {
        string[i] = toupper(string[i]);
    }
    return string;
}

int carregar_lista_usuarios(char arquivo_login[], struct login lista_usuarios[100])
{
int quantidade_usuarios = 0;
login aux_usuario;
FILE *file = fopen(arquivo_login, "rb");
if(file != NULL)
    {
        while(!feof(file))
        {
            if (fread(&aux_usuario, sizeof(struct login), 1, file))
            {
                strcpy(lista_usuarios[quantidade_usuarios].usuario_login, aux_usuario.usuario_login);
                strcpy(lista_usuarios[quantidade_usuarios].usuario_senha, aux_usuario.usuario_senha);
                strcpy(lista_usuarios[quantidade_usuarios].usuario_nome, aux_usuario.usuario_nome);
                strcpy(lista_usuarios[quantidade_usuarios].usuario_cpf, aux_usuario.usuario_cpf);
                quantidade_usuarios++;
            }
        }
    }

return quantidade_usuarios;
}

void login_senha(char arquivo_login[], struct login lista_usuarios[100], int quantidade_usuarios)
 {
    char s;
    int checar_digito;
    int tamanho_string = 0;
    bool logado = false;
    bool login_unico = false;
    char usuario[25];
    char senha[25];
    char nome[100];
    char cpf[12];
    char cpf_tmp[12];
    int opcao;
    char tmp;
    struct login aux;

    do
    {
        int tmp_int = 0;
        for (int i = 0; i < 25; i++)
            senha[i] = '\0';

        system("cls");
        printf("\n\n----------------------------------------------------------\n\n");
        printf("\n            Software de suporte ao Covid-19\n");
        setbuf(stdin, 0);
        printf("\n\n\t Usuário: ");
        scanf("%s", &usuario);
        if(strcmp(usuario, USUARIO_ADM) == 0)
        {
            setbuf(stdin, 0);
            printf("\n\t Senha: ");
            do                       //não mostrar a senha digitada:
            {
                setbuf(stdin, 0);
                s = getch();
                if (s != 13)
                {
                    senha[tmp_int] = s;  //caso o caractere digitado nao sejá 'ENTER' armazena o caractere na string senha
                    tmp_int++;
                    printf("*");          //imprime o '*' asterisco
                }
            }
            while(s != 13);

            if(strcmp(senha, SENHA_ADM) == 0)
            {
                logado = true;
                printf("\n\n----------------------------------------------------------\n\n");
                do
                {
                system("cls");
                printf("\n  Logado como Administrador!");
                printf("\n\n\t- Quantidade de usuários: %d", quantidade_usuarios);
                printf("\n\n----------------------------------------------------------\n");
                printf("\n            Software de suporte ao Covid-19\n");
                printf("\n\n\t0 - Sair \n\n\t1 - Novo usuário \n\n\t2 - Consultar usuários \n\n\t3 - Excluir usuário\n");
                printf("\n\n----------------------------------------------------------\n\n  Opcão: ");
                setbuf(stdin, 0);
                scanf("%d", &opcao);
                    switch(opcao)
                    {
                    case 0:  //Voltar
                        logado = false;
                        system("cls");
                        break;

                    case 1:  //Cadastro  de Usuário
                        system("cls");
                        printf("\n\n-------------------Cadastro de usuário--------------------\n");
                        setbuf(stdin, 0);
                        printf("\n  Digite o nome do usuário: ");
                        scanf("%[^\n]s", &aux.usuario_nome);

                        do
                        {
                            setbuf(stdin, 0);
                            printf("\n  Digite o login do usuário: ");
                            scanf("%s", &aux.usuario_login);
                            strcpy(usuario, aux.usuario_login);
                            converter_string_maiusculo(usuario);
                            for (int i = 0; i < quantidade_usuarios; i++)   //LOGIN COMUM
                            {
                                char *login_usuarios_maiusculo = converter_string_maiusculo(lista_usuarios[i].usuario_login);
                                if (strcmp(usuario, login_usuarios_maiusculo) == 0 || strcmp(usuario, USUARIO_ADM) == 0)
                                {
                                    login_unico = false;
                                    system("cls");
                                    printf("\n----------------------------------------------------------\n");
                                    printf("\n  Este usuário já esta em uso!!\n\n");

                                    break;

                                }
                                else
                                {
                                    login_unico = true;
                                }
                            }
                        }
                        while(login_unico == false);

                        setbuf(stdin, 0);
                        printf("\n  Digite a senha do usuário: ");
                        scanf("%s", &aux.usuario_senha);

                        do
                        {
                            setbuf(stdin, 0);
                            printf("\n  Digite o CPF do usuário: ");
                            scanf("%s", &aux.usuario_cpf);
                            tamanho_string = strlen(aux.usuario_cpf);
                            checar_digito = checar_digitos(aux.usuario_cpf);
                        }
                        while(!(tamanho_string == 11 && checar_digito == 1));

                        system("cls");
                        printf("\n\n----------------------------------------------------------\n");
                        printf("\n\n  Confirme os dados do usuário:");
                        printf("\n\n\tNome: %s", aux.usuario_nome);
                        printf("\n\n\tLogin: %s", aux.usuario_login);
                        printf("\n\n\tSenha: %s", aux.usuario_senha);
                        printf("\n\n\tCPF: %s", aux.usuario_cpf);

                        do
                        {
                        setbuf(stdin, 0);
                        printf("\n\n  Os dados estão corretos? (S/N)\n\n\t(S/N): ");
                        scanf("%c", &tmp);
                        }
                        while (!((tmp == 's') || (tmp == 'S') || (tmp == 'n') || (tmp == 'N')));

                            if(tmp == 'S' || tmp == 's')
                            {
                                FILE *file = fopen(arquivo_login, "ab");
                                if(file)
                                {
                                    fwrite(&aux, sizeof(struct login), 1, file);
                                    printf("\n*****Usuário cadastrado*****");
                                    fclose(file);
                                    quantidade_usuarios = carregar_lista_usuarios(arquivo_login, lista_usuarios);
                                    break;
                                }

                            }
                        break;

                    case 2:
                        system("cls");
                        for (int i = 0; i < quantidade_usuarios; i++)
                        {
                            printf("\n---------------------Usuário %d--------------------------\n", i+1);
                            printf("\n\tNome: %s", lista_usuarios[i].usuario_nome);
                            printf("\n\tLogin: %s", lista_usuarios[i].usuario_login);
                            printf("\n\tSenha: %s", lista_usuarios[i].usuario_senha);
                            printf("\n\tCPF: %s\n", lista_usuarios[i].usuario_cpf);
                        }
                        printf("\n----------------------------------------------------------\n");
                        system("pause");
                        break;

                    case 3:
                        system("cls");
                        printf("\n\n----------------------------------------------------------\n");
                        printf("  Digite o cpf do usuário a ser excluído:  \n\n\tCPF:");
                        scanf("%s", &cpf_tmp);
                        for (int i = 0; i < quantidade_usuarios; i++)
                        {
                            if (strcmp(cpf_tmp, lista_usuarios[i].usuario_cpf) == 0)
                            {
                                do
                                {
                                system("cls");
                                printf("\n\n---------------------Usuário %d--------------------------\n", i+1);
                                printf("\n\tNome: %s", lista_usuarios[i].usuario_nome);
                                printf("\n\tLogin: %s", lista_usuarios[i].usuario_login);
                                printf("\n\tSenha: %s", lista_usuarios[i].usuario_senha);
                                printf("\n\tCPF: %s", lista_usuarios[i].usuario_cpf);
                                printf("\n\n----------------------------------------------------------\n");

                                setbuf(stdin, 0);
                                printf("\n\n  Confirme a exclusão do usuário!! (S/N)\n\n\t(S/N): ");
                                scanf("%c", &tmp);
                                }
                                while (!((tmp == 's') || (tmp == 'S') || (tmp == 'n') || (tmp == 'N')));

                                    if(tmp == 'S' || tmp == 's')
                                    {
                                        FILE *file = fopen(arquivo_login, "wb");
                                        if(file)
                                        {
                                            for (int i = 0; i < quantidade_usuarios; i++)
                                            {
                                                if (strcmp(cpf_tmp, lista_usuarios[i].usuario_cpf) != 0)
                                                {
                                                    fwrite(&lista_usuarios[i], sizeof(login), 1, file);
                                                }
                                            }

                                        }
                                    fclose(file);
                                    quantidade_usuarios = carregar_lista_usuarios(arquivo_login, lista_usuarios);
                                    }
                            }
                        }

                        break;

                    default:
                        if(opcao < 0 || opcao > 2)
                        {
                            system("cls");
                            printf("\n----------------------------------------------------------\n\n");
                            printf("\n\tOpção invalida, digite novamente!!\n\n");
                            printf("\n----------------------------------------------------------\n\n\n");
                            system("pause");
                        }
                    }
                }
                while(opcao != 0);
            }
            system("cls");
        }
        else
        {
            for (int i = 0; i < quantidade_usuarios; i++)   //LOGIN COMUM
            {
                if (strcmp(usuario, lista_usuarios[i].usuario_login) == 0)
                    {
                        printf("\n\t Senha: ");
                        do                       //não mostrar a senha digitada:
                        {
                            setbuf(stdin, 0);
                            s = getch();
                            if (s != 13)
                            {
                                senha[tmp_int] = s;  //caso o caractere digitado nao seja 'ENTER' armazena o caractere na string senha
                                tmp_int++;
                                printf("*");          //imprime o '*' asterisco
                            }
                        }
                        while(s != 13);
                        if(strcmp(senha, lista_usuarios[i].usuario_senha)== 0)
                        {
                            logado = true;
                            usuario_logado = lista_usuarios[i];
                            break;
                        }
                        else
                        {
                            system("cls");
                        }
                    }
            }
        }
    }
    while(logado == false);

}

void inicializar_lista(No indice_cpf[])
{
    for (int i = 0; i < 100; i++)
    {
         indice_cpf[i].proximo = NULL;
    }
}

int carregar_bancodedados(char arquivo[], No indice_cpf[], struct tm *data_atual)
{
    inicializar_lista(indice_cpf);
    char cpf_str[12];
    long long cpf_tmp;
    pessoa paciente;
    FILE *file = fopen(arquivo, "rb");
    contador_grupo_de_risco = 0;
    int contador_contatos = 0;
    int indice;
    int idade;
    inicio_gdr.lista_gdr = NULL;
    inicio_gdr.quantidade_gdr = 0;

    if(file != NULL)
    {
        while(!feof(file))
        {
            if (fread(&paciente, sizeof(pessoa), 1, file))
            {
                No *novo = (No*)malloc(sizeof(No));
                novo->paciente = paciente;
                novo->proximo = NULL;
                strcpy(cpf_str, paciente.cpf);
                cpf_tmp = atoll(cpf_str);
                indice = obter_indice(cpf_tmp);

                printf("\ncpf cadastrado: %s\n", paciente.cpf);

                No *aux = &indice_cpf[indice];
                if (aux->proximo == NULL)
                {
                    aux->proximo = novo;
                    novo->proximo = NULL;
                }
                else
                {
                    while (aux->proximo != NULL)
                    {
                        aux = aux->proximo;
                    }
                    aux->proximo = novo;
                    novo->proximo = NULL;
                }
                idade = checar_idade((novo->paciente.nascimento.tm_mon + 1), (novo->paciente.nascimento.tm_mday), (novo->paciente.nascimento.tm_year + 1900), data_atual);
                novo->paciente.idade = idade;

                if (idade >= 65)
                {
                    novo->paciente.grupo_de_risco = true;
                }

                if (novo->paciente.grupo_de_risco == true)
                {

                No *novo_gdr = (No*)malloc(sizeof(No));
                memcpy(novo_gdr, novo, (sizeof(No)));
                No *aux_gdr = inicio_gdr.lista_gdr;


                    if (inicio_gdr.lista_gdr == NULL)
                    {
                        inicio_gdr.lista_gdr = novo_gdr;
                    }

                    else
                    {
                        while (aux_gdr->proximo != NULL)
                        {
                            aux_gdr = aux_gdr->proximo;
                        }
                        aux_gdr->proximo = novo_gdr;
                    }
                contador_grupo_de_risco++;
                }
                contador_contatos++;
            }
        }
    return contador_contatos;
    }
    else
    {
    printf("\n Banco de dados a ser criado.\n");
    }
fclose(file);
return 0;
}

void cadastrar(char arquivo[], No indice_cpf[], struct tm *data_atual)
{
    char tmp;
    int indice;
    int idade;
    long long cpf_tmp;
    char cpf_str[50];
    char telefone_str[15];
    int checar_digito;
    int tamanho_string = 0;

    setbuf(stdin, 0);
    No *novo = (No*)malloc(sizeof(No));
    system("cls");

    do
    {
        system("cls");
        printf("\n\n-------------------Cadastro de paciente-------------------\n\n");
        setbuf(stdin, 0);
        printf("\n  Digite o CPF do paciente: \n\n\tCPF: ");
        scanf("%s", &cpf_str);
        tamanho_string = strlen(cpf_str);
        checar_digito = checar_digitos(cpf_str);
    }
    while(!(tamanho_string == 11 && checar_digito == 1));

    cpf_tmp = atoll(cpf_str);  //transforma a string cpf em l.long
    strcpy(novo->paciente.cpf, cpf_str);
    indice = obter_indice(cpf_tmp); //obter_indice retorna os ultimos 2 digitos do cpf

    No *aux = &indice_cpf[indice];
    if (aux->proximo == NULL)
    {
        aux->proximo = novo;
        novo->proximo = NULL;
    }
    else
    {
        if (strcmp(cpf_str, aux->paciente.cpf) == 0)
            {
                system("cls");
                printf("\n\n----------------------------------------------------------\n");
                printf("\n\n\tPaciente já cadastrado!!\n\n");
                system("pause");
                printf("\n\n----------------------------------------------------------\n\n\n");
                return;
            }

        while (aux->proximo != NULL)
        {
            aux = indice_cpf[indice].proximo;
            if (strcmp(cpf_str, aux->paciente.cpf) == 0)
            {
                system("cls");
                printf("\n\n----------------------------------------------------------\n");
                printf("\n\n\tPaciente já cadastrado!!\n");
                printf("\n\n----------------------------------------------------------\n\n\n");
                system("pause");
                return;
            }

        }
    aux->proximo = novo;
    novo->proximo = NULL;
    }

    setbuf(stdin, 0);
    printf("\n  Digite o nome do paciente: \n\n\tNome: ");
    fgets(novo->paciente.nome, 99, stdin);

    do
    {
        system("cls");
        setbuf(stdin, 0);
        printf("\n  Digite o telefone (com DDD, ex:11988887777) do paciente: \n\n\tTelefone: ");
        scanf("%s", &telefone_str);
        tamanho_string = strlen(telefone_str);
        checar_digito = checar_digitos(telefone_str);
    }
    while(!(tamanho_string < 12 && checar_digito == 1));
    strcpy(novo->paciente.telefone, telefone_str);

    setbuf(stdin, 0);
    printf("\n  Digite o E-mail: \n\n\tE-mail: ");
    scanf("%s", novo->paciente.email);

    do
    {
        setbuf(stdin, 0);
        printf("\n  Digite o CEP da casa: \n\n\tCEP: ");
        scanf("%s", &novo->paciente.moradia.cep);
        tamanho_string = strlen(novo->paciente.moradia.cep);
        checar_digito = checar_digitos(novo->paciente.moradia.cep);
    }
    while(!(tamanho_string == 8 && checar_digito == 1));

    system("cls");
    setbuf(stdin, 0);
    printf("\n  Digite a rua da moradia: (ex: Avenida Copacabana, Rua Sao Joao)\n\n\tRua: ");
    fgets(novo->paciente.moradia.rua, 49, stdin);

    do
    {
        setbuf(stdin, 0);
        printf("\n  Digite o número da casa: \n\n\tNúmero: ");
        scanf("%s", novo->paciente.moradia.numero);
        tamanho_string = strlen(novo->paciente.moradia.numero);
        checar_digito = checar_digitos(novo->paciente.moradia.numero);
    }
    while(!(tamanho_string < 5 && checar_digito == 1));

    setbuf(stdin, 0);
    printf("\n  Digite o bairro: \n\n\tBairro: ");
    fgets(novo->paciente.moradia.bairro, 49, stdin);

    setbuf(stdin, 0);
    printf("\n  Digite a cidade: \n\n\tCidade: ");
    fgets(novo->paciente.moradia.cidade, 49, stdin);

    setbuf(stdin, 0);
    char estado_tmp[3];
    printf("\n  Digite o estado (Sigla: SP, RJ, etc): \n\n\tEstado: ");
    scanf("%s", estado_tmp);
    converter_string_maiusculo(estado_tmp);
    strcpy(novo->paciente.moradia.estado, estado_tmp);

    int tmp_ano;
    do
    {
        system("cls");
        setbuf(stdin, 0);
        printf("\n  Digite o ano de nascimento do paciente (aaaa)\n\n\tAno: ");
        scanf("%d", &tmp_ano);
    }
    while(!(tmp_ano > 1900 && tmp_ano <= (data_atual->tm_year+1900) && checar_digito == 1));
    novo->paciente.nascimento.tm_year = (tmp_ano - 1900);

    int tmp_mes;
    do
    {
        setbuf(stdin, 0);
        printf("\n  Digite o mês de nascimento do paciente (mm)\n\n\tMês: ");
        scanf("%d", &tmp_mes);
    }
    while(!(tmp_mes > 0 && tmp_mes < 13 && checar_digito == 1));
    if(tmp_ano == (data_atual->tm_year+1900))
    {
        if (tmp_mes > data_atual->tm_mon+1)
        {
            system("cls");
            printf("\n----------------------------------------------------------\n");
            printf("\n\n\tData invalida!!, digite novamente!!\n\n");
            printf("\n----------------------------------------------------------\n\n\n");
            system("pause");
            system("cls");
            do
            {
            setbuf(stdin, 0);
            printf("\n  Digite o mês de nascimento do paciente (mm)\n\n\tMês: ");
            scanf("%d", &tmp_mes);
            }
            while(tmp_mes <= data_atual->tm_mon);
        }
    }
    novo->paciente.nascimento.tm_mon = (tmp_mes - 1);

    int tmp_dia;
    do
    {
        setbuf(stdin, 0);
        printf("\n  Digite o dia de nascimento do paciente (dd)\n\n\tDia: ");
        scanf("%d", &tmp_dia);
    }
    while(!(tmp_dia > 0 && tmp_dia < 32 && checar_digito == 1));

    if (tmp_mes == 2 && tmp_dia == 29)
    {
        tmp_dia = checar_bisexto(tmp_mes, tmp_dia, tmp_ano);
    }
    novo->paciente.nascimento.tm_mday = tmp_dia;

    idade = checar_idade(tmp_mes, tmp_dia, tmp_ano, data_atual);
    novo->paciente.idade = idade;
    if (idade >= 65)
        novo->paciente.grupo_de_risco = true;

    do
    {
    setbuf(stdin, 0);
    printf("\n  O paciente possui alguma comorbidade? (S/N)\n\n\t(S/N): ");
    scanf("%c", &tmp);
    }
    while (!((tmp == 's') || (tmp == 'S') || (tmp == 'n') || (tmp == 'N')));

        if(tmp == 'S' || tmp == 's')
        {
        int qnt_comorbidades = 0;
            for (int i = 1; i<10; i++)
            {
            tmp = 'a';
            setbuf(stdin, 0);
            printf("\n  Digite a comorbidade: (Uma por vez, ex: Diabetes) \n\n\tComorbidade: ");
            fgets(novo->paciente.comorbidades[i], 24, stdin);
            qnt_comorbidades++;
            setbuf(stdin, 0);
            printf("\n  O paciente possui mais alguma comorbidade? (S/N)\n\n\t(S/N): ");
            scanf("%c", &tmp);
                if((tmp == 'N') || (tmp == 'n'))
                {
                    break;
                }
            }
            char qnt_comorbidades_str[25];
            setbuf(stdin, 0);
            itoa(qnt_comorbidades, qnt_comorbidades_str, 10);
            strcpy(novo->paciente.comorbidades[0], qnt_comorbidades_str);
            novo->paciente.grupo_de_risco = true;
        }
    //printf("  Data: %d/%d/%d\n", data_atual->tm_mday, data_atual->tm_mon+1, data_atual->tm_year+1900);
    strcpy(novo->paciente.log_usuario.cadastrado_por, usuario_logado.usuario_nome);

    //Log de usuário:
    novo->paciente.log_usuario.data_cadastro.tm_mday = data_atual->tm_mday;
    novo->paciente.log_usuario.data_cadastro.tm_mon = data_atual->tm_mon;
    novo->paciente.log_usuario.data_cadastro.tm_year = data_atual->tm_year;
    novo->paciente.log_usuario.usuario_alterado = false;

pessoa tmp_salvar = novo->paciente;
FILE *file = fopen(arquivo, "ab");
    if(file)
    {
        fwrite(&tmp_salvar, sizeof(pessoa), 1, file);
        fclose(file);
    }

system("cls");
printf("\n\n----------------------------------------------------------\n");
printf("\n\n\tPaciente cadastrado com sucesso!!\n");
printf("\n\n----------------------------------------------------------\n\n\n\n");
system("pause");
return;
}

void consultar(No indice_cpf[])
{
    bool nao_encontrado = true;
    char cpf_str[12];
    int indice;
    long long cpf_tmp;
    system("cls");
    printf("\n----------------------------------------------------------\n\n");
    printf("\n  Qual o cpf do cliente a ser consultado?\n\n\t CPF: ");
    scanf("%s", cpf_str);
    cpf_tmp = atoll(cpf_str);
    indice = obter_indice(cpf_tmp);
    No *aux;
    aux = &indice_cpf[indice];

    while (aux != NULL)
    {
        if (strcmp(aux->paciente.cpf, cpf_str) == 0)
        {
        nao_encontrado = false;
        system("cls");
        printf("\n-------------------------Paciente:------------------------\n\n");
        printf("\n\tNome: %s  \n\tCPF: %s", aux->paciente.nome, aux->paciente.cpf);
        printf("\n\n\tTelefone: (%c%c)", aux->paciente.telefone[0], aux->paciente.telefone[1]);
        for (int i = 2; i < strlen(aux->paciente.telefone); i++)
            {

                printf("%c", aux->paciente.telefone[i]);
            }
        printf("\n\n\tE-mail: %s\n", aux->paciente.email);
        printf("\n\tIdade: %i\n", aux->paciente.idade);
        printf("\n\tData de nascimento %02d/%02d/%d\n", aux->paciente.nascimento.tm_mday, aux->paciente.nascimento.tm_mon+1, aux->paciente.nascimento.tm_year+1900);
        printf("\n\tCEP: %s\n", aux->paciente.moradia.cep);
        printf("\n\tEndereço: %s,", aux->paciente.moradia.rua);
        printf("\n\tNúmero: %s\n", aux->paciente.moradia.numero);
        printf("\n\tBairro: %s", aux->paciente.moradia.bairro);
        printf("\n\tCidade: %s \n\tEstado: %s.\n", aux->paciente.moradia.cidade, aux->paciente.moradia.estado);
        printf("\n\tCadastrado pelo usuário: %s\tData: %d/%d/%d.\n", aux->paciente.log_usuario.cadastrado_por, aux->paciente.log_usuario.data_cadastro.tm_mday, aux->paciente.log_usuario.data_cadastro.tm_mon+1, aux->paciente.log_usuario.data_cadastro.tm_year+1900);

        if (aux->paciente.log_usuario.usuario_alterado == true)
        {
            printf("\n\tAlterado pelo usuário: %s\tData: %d/%d/%d.\n", aux->paciente.log_usuario.alterado_por, aux->paciente.log_usuario.data_alteracao.tm_mday, aux->paciente.log_usuario.data_alteracao.tm_mon+1, aux->paciente.log_usuario.data_alteracao.tm_year+1900);
        }

        if (aux->paciente.grupo_de_risco == false)
        {
            printf("\n\tGrupo de risco: Não.\n");
        }
        else
        {
            printf("\n\tGrupo de risco: Sim.\n");
            printf("\n----------------Lista de comorbidades:--------------------\n");
            int qnt_comorbidades = atoi(aux->paciente.comorbidades[0]);
            for (int i = 1; i <= qnt_comorbidades; i++)
            {
                printf("\n\t%d - %s", i, aux->paciente.comorbidades[i]);
            }
        }
        printf("\n----------------------------------------------------------\n");
        int tmp = 1;
        system("pause");
        system("cls");
        return;
        }
    aux = aux->proximo;
    }
    system("cls");
    printf("\n----------------------------------------------------------\n");
    printf("\n\n\tCPF '%s' não encontrado!!\n\n", cpf_str);
    printf("\n----------------------------------------------------------\n\n\n");
    system("pause");

return;
}

bool salvar_alteracoes(char arquivo[], char *cpf_alterado, struct pessoa paciente_alterado)
{
    pessoa paciente;
    FILE *file = fopen(arquivo, "rb+");
    int contador_contatos = 0;

    if(file != NULL)
    {
        while(!feof(file))
        {
            if (fread(&paciente, sizeof(pessoa), 1, file) == 1)
            {
                No *novo = (No*)malloc(sizeof(No));
                novo->paciente = paciente;
                contador_contatos++;

                if ((strcmp(novo->paciente.cpf, cpf_alterado) == 0))
                {
                    contador_contatos--;

                    fseek(file, contador_contatos * sizeof(pessoa), SEEK_SET);
                    fwrite(&paciente_alterado, sizeof(pessoa), 1, file);

                    printf("\n\t Alteracoes salvas com sucesso!!\n");
                    fclose(file);
                    return true;
                }
            }

        }
    }
    else
    {
    printf("\n Erro ao salvar alteracoes\n");
    }
fclose(file);
return false;
}

void alterar(No indice_cpf[], char arquivo[], struct tm *data_atual)
{
    char cpf_str[12];
    int indice;
    long long cpf_tmp;
    struct pessoa paciente_alterado;
    system("cls");
    printf("\n----------------------------------------------------------\n");
    printf("\n  Qual o cpf do cliente a ser alterado?\n\n\tCPF: ");
    scanf("%s", cpf_str);
    cpf_tmp = atoll(cpf_str);
    indice = obter_indice(cpf_tmp);
    No *aux;
    aux = &indice_cpf[indice];
    No *checar_info;
    int tmp;

    if (aux == NULL)
    {
    printf("\n----------------------------------------------------------\n");
    printf("\n\n\tPaciente não cadastrado!\n");
    }
    else
    {
        while (aux != NULL)
        {
            if (strcmp(aux->paciente.cpf, cpf_str) == 0)
            {
                int op;
                int opcao;
                int opcao2;
                int opcao3;
                int opcao4;
                int tmp_dia;
                int tmp_mes;
                int tmp_ano;
                int checar_digito;
                bool stop;
                char checar_cpf_str[50];
                int tamanho_string;
                char qnt_comorbidades_str[25];
                int qnt_comorbidades;
                char tmp_excluir;

                do
                {
                system("cls");
                printf("\n  Paciente cadastrado pelo usuário: %s\n", aux->paciente.log_usuario.cadastrado_por);
                printf("\n--------------------Alterar paciente:------------------------\n");
                printf("\n  Digite a opção de alteracao desejada:\n\n");
                printf("\n\t0 - Voltar\n\n\t1 - Nome: %s\n\t2 - CPF: %s\n\n\t3 - Telefone: %s\n\n\t4 - E-mail: %s", aux->paciente.nome, aux->paciente.cpf, aux->paciente.telefone, aux->paciente.email);
                printf("\n\n\t5 - Data de nascimento\n\n\t6 - Endereço\n\n\t7 - Comorbidades");
                setbuf(stdin, 0);
                printf("\n\n----------------------------------------------------------\n\n  Opcão: ");
                scanf("%d", &opcao);

                    switch(opcao)
                    {
                    case 0:  //Voltar
                        opcao = 0;
                        break;

                    case 1:  //Nome
                        system("cls");
                        printf("\n----------------------------------------------------------\n");
                        printf("\n  Nome registrado: %s  \n", aux->paciente.nome);
                        setbuf(stdin, 0);
                        printf("\n  Digite o nome do paciente a ser alterado: \n\n\tNome: ");
                        fgets(aux->paciente.nome, 99, stdin);
                        atualizar_log_alteracao(aux, data_atual);
                        printf("\n----------------------------------------------------------\n");
                        printf("\n\n  Mudança de nome registrado!!\n\n  Nome atualizado: %s\n", aux->paciente.nome);
                        printf("\n----------------------------------------------------------\n");
                        system("pause");
                        break;

                    case 2:  //CPF
                        do
                        {
                            stop = false;
                            system("cls");
                            printf("\nCPF registrado: %s\n", aux->paciente.cpf);
                            printf("\n----------------------------------------------------------\n\n");
                            setbuf(stdin, 0);
                            printf("\n  Digite o CPF do paciente: \n\n\tCPF: ");
                            scanf("%s", checar_cpf_str);
                            tamanho_string = strlen(checar_cpf_str);
                            checar_digito = checar_digitos(checar_cpf_str);
                        }
                        while(!(tamanho_string == 11 && checar_digito == 1));
                        cpf_tmp = atoll(checar_cpf_str);
                        indice = obter_indice(cpf_tmp);
                        checar_info = &indice_cpf[indice];

                        while (checar_info != NULL)
                        {
                            if (strcmp(checar_info->paciente.cpf, checar_cpf_str) == 0)
                            {
                                printf("\n  CPF já cadastrado!\n\n\n");
                                stop = true;
                                system("pause");
                                break;
                            }
                            checar_info = checar_info->proximo;
                        }

                        if(stop == false)
                        {
                        strcpy(aux->paciente.cpf, checar_cpf_str);
                        atualizar_log_alteracao(aux, data_atual);
                        printf("\n----------------------------------------------------------\n");
                        printf("\n  Mudança de CPF registrado!!\n\n\n");
                        system("pause");
                        }
                        break;

                    case 3:
                        system("cls");
                        printf("\n Telefone registrado: %s  \n", aux->paciente.telefone);
                        setbuf(stdin, 0);
                        printf("\n----------------------------------------------------------\n");
                        printf("\n  Digite o telefone do paciente a ser alterado: \n\n\tTelefone: ");
                        scanf("%s", aux->paciente.telefone);
                        atualizar_log_alteracao(aux, data_atual);
                        system("cls");
                        printf("\n----------------------------------------------------------\n");
                        printf("\n\n  Mudança de telefone registrado!!\n\n  Telefone atualizado: %s\n\n", aux->paciente.telefone);
                        printf("\n----------------------------------------------------------\n\n\n");
                        system("pause");
                        break;

                    case 4:  //E-mail
                        system("cls");
                        printf("\n E-mail registrado: %s  \n", aux->paciente.email);
                        setbuf(stdin, 0);
                        printf("\n----------------------------------------------------------\n");
                        printf("\n  Digite o e-mail do paciente a ser alterado: \n\n\tE-mail: ");
                        scanf("%s", aux->paciente.email);
                        atualizar_log_alteracao(aux, data_atual);
                        system("cls");
                        printf("\n----------------------------------------------------------\n");
                        printf("\n\n  Mudança de E-mail registrado!!\n\n  E-mail atualizado: %s\n\n", aux->paciente.email);
                        printf("\n----------------------------------------------------------\n\n\n");
                        system("pause");
                        break;

                    case 5:  //Data de Nascimento
                        do
                        {
                        system("cls");
                        printf("\n\n------------------Data de Nascimeto:----------------------\n");
                        printf("\n  Digite a opção desejada:\n");
                        setbuf(stdin, 0);
                        printf("\n\t0 - Voltar\n\n\t1 - Dia: %02d\n\n\t2 - Mês: %02d\n\n\t3 - Ano: %d", aux->paciente.nascimento.tm_mday, aux->paciente.nascimento.tm_mon+1, aux->paciente.nascimento.tm_year+1900);
                        printf("\n\n----------------------------------------------------------\n\n  Opcão: ");
                        scanf("%d", &opcao2);
                            switch(opcao2)
                            {
                            case 0:
                                break;

                            case 1:  //Dia
                                system("cls");
                                printf("\n Dia de nascimento registrado: %2d  \n", aux->paciente.nascimento.tm_mday);
                                setbuf(stdin, 0);
                                printf("\n----------------------------------------------------------\n");
                                printf("\n  Digite o dia de nascimento a ser alterado: \n\n\tDia: ");
                                scanf("%d", &tmp_dia);

                                if (aux->paciente.nascimento.tm_mon == 1 && tmp_dia == 29)
                                {
                                    tmp_dia = checar_bisexto(aux->paciente.nascimento.tm_mon+1, tmp_dia, aux->paciente.nascimento.tm_year+1900);
                                }

                                aux->paciente.nascimento.tm_mday = tmp_dia;
                                atualizar_log_alteracao(aux, data_atual);
                                system("cls");
                                printf("\n----------------------------------------------------------\n");
                                printf("\n  Mudança do dia de nascimento registrado!!\n\n\n  Dia: %2d\n\n", aux->paciente.nascimento.tm_mday);
                                printf("\n----------------------------------------------------------\n\n\n");
                                system("pause");
                                break;

                            case 2:  //mês
                                system("cls");
                                printf("\n Mês de nascimento registrado: %2d  \n", aux->paciente.nascimento.tm_mon+1);
                                setbuf(stdin, 0);
                                printf("\n----------------------------------------------------------\n");
                                printf("\n\n  Digite mês de nascimento a ser alterado: \n\n\tMês: ");
                                scanf("%d", &tmp_mes);
                                if (tmp_mes == 2 && aux->paciente.nascimento.tm_mday == 29)
                                {
                                    system("cls");
                                    printf("\n----------------------------------------------------------\n\n");
                                    printf("\n\tData invalida!! \n\nInforme um valor menor que 29 para Fevereiro.\n\n");
                                    printf("\n----------------------------------------------------------\n\n\n");
                                    system("pause");
                                    break;
                                }
                                aux->paciente.nascimento.tm_mon = tmp_mes;
                                aux->paciente.nascimento.tm_mon--;
                                atualizar_log_alteracao(aux, data_atual);
                                system("cls");
                                printf("\n----------------------------------------------------------\n");
                                printf("\n  Mudança do mês de nascimento registrado!!\n\n  Mês: %02d\n\n", aux->paciente.nascimento.tm_mon+1);
                                printf("\n----------------------------------------------------------\n\n\n");
                                system("pause");
                                break;

                            case 3:  //Ano
                                system("cls");
                                printf("\n Ano de nascimento registrado: %d \n", aux->paciente.nascimento.tm_year+1900);
                                setbuf(stdin, 0);
                                printf("\n----------------------------------------------------------\n");
                                printf("\n\n  Digite ano de nascimento a ser alterado: \n\n\tAno: ");
                                scanf("%d", &aux->paciente.nascimento.tm_year);
                                aux->paciente.nascimento.tm_year = aux->paciente.nascimento.tm_year - 1900;
                                atualizar_log_alteracao(aux, data_atual);
                                system("cls");
                                printf("\n----------------------------------------------------------\n");
                                printf("\n  Mudança do ano de nascimento registrado!!\n\n  Ano: %d\n\n", aux->paciente.nascimento.tm_year+1900);
                                printf("\n----------------------------------------------------------\n\n\n");
                                system("pause");
                                break;

                            default:
                                if(opcao2 < 0 || opcao2 > 3)
                                {
                                    system("cls");
                                    printf("\n----------------------------------------------------------\n\n");
                                    printf("\n\tOpção invalida, digite novamente!!\n\n");
                                    printf("\n----------------------------------------------------------\n\n\n");
                                    system("pause");
                                    break;
                                }
                            }
                        }
                        while(opcao2 != 0);
                        break;

                    case 6:  //Endereço
                        do
                        {
                        system("cls");
                        printf("\n\n----------------------Endereco:----------------------------\n");
                        printf("\n  Digite a Opção desejada:\n");
                        setbuf(stdin, 0);
                        printf("\n\t0 - Voltar\n\n\t1 - CEP: %s\n\n\t2 - Rua: %s\n\t3 - Número: %s", aux->paciente.moradia.cep, aux->paciente.moradia.rua, aux->paciente.moradia.numero);
                        printf("\n\n\t4 - Cidade: %s\n\t5 - Bairro: %s\n\t6 - Estado: %s", aux->paciente.moradia.cidade, aux->paciente.moradia.bairro, aux->paciente.moradia.estado);
                        printf("\n\n----------------------------------------------------------\n\n  Opção: ");
                        scanf("%d", &opcao3);
                        char estado_tmp[3];
                        char *estado_tmp2;
                            switch(opcao3)
                            {
                                case 0:
                                    break;

                                case 1:  //CEP
                                    system("cls");
                                    printf("\n CEP registrado: %s  \n", aux->paciente.moradia.cep);
                                    setbuf(stdin, 0);
                                    printf("\n----------------------------------------------------------\n");
                                    printf("\n  Digite o CEP do paciente a ser alterado: \n\n\tCEP: ");
                                    scanf("%s", aux->paciente.moradia.cep);
                                    atualizar_log_alteracao(aux, data_atual);
                                    system("cls");
                                    printf("\n----------------------------------------------------------\n");
                                    printf("\n  Mudança de CEP registrado!!\n\n  CEP: %s\n", aux->paciente.moradia.cep);
                                    printf("\n----------------------------------------------------------\n\n\n");
                                    system("pause");
                                    break;

                                case 2:  //Rua
                                    system("cls");
                                    printf("\n Rua registrada: %s  \n", aux->paciente.moradia.rua);
                                    setbuf(stdin, 0);
                                    printf("\n----------------------------------------------------------\n");
                                    printf("\n  Digite o nome da rua do paciente a ser alterado: \n\n\tRua: ");
                                    fgets(aux->paciente.moradia.rua, 49, stdin);
                                    atualizar_log_alteracao(aux, data_atual);
                                    system("cls");
                                    printf("\n----------------------------------------------------------\n\n");
                                    printf("\n  Mudança de rua registrado!!\n\n  Rua: %s\n", aux->paciente.moradia.rua);
                                    printf("\n----------------------------------------------------------\n\n\n");
                                    system("pause");
                                    break;

                                case 3:  //Número
                                    system("cls");
                                    printf("\n Número da casa registrada: %s  \n", aux->paciente.moradia.numero);
                                    setbuf(stdin, 0);
                                    printf("\n----------------------------------------------------------\n");
                                    printf("\n  Digite o número da rua do paciente a ser alterado: \n\n\tNúmero: ");
                                    scanf("%s", aux->paciente.moradia.numero);
                                    atualizar_log_alteracao(aux, data_atual);
                                    system("cls");
                                    printf("\n----------------------------------------------------------\n\n");
                                    printf("\n  Mudança de número da casa registrada!!\n\n  Número: %s\n", aux->paciente.moradia.numero);
                                    printf("\n----------------------------------------------------------\n\n\n");
                                    system("pause");
                                    break;

                                case 4:  //Cidade
                                    system("cls");
                                    printf("\n Cidade registrada: %s  \n", aux->paciente.moradia.cidade);
                                    setbuf(stdin, 0);
                                    printf("\n----------------------------------------------------------\n");
                                    printf("\n  Digite a cidade do paciente a ser alterado: \n\n\tCidade: ");
                                    fgets(aux->paciente.moradia.cidade, 49, stdin);
                                    atualizar_log_alteracao(aux, data_atual);
                                    system("cls");
                                    printf("\n----------------------------------------------------------\n\n");
                                    printf("\n  Mudança de cidade registrada!!\n\n  Cidade: %s\n", aux->paciente.moradia.cidade);
                                    printf("\n----------------------------------------------------------\n\n\n");
                                    system("pause");
                                    break;

                                case 5:  //Bairro
                                    system("cls");
                                    printf("\n Bairro registrado: %s  \n", aux->paciente.moradia.bairro);
                                    setbuf(stdin, 0);
                                    printf("\n----------------------------------------------------------\n");
                                    printf("\n  Digite a bairro do paciente a ser alterado: \n\n\tBairro: ");
                                    fgets(aux->paciente.moradia.bairro, 49, stdin);
                                    atualizar_log_alteracao(aux, data_atual);
                                    system("cls");
                                    printf("\n----------------------------------------------------------\n\n");
                                    printf("\n  Mudança de bairro registrado!!\n\n  Bairro: %s\n", aux->paciente.moradia.bairro);
                                    printf("\n----------------------------------------------------------\n\n\n");
                                    system("pause");
                                    break;

                                case 6:  //Estado
                                    system("cls");
                                    printf("\n Estado registrado: %s  \n", aux->paciente.moradia.estado);
                                    setbuf(stdin, 0);
                                    printf("\n----------------------------------------------------------\n");
                                    printf("\n  Digite a estado do paciente a ser alterado: \n\n\tEstado: ");
                                    scanf("%s", estado_tmp);
                                    estado_tmp2 = converter_string_maiusculo(estado_tmp);
                                    strcpy (aux->paciente.moradia.estado, estado_tmp2);
                                    atualizar_log_alteracao(aux, data_atual);
                                    system("cls");
                                    printf("\n----------------------------------------------------------\n\n");
                                    printf("\n  Mudança de estado registrado!!\n\n  Estado: %s\n", aux->paciente.moradia.estado);
                                    printf("\n----------------------------------------------------------\n\n\n");
                                    system("pause");
                                    break;

                                default:
                                    if(opcao3 < 0 || opcao3 > 6)
                                    {
                                    system("cls");
                                    printf("\n----------------------------------------------------------\n");
                                    printf("\n\tOpção invalida, digite novamente!!\n");
                                    printf("\n----------------------------------------------------------\n\n\n");
                                    system("pause");
                                    }
                                }
                            }
                            while(opcao3 != 0);
                            break;

                    case 7: //Comorbidade
                        do
                        {
                            system("cls");
                            printf("\n------------------------Paciente-------------------------\n");
                            if (aux->paciente.grupo_de_risco == false)
                            {
                                printf("\n\tGrupo de risco: Não.\n");
                            }
                            else
                            {
                                printf("\n\tGrupo de risco: Sim.\n");
                                printf("\n\tLista de comorbidades:\n");
                                int qnt_comorbidades = atoi(aux->paciente.comorbidades[0]);
                                for (int i = 1; i <= qnt_comorbidades; i++)
                                {
                                    printf("\n\t%d - %s", i, aux->paciente.comorbidades[i]);
                                }
                            }
                            printf("\n-----------------Alterar Comorbidades:--------------------\n");
                            setbuf(stdin, 0);
                            printf("\n\n\t0 - Sair\n\n\t1 - Adicionar\n\n\t2 - Excluir Lista\n\n  Opção: ");
                            scanf("%d", &opcao4);
                            switch(opcao4)
                            {
                                case 0:
                                    break;

                                case 1:
                                    system("cls");
                                    qnt_comorbidades = atoi(aux->paciente.comorbidades[0]);
                                    if(qnt_comorbidades <= 9)
                                    {
                                    setbuf(stdin, 0);
                                    printf("\n----------------------------------------------------------\n\n");
                                    printf("\n\tDigite a comorbidade:\n\n\tComorbidade: ");
                                    fgets(aux->paciente.comorbidades[qnt_comorbidades+1], 25, stdin);
                                    qnt_comorbidades++;
                                    itoa(qnt_comorbidades, qnt_comorbidades_str, 10);
                                    strcpy(aux->paciente.comorbidades[0], qnt_comorbidades_str);
                                    aux->paciente.grupo_de_risco = true;
                                    atualizar_log_alteracao(aux, data_atual);
                                    }
                                    else
                                    {
                                        printf("\n----------------------------------------------------------\n\n");
                                        printf("\n  Quantidade de máxima de comorbidades atingida!!\n");
                                        printf("\n----------------------------------------------------------\n\n\n");
                                        system("pause");
                                    }
                                    break;

                                case 2:
                                    system("cls");
                                    do
                                    {
                                    setbuf(stdin, 0);
                                    printf("\n----------------------------------------------------------\n");
                                    printf("\n\n  Confirma a exclusão da lista de comorbidades? (S/N)\n\n    (S/N): ");
                                    scanf("%c", &tmp_excluir);
                                    }
                                    while (!((tmp_excluir == 's') || (tmp_excluir == 'S') || (tmp_excluir == 'n') || (tmp_excluir == 'N')));

                                        if(tmp_excluir == 'S' || tmp_excluir == 's')
                                        {
                                            int qnt_comorbidades = 0;
                                            itoa(qnt_comorbidades, qnt_comorbidades_str, 10);
                                            strcpy(aux->paciente.comorbidades[0], qnt_comorbidades_str);

                                            if (aux->paciente.idade >= 65)
                                                aux->paciente.grupo_de_risco = true;
                                            else
                                                aux->paciente.grupo_de_risco = false;


                                            for (int i = 1; i<10; i++)
                                            {
                                                aux->paciente.comorbidades[i][0] = '\0';
                                            }
                                            setbuf(stdin, 0);
                                            atualizar_log_alteracao(aux, data_atual);
                                        }
                                        else if((tmp_excluir == 'N') || (tmp_excluir == 'n'))
                                        {
                                            setbuf(stdin, 0);
                                            break;
                                        }
                                    break;

                                default:
                                    if(opcao4 < 0 || opcao4 > 2)
                                    {
                                        system("cls");
                                        printf("\n----------------------------------------------------------\n");
                                        printf("\n\tOpção invalida, digite novamente!!\n");
                                        printf("\n----------------------------------------------------------\n\n\n");
                                        system("pause");
                                    }
                            }
                        }
                        while (opcao4 != 0);

                        default:
                            if(opcao < 0 || opcao > 7)
                            {
                                system("cls");
                                printf("\n----------------------------------------------------------\n");
                                printf("\n\tOpção invalida, digite novamente!!\n");
                                printf("\n----------------------------------------------------------\n\n\n");
                                system("pause");
                                break;
                            }
                        }
                    }
                    while(opcao != 0);
                    paciente_alterado = aux->paciente;
                    break;
                }
            aux = aux->proximo;
        }
    }
salvar_alteracoes(arquivo, cpf_str, paciente_alterado);
carregar_bancodedados(arquivo, indice_cpf, data_atual);
}

void gerar_txt(struct tm *data_atual)
{
    char arquivotxt[] = {"grupoderisco.txt"};
    FILE *file = fopen(arquivotxt, "w");
    No *aux_txt = inicio_gdr.lista_gdr;
    if(file != NULL)
    {
        fprintf(file, "Arquivo gerado pelo usuario: %s -- CPF: %s", usuario_logado.usuario_nome, usuario_logado.usuario_cpf);
        fprintf(file, " -- Data: %d/%d/%d\n", data_atual->tm_mday, data_atual->tm_mon+1, data_atual->tm_year+1900);
        fprintf(file, "CEP, Idade\n");

        if (aux_txt->proximo == NULL)
        {
            fprintf(file, "%s, ", aux_txt->paciente.moradia.cep);
            fprintf(file, "%d\n", aux_txt->paciente.idade);
        }
        else
        {
            while (aux_txt->proximo != NULL)
            {
                fprintf(file, "%s, ", aux_txt->paciente.moradia.cep);
                fprintf(file, "%d\n", aux_txt->paciente.idade);
                aux_txt = aux_txt->proximo;
            }
            if (aux_txt->proximo == NULL)
            {
                fprintf(file, "%s, ", aux_txt->paciente.moradia.cep);
                fprintf(file, "%d\n", aux_txt->paciente.idade);
            }
        }

    }
    fclose(file);
    printf("\n--------------------------------------------------------------------\n\n");
    printf("\n\n\tArquivo Texto '%s' foi gerado com sucesso! \n\n\tVerifique a pasta de origem do software!!\n\n", arquivotxt);
    printf("\n\n--------------------------------------------------------------------\n\n\n\n");
    system("pause");
    return;
}

void main()
{
    setlocale(LC_ALL, "Portuguese_Brazil");
    struct tm *data_atual;
    time_t segundos;
    time(&segundos);
    data_atual = localtime(&segundos);
    struct login lista_usuarios[100];
    char arquivo_login[] = {"usuarios.dat"};
    int quantidade_usuarios = carregar_lista_usuarios(arquivo_login, lista_usuarios);
    login_senha(arquivo_login, lista_usuarios, quantidade_usuarios);
    int opcao_main;
    No indice_cpf[100];   //Lista onde cada posição é o inicio da sub-lista com valor correspondente ao indice gerado pelo cpf (ultimos 2 digitos)
    char arquivo[] = {"pacientes.dat"};
    inicializar_lista(indice_cpf);

    do
    {
        int pacientes_cadastrados = carregar_bancodedados(arquivo, indice_cpf, data_atual);
        system("cls");
        printf("\n  Usuário: %.25s             ", usuario_logado.usuario_nome);
        printf("  Data: %d/%d/%d\n", data_atual->tm_mday, data_atual->tm_mon+1, data_atual->tm_year+1900);
        printf("\n  (%d/%d) pacientes no grupo de risco!!", contador_grupo_de_risco, pacientes_cadastrados);
        printf("\n\n----------------------------------------------------------\n");
        printf("\n            Software de suporte ao Covid-19\n");
        printf("\n  Digite a opção desejada:\n");
        setbuf(stdin, 0);
        printf("\n\n\t0 - Salvar e Sair\n\n\t1 - Cadastrar Paciente\n\n\t2 - Alterar dados Cadastrados\n\n\t3 - Consulta de paciente por CPF\n\n\t4 - Gerar Arquivo Texto\n");
        printf("\n\n----------------------------------------------------------\n\n  Opcão: ");
        scanf("%i", &opcao_main);
        switch(opcao_main)
        {
            case 0:
                break;
            case 1:
                system("cls");
                cadastrar(arquivo, indice_cpf, data_atual);
                break;
            case 2:
                system("cls");
                alterar(indice_cpf, arquivo, data_atual);
                break;
            case 3:
                system("cls");
                consultar(indice_cpf);
                break;
            case 4:
                system("cls");
                gerar_txt(data_atual);
                break;
            default:
                if(opcao_main < 0 || opcao_main > 4)
                {
                    system("cls");
                    printf("\n----------------------------------------------------------\n");
                    printf("\n\n\tOpção invalida, digite novamente!!\n\n");
                    printf("\n----------------------------------------------------------\n\n\n");
                    system("pause");
                    break;
            }
        }
    }
    while(opcao_main != 0);

printf("\n\n\n\tFinalizando....\n\n");
return;
}
