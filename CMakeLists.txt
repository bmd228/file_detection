# CMakeList.txt: проект CMake для file_detection; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("file_detection")

find_package(unofficial-libmagic REQUIRED)
# Указываем исходный бинарный файл
set(RESOURCE_FILE_PATH ${unofficial-libmagic_DICTIONARY})

# Генерация C++ заголовочного файла с данными из бинарного файла
set(GENERATED_HEADER "${CMAKE_BINARY_DIR}/generation/embedded_data.h")

# Чтение содержимого бинарного файла и его запись в C++ массив вручную
# Генерируем заголовочный файл с массивом данных
add_custom_command(
    OUTPUT ${GENERATED_HEADER}
    COMMAND ${CMAKE_COMMAND} -DRESOURCE_FILE_PATH=${RESOURCE_FILE_PATH} -DGENERATED_HEADER=${GENERATED_HEADER} -P ${CMAKE_SOURCE_DIR}/convert_to_hex.cmake 
    DEPENDS ${RESOURCE_FILE_PATH}
)

# Указываем, что этот сгенерированный файл должен быть включен в сборку
add_custom_target(embed_resources ALL DEPENDS ${GENERATED_HEADER})
# Добавьте источник в исполняемый файл этого проекта.
add_executable (file_detection "file_detection.cpp" "file_detection.h")
add_dependencies(file_detection embed_resources)
# Добавляем путь к сгенерированным заголовкам
target_include_directories(file_detection PRIVATE "${CMAKE_BINARY_DIR}/generation")

target_link_libraries(file_detection PRIVATE unofficial::libmagic::libmagic)   
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET file_detection PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
