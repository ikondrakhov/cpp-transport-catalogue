# Проект: транспортный справочник

<p>
Программа позволяет строить маршруты между остановками, узнавать информацию о маршрутах и рисовать карту маршрутов в svg формате. На вход программе подается JSON с данными и запросами. Данные представляют из себя списки остановок и маршрутов и параметры для отрисовки карты маршрутов, запросы позволяют получить информацию о маршруте, маршрутки проходящие через остановку, построить маршрут или нарисовать карту маршрутов.
</p>
<p>
В проекте используется формат JSON для получения данных и запросов, а также формат svg для отрисовки карты маршрутов. Для построения и дальнейшей обработки запросов используется граф. Для поиска оптимального маршрута осуществляется поиск в ширину на графе.
</p>
Для сборки проекта используется:
cmake,
C++17

## Запуск

Для сборки проекта нужен CMake версии 3.8 и выше

Windows:
```cmd
cd transport-catalogue
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
Сгенерированный фаил будет находиться в папке build/Release

Linux:
```sh
cd transport-catalogue
mkdir build
cd build
cmake ..
make
```