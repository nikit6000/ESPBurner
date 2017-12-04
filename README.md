# ESPBurner

ESPBurner позволяет вывадить на OLED экранчик данные из MSI After Burner.

## Как пользоваться

* Прошить ESP8266
* Скомпилировать и запустить (от имени администратора!) MAHMSharedMemorySample

### Необходиные библиотеки для Arduino IDE

Для успешной сборки прошивки необходимы следующие библиотеки:

* [ESP8266 Arduino](https://github.com/esp8266/Arduino) - ESP8266 для Arduino IDE.
* [ESP8266 oled ssd1306](https://github.com/squix78/esp8266-oled-ssd1306) - библиотека для работы с SSD1306.
* [ArduinoJson](https://github.com/bblanchon/ArduinoJson) - библиотека для работы с JSON.

### Необходимые библиотеки для Visual C++

Для успешной компиляции необходимы следующте библиотеки:

* [CppRestSDK](https://github.com/Microsoft/cpprestsdk) - ESP8266 для Arduino IDE.

## Управление

На данный момент управлять экранчиком можно только при помощи комманд:

```
* connect <ssid> <password> - Подключиться к точке доступа WiFi.
* scan - Получить список доступных точек доступа.
* sethost <host> <port> - адресс и порт сервера данных (локальный ip пк с запушенной MAHMSharedMemorySample )
* gethost - Получить адресс текущего сервера данных.
* setpage <page> - Задать страницу данных. 
* setnode <name> - Задать ноду для отображения
  * global - Нода отображающая FPS, и информацию о системе.
  * gpu0 - Информация о первой видеокарте.
  * gpu1 - Информация о второй видиокарте.
  * ...
  * gpu<n-1> - Информация о n-ой видеокарте.
```
## Автор

* **Тархов Никита** - *Идея, разработка* - [nikit6000](https://github.com/nikit6000)

## Лиценция

Этот проект лицензируется по лицензии MIT - смотри [LICENSE.md](LICENSE) для подробностей.
