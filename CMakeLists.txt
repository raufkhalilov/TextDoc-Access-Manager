    # Имя проекта
    project(ClientServerApp)

    # Устанавливаем стандарт C++
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED True)

    # Найдите заголовочные файлы для библиотеки Asio
    find_path(ASIO_INCLUDE_DIR NAMES asio.hpp PATHS /usr/include/asio)
    include_directories(${ASIO_INCLUDE_DIR})

    # Добавляем исполняемый файл для сервера
    add_executable(server server.cpp)

    # Добавляем исполняемый файл для клиента
    add_executable(client client.cpp)  

    # Создаем папку data в build
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data)