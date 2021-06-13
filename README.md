# Compressor BMP
Compressor de imagens BMP (e respectivo descompressor) feito para a disciplina de Multimídia.

## Integrantes
- [André Luís Mendes Fakhoury](https://github.com/andrefakhoury/)
- [Lucas Fernandes Turci](https://github.com/lucasturci)

## Como usar
Para compilar e executar, pode-se executar o Makefile.

## Etapas de Compressão

O compressor utiliza as seguintes funcionalidades:

### 1 Leitura do arquivo BMP

Nesta etapa, o arquivo BMP é lido, e as informações de *header* e *imagem* são armazenadas. Para isso, são utilizadas structs para o cabeçalho e para cada pixel RGB. As principais funções deste módulo estão nos códigos fonte *bmp.c* e *bmp.h*.

### 2 Preparação de blocos

Neste módulo, a imagem RGB começará a ser comprimida. Para isso, haverá a conversão para YCbCr, subamostragem dos canais Cb e Cr, padding na imagem para permitir a divisão em blocos e, finalmente, a divisão dos pixels em blocos 8x8.

Os códigos para estas etapas estão nos arquivos fonte *preparation.h* e *preparation.c*.

#### 2.1 Conversão YCbCr e subamostragem

Os canais originais da imagem (RGB) são convertidos para YCbCr da seguinte forma:

- Y = 0.299 * R + 0.587 * G + 0.114 * B
- Cb = 0.564 * (B - Y)
- Cr = 0.713 * (R - Y)

Para a futura descompressão, será realizada a conversão YCbCr para RGB:

- R = clip(Y + 1.402 * Cr, 0, 255)
- G = clip(Y - 0.344 * Cb - 0.714 * Cr, 0, 255)
- B = clip(Y + 1.772 * Cb, 0, 255)

O "clip" é utilizado para restringir os valores para o intervalo [0, 255], pois, devido a erros de precisão e arredondamentos em todo o processo, alguns valores podem ficar fora deste intervalo.

A partir disso, os canais Cb e Cr sofrerão subamostragem 4:2:0 para serem comprimidos. Ou seja, de maneira menos formal: para cada valor amostrado, todos os vizinhos adjacentes serão 'ignorados'.

#### 2.2 Padding e divisão dos blocos

Nesta etapa, primeiramente, cada um dos canais (Y, Cb e Cr) soferá um padding com zeros, feito para 'ajustar' o tamanho de cada canal permitindo a separação em blocos. Como será feita a divisão em blocos 8x8, pode ser realizado um padding horizontal ou vertical, para que ambas as dimensões sejam múltiplas de 8. Note que, pela subamostragem acima mencionada, o tamanho dos canais Cb e Cr serão distintos do tamanho do canal Y.

Agora, com os canais devidamente redimensionados, é possível realizar a simples divisão em blocos. Isso é feito com uma matriz de double (`double** blocks`), em que cada `blocks[i]` possui tamanho 64 e representa um bloco. Estes estão enumerados linha a linha (primeiramente vão os blocos da primeira linha, depois os da segunda, e assim por diante).

### 3 Transformada DCT e Quantização

Nesta etapa, cada bloco será sujeito à transformada DCT (*Discrete Cosine Transform*). Para aumentar a eficiência do código, os cossenos e raízes necessários para as contas foram pré-processados, diminuindo um pouco o custo computacional. A respectiva inversa da transformada também foi implementada desta forma.

Após a transformada DCT, é realizada a quantização de cada bloco. A quantização foi realizada com *fator de compressão* 1, e a tabela utilizada para ela pode ser enviada por parâmetro, caso queira trocar. Porém, por simplicidade, está sendo utilizada uma tabela padrão de quantização, dada a seguir:

|  |  |  |  |  |  |  |  |
|----|----|----|----|----|----|----|----|
| 17 | 18 | 24 | 47 | 99 | 99 | 99 | 99 |
| 18 | 21 | 26 | 66 | 99 | 99 | 99 | 99 |
| 24 | 26 | 56 | 99 | 99 | 99 | 99 | 99 |
| 47 | 66 | 99 | 99 | 99 | 99 | 99 | 99 |
| 99 | 99 | 99 | 99 | 99 | 99 | 99 | 99 |
| 99 | 99 | 99 | 99 | 99 | 99 | 99 | 99 |
| 99 | 99 | 99 | 99 | 99 | 99 | 99 | 99 |
| 99 | 99 | 99 | 99 | 99 | 99 | 99 | 99 |


Esta tabela está disponível no arquivo `dct.h`, declarada como `int quant_table[]`. Como ela foi fixada no código, não foi necessário incluí-la no *header* do arquivo binário comprimido.

### .........



## Conclusão