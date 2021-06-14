# Compressor BMP
Compressor de imagens BMP (e respectivo descompressor) feito para a disciplina de Multimídia.

## Integrantes
- [André Luís Mendes Fakhoury](https://github.com/andrefakhoury/)
- [Lucas Fernandes Turci](https://github.com/lucasturci)

## Como usar

**Compressão**

	make compress
	./compress <imagem bmp>

O resultado da execução é um arquivo binário `out.bin`, contendo a imagem comprimida, além da impressão no `stdout` da taxa de compressão atingida.

**Descompressão**

	make decompress
	./decompress out.bin

O resultado da execução é um arquivo `img_save.bmp`, a imagem recuperada com a descompressão.

## Etapas de Compressão

O compressor é dividido nas seguintes etapas:

### 1. Leitura do arquivo BMP

Nesta etapa, o arquivo BMP é lido, e as informações de *header* e *imagem* são armazenadas. Para isso, são utilizadas structs para o cabeçalho e para cada pixel RGB. As principais funções deste módulo estão nos códigos fonte *bmp.c* e *bmp.h*.

### 2. Preparação de blocos

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

### 3. Transformada DCT e Quantização

Nesta etapa, cada bloco será sujeito à transformada DCT (*Discrete Cosine Transform*). Para aumentar a eficiência do código, os cossenos e raízes necessários para as contas foram pré-processados, diminuindo um pouco o custo computacional. A respectiva inversa da transformada também foi implementada desta forma.

Após a transformada DCT, é realizada a quantização de cada bloco. A quantização foi realizada com *fator de compressão* 1, e a tabela utilizada para ela pode ser enviada por parâmetro para a função, para facilitar modificações. Porém, por simplicidade, está sendo utilizada uma tabela padrão de quantização, dada a seguir:

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

### 4. Separação entre DCs e ACs

Após aplicar a transformada DCT nos blocos, cada `block[i]` é rearranjado na ordem de zig-zag, conforme a imagem:

![zig zag](https://i.stack.imgur.com/6kHBB.gif)

Agora, os primeiros valores de `block[i]` são concatenados em um vetor `DCs`, e o resto, em um vetor `ACs`.

### 5. Codificação por Diferença

Em seguida, a codificação por diferenças é realizada sobre o vetor `DCs`, gerando inteiros com menores valores absolutos. Depois, a codificação por entropia é feita sobre esses valores, na qual simbolos são gerados concatenando-se os prefixos de huffman - que representam o tamanho em bits dos valores - com os valores em si.

Os prefixos de huffman usados foram:

|   |   |   |   |   |   |   |   |   |   |   |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |
| `010` | `011` | `100` | `00` | `101` | `110` | `1110` | `11110` | `111110` | `1111110` | `11111110` |

onde a primeira linha representa o tamanho do valor (em bits) e a segunda o prefixo de huffman correspondente. Essa tabela pode ser encontrada no código como `prefTable[]` em `differential.c`, que também implementa todas as funções relacionados à codificação por diferença.

Exemplo de símbolo gerado para 6 (`110`) :
> ` 00 110`

### 6. Codificação por Carreira

A codificação por carreira também é realizada sobre o vetor `ACs`. Como a transformação DCT seguida da quantização gera muitos valores zero nos ACs, a codificação por carreira consegue reduzir bastante a quantidade de informação necessária para representar o vetor.

Nessa codificação, carreiras de zero são representadas por um par: `(#zeros, próximo valor)`. O número de zeros em uma carreira é limitado em 15, e neste caso o `próximo valor` também pode ser um zero. 

Diferente da codificação por diferença, na codificação por carreira o número de símbolos gerado (neste caso, os pares) é desconhecido. Por isso o par `(0, 0)` é usado como delimitador para o fim da sequência.

Depois disso, a codificação por entropia é feita sobre esses pares, no qual os símbolos são gerados concatenando-se o prefixo de huffman com o segundo valor do par.

Aqui, o prefixo de huffman é usado para representar o número de zeros na carreira e o número de bits de `próximo valor`. A carreira é limitada em 15, portanto, para reduzir o número de símbolos necessários.

Exemplo de símbolo gerado para (5, 3):
> `1111111001 11`

A tabela de prefixos de huffman é muito grande para ser apresentada aqui, mas ela é armazenada em nosso código na matriz `_rle_prefixes[][]`, em `rle.c`, que também implementa todas as funções relacionadas à codificação por carreira. A função `read_rle_prefixes()` é responsável por carregar esses prefixos do arquivo `rle_prefixes.txt`. Tanto esses prefixos quanto os da codificação por diferença foram tirados do padrão JPEG como descrito no PDF da Poli, disponibilizado no Tidia.

### 7. Escrita no arquivo de saída.

De forma a conseguir estruturar bem o código, tanto a função de codificação por carreira quanto a de codificação por diferença escrevem seus resultados em um vetor de `INT_PAIR` no formato `(símbolo, tamanho em bits do símbolo)`. Dessa forma, outra função pode ficar responsável por apenas escrever esses símbolos no arquivo de saída.

Portanto, a função `write_bits()`, de `bits.c` recebe um vetor de `INT_PAIR` e é responsável por escrever esses símbolos no arquivo de saída. 

Cada canal é escrito separadamente, vindo primeiro o Y, seguido do Cb e do Cr. Dentro de cada canal, os símbolos DCs são escritos primeiro, seguido dos símbolos ACs.

#### 7.1 Método de escrita
O método usado foi utilizar uma variável `int` como buffer, e iterar por todos os símbolos e ir escrevendo os símbolos no buffer, até que ele fique cheio. Neste caso, o buffer é escrito no arquivo e é "limpo" para novas escritas. Se alguma coisa sobra no buffer após o fim da iteração, este valor é "shiftado" para se alinhar à esquerda do último byte do buffer, que é, finalmente, escrito no arquivo.

Aqui, os bits "lixo" que sobram no último inteiro não importam, porque é sabido quando parar de ler símbolos na etapa de descompressão, por causa do delimitador da codificação por carreiras.

### 8. Taxa de compressão

A taxa de compressão, que é impressa em `stdout` como resultado da execução da compressão, foi calculada como: `tamanho original / tamanho comprimido`, como descrito [aqui](https://en.wikipedia.org/wiki/Data_compression_ratio).

### 9. Considerações sobre descompressão

Até agora foram discutidas apenas as etapas de compressão, porque as etapas de descompressão são análogas (são só as inversas). A estrutura de cada etapa é bem semelhante, e, para cada função das etapas de compressão, tentou-se implementar a função inversa correspondente.

No entanto, as funções de codificação por entropia (por diferença e carreira) não tem inversas, devido às buscas pelos prefixos de huffman - `find_rle_prefix()` em `rle.c` e `find_differential_prefix` em `differential.c` - na etapa da leitura dos bits da descompressão já conseguir os símbolos originais antes da codificação por entropia.

Tirando essa mudança da estrutura do código, todas as outras etapas tem códigos similares para as etapas inversas, e podem ser entendidos facilmente comparando `compress.c` e `decompress.c` lado a lado.

## Conclusão

Com os algoritmos usados, conseguimos uma taxa de compressão muito boa (cerca de 16 para a imagem da Lenna).