#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//T1 de Baixo Nível
//manipulação de imagem por meio de pixel
//Ana Carolina Policarpo Schmitt

typedef struct {
    unsigned char r, g, b;
} Pixel;

typedef struct {
    int largura;
    int altura;
    Pixel** pixels;
} Imagem;

Imagem* criarImagem(int largura, int altura) {
    Imagem* img = (Imagem*)malloc(sizeof(Imagem));
    if (!img) {
        printf("Erro! de alocação de memória para imagem.\n");
        exit(1);
    }
    img->largura = largura;
    img->altura = altura;
    img->pixels = (Pixel**)malloc(altura * sizeof(Pixel*));
    if (!img->pixels) {
        printf("Erro! de alocação de memória para pixels.\n");
        exit(1);
    }
    for (int i = 0; i < altura; i++) {
        img->pixels[i] = (Pixel*)malloc(largura * sizeof(Pixel));
        if (!img->pixels[i]) {
            printf("Erro! de alocação de memória para linha de pixels.\n");
            exit(1);
        }
    }
    return img;
}

void liberarImagem(Imagem* img) {
    for (int i = 0; i < img->altura; i++) {
        free(img->pixels[i]);
    }
    free(img->pixels);
    free(img);
}

Imagem* lerImagemPPM(char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) {
        printf("Erro! ao abrir o arquivo %s.\n", nomeArquivo);
        exit(1);
    }
    
    char formato[3];
    int largura, altura, maxValor;

    if (fscanf(arquivo, "%s", formato) != 1) {
        printf("Erro! ao ler o formato da imagem.\n");
        exit(1);
    }

    if (formato[0] != 'P' || formato[1] != '6') {
        printf("Formato de imagem inválido!!! Deve ser P6 (formato de imagens simples RGB).\n");
        exit(1);
    }

    if (fscanf(arquivo, "%d %d", &largura, &altura) != 2) {
        printf("Erro! ao ler as dimensões da imagem.\n");
        exit(1);
    }

    if (fscanf(arquivo, "%d", &maxValor) != 1 || maxValor != 255) {
        printf("Erro! ao ler o valor máximo da cor ou valor inválido.\n");
        exit(1);
    }

    fgetc(arquivo);

    Imagem* img = criarImagem(largura, altura);
    for (int i = 0; i < altura; i++) {
        if (fread(img->pixels[i], sizeof(Pixel), largura, arquivo) != largura) {
            printf("Erro! ao ler os pixels da imagem.\n");
            exit(1);
        }
    }

    fclose(arquivo);
    return img;
}

void salvarImagemPPM(char* nomeArquivo, Imagem* img) {
    FILE* arquivo = fopen(nomeArquivo, "wb");
    if (!arquivo) {
        printf("Erro! ao salvar a imagem.\n");
        return;
    }
    fprintf(arquivo, "P6\n%d %d\n255\n", img->largura, img->altura);
    for (int i = 0; i < img->altura; i++) {
        fwrite(img->pixels[i], sizeof(Pixel), img->largura, arquivo);
    }
    fclose(arquivo);
}

void converterTonsCinza(Imagem* img) {
    for (int j = 0; j < img->altura; j++) {
        for (int i = 0; i < img->largura; i++) {
            unsigned char cinza = (unsigned char)(img->pixels[j][i].r * 0.299 +
                                                 img->pixels[j][i].g * 0.587 +
                                                 img->pixels[j][i].b * 0.114);
            img->pixels[j][i].r = img->pixels[j][i].g = img->pixels[j][i].b = cinza;
        }
    }
}

void gerarNegativa(Imagem* img) {
    for (int j = 0; j < img->altura; j++) {
        for (int i = 0; i < img->largura; i++) {
            img->pixels[j][i].r = 255 - img->pixels[j][i].r;
            img->pixels[j][i].g = 255 - img->pixels[j][i].g;
            img->pixels[j][i].b = 255 - img->pixels[j][i].b;
        }
    }
}

void gerarRaioX(Imagem* img) {
    converterTonsCinza(img);
    float fator = 1.5;
    for (int j = 0; j < img->altura; j++) {
        for (int i = 0; i < img->largura; i++) {
            unsigned char raioX = (unsigned char)pow(img->pixels[j][i].r, fator);
            img->pixels[j][i].r = img->pixels[j][i].g = img->pixels[j][i].b = raioX;
        }
    }
}

Imagem* rotacionar90Graus(Imagem* img) {
    Imagem* novaImagem = criarImagem(img->altura, img->largura);
    for (int i = 0; i < img->largura; i++) {
        for (int j = 0; j < img->altura; j++) {
            novaImagem->pixels[i][j] = img->pixels[img->altura - j - 1][i];
        }
    }
    liberarImagem(img);
    return novaImagem;
}

void gerarEnvelhecida(Imagem* img) {
    float fator = 0.1;
    for (int j = 0; j < img->altura; j++) {
        for (int i = 0; i < img->largura; i++) {
            img->pixels[j][i].b = (unsigned char)(img->pixels[j][i].b * (1 - fator));
            img->pixels[j][i].r = (unsigned char)(img->pixels[j][i].r * (1 + fator)) + 10;
            img->pixels[j][i].g = (unsigned char)(img->pixels[j][i].g * (1 + fator)) + 10;
        }
    }
}

void exibirMenu(Imagem* img) {
    int opcao;
    printf("Escolha uma opção:\n");
    printf("1. Converter para Tons de Cinza\n");
    printf("2. Gerar Imagem Negativa\n");
    printf("3. Gerar Imagem Raio-X\n");
    printf("4. Rotacionar Imagem 90 Graus\n");
    printf("5. Gerar Imagem Envelhecida\n");
    printf("Digite a opção desejada: ");
    scanf("%d", &opcao);

    switch(opcao) {
        case 1:
            converterTonsCinza(img);
            salvarImagemPPM("imagem_cinza.ppm", img);
            break;
        case 2:
            gerarNegativa(img);
            salvarImagemPPM("imagem_negativa.ppm", img);
            break;
        case 3:
            gerarRaioX(img);
            salvarImagemPPM("imagem_raiox.ppm", img);
            break;
        case 4:
            img = rotacionar90Graus(img);
            salvarImagemPPM("imagem_rotacionada.ppm", img);
            break;
        case 5:
            gerarEnvelhecida(img);
            salvarImagemPPM("imagem_envelhecida.ppm", img);
            break;
        default:
            printf("Opção inválida.\n");
    }
}

int main() {
    char nomeArquivo[100];
    printf("Digite o nome do arquivo PPM (ex.: imagem.ppm): ");
    scanf("%s", nomeArquivo);

    Imagem* img = lerImagemPPM(nomeArquivo);

    if (img->largura < 400 || img->altura < 400) {
        printf("Erro!!! A imagem deve ter no mínimo 400x400 pixels.\n");
        liberarImagem(img);
        return 1;
    }

    exibirMenu(img);
    liberarImagem(img);

    return 0;
}
