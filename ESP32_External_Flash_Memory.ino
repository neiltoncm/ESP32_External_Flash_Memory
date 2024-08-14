/*
Este exemplo utiliza integração ESP IDF para conectar uma memória flash externa com base no exemplo:
https://github.com/espressif/esp-idf/tree/master/examples/storage/ext_flash_fatfs

Utilize os exemplos de leitura e gravação utilizando o sistema de arquivo LittleFS:
https://github.com/lorol/LITTLEFS/blob/master/examples/LITTLEFS_test/LITTLEFS_test.ino

COMPONENTES:
Microcontrolado: ESP32 DevKit v1
Memória Flash: W25Q128FVSG

PINOUT W25Q->ESP:
VCC:  3.3v
CS:   5     (SS)
DO:   19    (MISO)
GND:  GND
CLK:  18    (SCK)
DI:   23    (MOSI)

RETORNO ESPERADO NO SERIAL MONITOR
Inicializando memória SPI flash externa. Pinos definidos:
MOSI: 23  MISO: 19   SCLK: 18   CS: 5
Memória flash externa iniciada, capacidade=8192 KB, ID=0x15679511
Adicionando partição à memória flash, nome=littlefs, capacidade=8192 KB
Lista de partições:
Partição nvs, subtype 2, offset 0x36864 , capacidade 20 kB
Partição otadata, subtype 0, offset 0x57344 , capacidade 8 kB
Partição spiffs, subtype 130, offset 0x2686976 , capacidade 1408 kB
Partição coredump, subtype 3, offset 0x4128768 , capacidade 64 kB
Partição littlefs, subtype 131, offset 0x0 , capacidade 8192 kB
Formatando partição com o sistema LittleFS
...
Memória flash externa e sistema de arquivos LittleFS iniciados com sucesso: Capacidade total: 8388608 Bytes, espaço utilizado: 8192 Bytes.
*/

//Importação de dependências
#include "LittleFS.h"
#include "esp_flash.h"
#include "esp_flash_spi_init.h"
#include "esp_vfs_fat.h"
#include "soc/spi_pins.h"

//Definições de parâmetros SPI e pinos
#define HOST_ID SPI3_HOST
#define SPI_DMA_CHAN SPI_DMA_CH_AUTO
//Os pinos MOSI, MISO, CLK e CS está definido como padrão,
//Utilizando ESP32 DevKit V1 é identificado automaticamente de acrodo com soc/spi_pins.h
//Faça alteração de acordo com a conexão dos pinos
#define PIN_MOSI SPI3_IOMUX_PIN_NUM_MOSI
#define PIN_MISO SPI3_IOMUX_PIN_NUM_MISO
#define PIN_CLK SPI3_IOMUX_PIN_NUM_CLK
#define PIN_CS SPI3_IOMUX_PIN_NUM_CS

//Declaração de variáveis
const char* base_path = "/littlefs";
const char* partition_label = "littlefs";

//Declaração de funções/classes
static esp_flash_t* init_ext_flash(void);
static const esp_partition_t* add_partition(esp_flash_t* ext_flash, const char* partition_label);
static void list_data_partitions(void);

void setup() {
  //Inicia porta serial
  Serial.begin(115200);

  //Inicia memória flash externa de acordo com pinos definidos
  esp_flash_t* flash = init_ext_flash();
  if (flash == NULL)
    return;

  //Adiciona partição para memória flash externa
  add_partition(flash, partition_label);

  //Imprime lista de partição da memória flash externa
  list_data_partitions();

  //Inicia sistema de arquivos (O sistema FAT apresentou limitações de uso)
  if (!LittleFS.begin(true, base_path, 5, partition_label)) {
    Serial.println("Ocorreu uma falha ao montar LittleFS");
    return;
  }

  // Realiza formatação da memória flash externa de acordo com partição. Não é obrigatória a execução.
  // Pode ser feita de maneira pontual de acordo com as características e necessidades de cada projeto
  Serial.println("Formatando partição com o sistema LittleFS");
  LittleFS.format();

  // Memória flash externa inicializado
  Serial.print("Memória flash externa e sistema de arquivos LittleFS iniciados com sucesso: ");
  Serial.print("Capacidade total: ");
  Serial.print(LittleFS.totalBytes());
  Serial.print(" Bytes, espaço utilizado: ");
  Serial.print(LittleFS.usedBytes());
  Serial.println(" Bytes.");
}

void loop() {}

static esp_flash_t* init_ext_flash(void) {

  //Definição da comunicação SPI e da memória flash
  const spi_bus_config_t bus_config = {
    .mosi_io_num = PIN_MOSI,
    .miso_io_num = PIN_MISO,
    .sclk_io_num = PIN_CLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
  };
  const esp_flash_spi_device_config_t device_config = {
    .host_id = HOST_ID,
    .cs_io_num = PIN_CS,
    .io_mode = SPI_FLASH_DIO,
    .cs_id = 0,
    .freq_mhz = ESP_FLASH_40MHZ,
  };

  // Inicialização da comunicação SPI
  Serial.println("Inicializando memória SPI flash externa. Pinos definidos:");
  Serial.println("MOSI: " + String(bus_config.mosi_io_num) + "  MISO: " + String(bus_config.miso_io_num) + "   SCLK: " + String(bus_config.sclk_io_num) + "   CS: " + String(device_config.cs_io_num));
  ESP_ERROR_CHECK(spi_bus_initialize(HOST_ID, &bus_config, SPI_DMA_CHAN));

  // Adiciona memória flash à comunicação SPI
  esp_flash_t* ext_flash;
  ESP_ERROR_CHECK(spi_bus_add_flash_device(&ext_flash, &device_config));

  // Inicialização da memória flash
  esp_err_t err = esp_flash_init(ext_flash);
  if (err != ESP_OK) {
    Serial.println("Ocorreu uma falha na inicialização da memória flash externa: " + String(esp_err_to_name(err)));
    return NULL;
  }

  // Exibição de informações da memória flash externa
  uint32_t id;
  ESP_ERROR_CHECK(esp_flash_read_id(ext_flash, &id));
  Serial.println("Memória flash externa iniciada, capacidade=" + String(ext_flash->size / 1024) + " KB, ID=0x" + String(id));

  return ext_flash;
}

static const esp_partition_t* add_partition(esp_flash_t* ext_flash, const char* partition_label) {

  // Adicionando partição a memória flash externa
  Serial.println("Adicionando partição à memória flash, nome=" + String(partition_label) + ", capacidade=" + String(ext_flash->size / 1024) + " KB");
  const esp_partition_t* littleFS_partition;
  const size_t offset = 0;
  ESP_ERROR_CHECK(esp_partition_register_external(ext_flash, offset, ext_flash->size, partition_label, ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_LITTLEFS, &littleFS_partition));

  return littleFS_partition;
}

static void list_data_partitions(void) {

  // Exibição das partições adicionadas à memória flash externa
  Serial.println("Lista de partições:");
  esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);
  for (; it != NULL; it = esp_partition_next(it)) {
    const esp_partition_t* part = esp_partition_get(it);
    Serial.println("Partição " + String(part->label) + ", subtype " + String(part->subtype) + ", offset 0x" + String(part->address) + " , capacidade " + String(part->size / 1024) + " kB");
  }
  esp_partition_iterator_release(it);
}