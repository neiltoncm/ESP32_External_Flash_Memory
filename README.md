Este exemplo utiliza integração ESP IDF para conectar uma memória flash externa com base no exemplo:\
https://github.com/espressif/esp-idf/tree/master/examples/storage/ext_flash_fatfs

Utilize os exemplos de leitura e gravação utilizando o sistema de arquivo LittleFS:\
https://github.com/lorol/LITTLEFS/blob/master/examples/LITTLEFS_test/LITTLEFS_test.ino

COMPONENTES:\
Microcontrolado: ESP32 DevKit v1\
Memória Flash: W25Q128FVSG

PINOUT W25Q->ESP:\
VCC:  3.3v\
CS:   5     (SS)\
DO:   19    (MISO)\
GND:  GND\
CLK:  18    (SCK)\
DI:   23    (MOSI)

RETORNO ESPERADO NO SERIAL MONITOR\
Inicializando memória SPI flash externa. Pinos definidos:\
MOSI: 23  MISO: 19   SCLK: 18   CS: 5\
Memória flash externa iniciada, capacidade=8192 KB, ID=0x15679511\
Adicionando partição à memória flash, nome=littlefs, capacidade=8192 KB\
Lista de partições:\
Partição nvs, subtype 2, offset 0x36864 , capacidade 20 kB\
Partição otadata, subtype 0, offset 0x57344 , capacidade 8 kB\
Partição spiffs, subtype 130, offset 0x2686976 , capacidade 1408 kB\
Partição coredump, subtype 3, offset 0x4128768 , capacidade 64 kB\
Partição littlefs, subtype 131, offset 0x0 , capacidade 8192 kB\
Formatando partição com o sistema LittleFS\
...\
Memória flash externa e sistema de arquivos LittleFS iniciados com sucesso: Capacidade total: 8388608 Bytes, espaço utilizado: 8192 Bytes.
