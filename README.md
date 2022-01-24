
# Interruptor Inteligente - Smart Lab

Controle distribuído de sistemas de iluminação com ESP32

## Descrição
O sketch em questão foi desenvolvido como parte de um sistema para automatização completa de um dos laboratórios do ICMC - Instituto de Ciências Matemáticas e de Computação. Ele faz o controle via rede de um sistema de iluminação. Para isso utiliza de um microcontrolador ESP32 integrado a um módulo relé, o qual faz o "chaveamento" da rede elétrica que alimenta a lâmpada. Para isso, utiliza o protocolo de comunicação [MQTT](https://developer.ibm.com/br/articles/iot-mqtt-why-good-for-iot/), muito comum na área de IoT.  

## Instalação
O projeto possui a necessidade da execução de uma série de processos para a execução, os quais serão dividos em duas partes, a primeira é referente à configuração do ambiente necessário para que a aplicação funcione; a segunda se refere à configuração e montagem do _hardware_ (ESP32).

### Configuração do Ambiente
Como o protocolo de comunicação usado é o MQTT faz se necessário o uso de um _Broker_, o qual pode ser público ou privado. Caso trate-se de um _broker_ público qualquer um pode ter acesso as mensagens trocadas ou ainda se passar por um dispositivo. Porém, não há a necessidade de nenhuma configuração de ambiente, bastando apenas utilizar o endereço do broker em questão, [aqui](https://test.mosquitto.org/) há um exemplo de broker público.
Caso a opção escolhida seja a criação de um broker privado, para maior privacidade e controle de acesso desse, há a necessidade de hardware para execução deste, o qual pode ser uma Raspberry Pi, um computador, ou até ser hospedado em núvem. Existem vários Brokers disponíveis para serem usados e qualquer um deles pode ser utilizado no projeto, porém no desenvolvimento do projeto o broker escolhido foi o [Aedes](https://github.com/moscajs/aedes), o qual pode ser rodado como um nó do _framework_ [Node-RED](https://nodered.org/). Esse broker não é recomendado para o ambiente de produção, porém é muito empregado em ambientes de teste e desenvolvimento. 
Como se trata de um nó do Node-RED, é necessário, incialmente, a instalação deste. Para isso, no terminal, deve-se usar o seguinte comando: 
```console
sudo npm install -g --unsafe-perm node-red
```
Caso o Sistema Operacional for o Windows, basta remover o sudo do inicio do comando. Com isso o node-red estará instalado na máquina. A instalação aqui desmonstrada é feita localmente, outras opções podem ser encontradas [aqui](https://nodered.org/docs/getting-started/).
Em seguida, basta executar o Node-RED, acessar sua interface via navegador, o qual por padrão se encontra no endereço da máquina e na porta 1880. Em seguida, acessando o menu localizado no canto superior direito há a opção "Manage palette", nela novos nós podem ser instalados para utilização. Clicando na aba Install basta buscar por "aedes" e clicar no botão "Install". Bastando, por fim, apenas arrastar o novo nó instalado para um dos Flows do Node-RED e, clicando duas vezes nesse nó, configurar a porta desejada, por padrão o MQTT usa a porta 1883.
Com isso o ambiente necessário já está configurado e pronto para ser usado para no projeto. Lembre-se do `endereço` da máquina na qual o broker foi instalado e também a `porta` empregada para a comunicação, pois serão utilizados na configuração do ESP32.

### Configuração do ESP32
Para configuração e gravação da lógica na memória do microcontrolador foi utilizada a [Arduino IDE](https://www.arduino.cc/en/Main/Software_), para a utilização dessa IDE com o ESP32 há a necessidade da realização de alguns processos, caso não os tenha feito, basta seguir [este tutorial](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/). 
As bibliotecas necessárias são:
* [NTP Client](https://github.com/arduino-libraries/NTPClient)
* [ArduinoJSON](https://github.com/bblanchon/ArduinoJson)
* [Wi-Fi Manager](https://github.com/tzapu/WiFiManager)
* [PubSubClient](https://github.com/knolleary/pubsubclient)

Com a IDE configurada corretamente a configuração pode ser feita, no repositório há o arquivo [credencials.h](https://gitlab.com/smartlaboratory/2021/smart-light/-/blob/main/credencials.h), neste encontram-se as variáveis de ambiente que precisam ser configuradas, como por exemplo o número dos pinos de GPIO que se encontram o relé e o sensor de Luminancia LDR, há também o `endereço` do broker MQTT e a sua `porta`, configurados anteriormente. No arquivo de credenciais presente no repositório os pinos escolhidos são o 34 para o sensor LDR, o qual deve ser um pino analógico, dado o funcionamento do sensor; e o pino do relé é o 4.
Além disso, na fase atual do projeto, os tópicos utilizados na comunicação são implementados de maneira direta no arquivo [topics.hpp](https://gitlab.com/smartlaboratory/2021/smart-light/-/blob/main/topics.hpp), estes precisam ser alterados também conforme a necessidade. A estrutura dos tópicos será apresentada posteriormente.
Com isso temos o código corretamente configurado, bastando enviá-lo para a ESP32 via IDE. Há agora a necessidade da montagem do esquemático, a imagem a seguir apresenta a configuração utilizada.
![Esquemático utilizado](https://gitlab.com/smartlaboratory/2021/smart-light/-/blob/main/img/esquematico.jpg)

Após a montagem do esquemático o projeto está pronto.

## Comunicação
### Estrutura de tópicos
A comunicação usa o protocolo MQTT, como já foi citado, possuindo um tópico específico para cada um dos dispositivos utilizados. Esse tópico é divido em 3 "campos", o primeiro correspondente ao ID do ambiente, o segundo é o módulo com o qual se está comunicando, no caso do sistema de iluminação esse campo será sempre 'light', no caso do módulo de controle de temperatura, esse campo tem o valor 'aircon', já o terceiro campo é o ID do dispositivo a ser controlado. E o dígito de separação desses campos é um ponto ".", o tópico completo utilizado no código desse repositório é: `"5.light.2"`. Como já dito esses campos podem ser alterados no arquivo topics.hpp presente no repositório.
Portanto, para o controle da iluminação basta enviar a mensagem para o tópico definido.

### Conteudo da Mensagem
O conteúdo da mensagem é passado dentro de uma estrutura [JSON](https://www.json.org/json-en.html), para melhor manipulação. Os comandos são divididos em campos segundo o seguinte [protocolo](https://docs.google.com/spreadsheets/d/1hY8HoQSQWAoaamR6uw4nxDGdlij4B_vqstKHH4BYxnc/edit?usp=sharing), ele foi criado para diminuir o tamanho da mensagem, transformando os nomes dos campos para números, em vez de strings. Um exemplo de JSON enviado será apresentado e explicado a seguir:
```json
{
    "0": 1,
    "23": "12345",
    "21": 1,
    "12": 0
}
```
O exemplo de mensagem possui 4 campos, "0", "23", "21" e "12". O campo "23" representa o ID da mensagem enviada, o "21" é o estado em que a lâmpada deve se encontrar, utilizando 1 como valor quando é desejado que a lâmpada ligue, 0 para que desligue e 2 para inverter o estado em que se encontra, ou seja, se estiver ligada o microcontrolador a desligará, e vice versa. Já o campo "12" foi utilizado pensando uma futura implementação de modo de operação, que traria ao dispositivo 2 modos de operação, modo 1: comandos instantâneos e modo 2: comandos agendados por horário, o valor 0 representaria uma ação executada instantaneamente e 1 uma ação agendada, o segundo modo demandaria de mais campos contendo a hora desse agendamento de comando. Por fim, o campo "0" é o que auxilia o microcontrolador saber quantos campos estão presentes na mensagem, nesse protocolo cada um dos campos da mensagem possui um "peso" e estes são somados e inseridos no campo 0. A tabela exibe os pesos dos campos, seus significados e a gama de valores que estes podem assumir:
Campo | Significado do Campo  | Peso do campo | Possíveis valores
--------- | ------ | ------ | --------------
"0" | Indica os campos da mensagem | 0 | Inteiro: soma dos pesos dos outros campos da mensagem
"23" |ID da mensagem | 0 | String: valor do ID da mensagem
"21" | Estado que a lâmpada será setada | 1 | Inteiro: ON: 1 / OFF: 0
"12" | Modo de funcionamento |  0 | Inteiro: Manual: 0 / Agendado: 1
"13" | Horário para lampada ser ligada | 4 | horario com formatação "dd/mm/yy hh:mm:ss" 
"14" | Horário para lampada ser desligada | 8 | horario com formatação "dd/mm/yy hh:mm:ss" 

Os campos que sempre estarão presentes nas mensagem apresentam peso 0. Vale lembrar que a implementação do modo de operação por agendamento ainda não foi implementado.

Sempre que um comando é enviado ao ESP32 uma mensagem é enviada por ele no tópico "response", contendo também um JSON, apresentado a seguir: 
```json
{
    "0": 2,
    "23": "12345",
    "21": 0,
    "device_type": 2,
    "time_on": 12.65,
    "intensity": 0
}
```
A mensagem de resposta não atribui "peso" aos campos, como na mensagem do envio do comando, nesse caso o campo "0" representa o ID do dispositivo, o qual está presente também no tópico de envio do comando, já os campos "23" e "21" possuem o mesmo significado da mensagem anterior. A tabela a seguir indicará o que cada um dos campos da mensagem de resposta representa, para melhor visualização: 
Campo | Significado do Campo  | Possíveis valores
--------- | ------  | --------------
"0" | ID do dispositivo  | Inteiro: ID do dispositivo
"23" |ID da mensagem | String: valor do ID da mensagem
"21" | Estado que a lâmpada será setada  | Inteiro: ON: 1 / OFF: 0
"device_type" | Módulo da mensagem | Inteiro: light: 2 / aircon: 1
"time_on" | Tempo total que a lâmpada ficou ligada  | Inteiro 
"intensity" | Intensidade medida pelo sensor LDR  | Inteiro: 0 a 4095 

O campo "time_on" é enviado somente quando a lâmpada é desligada.

Por fim, o microcontrolador envia também uma mensagem quando já se conectou a rede, se conectou ao broker MQTT e está pronto para operação. Essa mensagem é enviada no tópico corresponde à sua localização com um adicional de ".ready", ou seja, o tópico ficaria, para a implementação aqui presente, "5.light.2.ready". O conteúdo dessa mensagem é um JSON: { "status": "on" }.


