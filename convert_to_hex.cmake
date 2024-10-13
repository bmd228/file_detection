# convert_to_hex.cmake
# Установите политику для упрощенной обработки переменных
cmake_policy(SET CMP0053 NEW)

# Задание путей для входного и выходного файлов
set(INPUT_FILE ${RESOURCE_FILE_PATH})
set(OUTPUT_HEADER ${GENERATED_HEADER})

# Проверка наличия входного файла
if(NOT EXISTS ${INPUT_FILE})
    message(FATAL_ERROR "Input file does not exist: ${INPUT_FILE}")
endif()

# Получение имени файла
get_filename_component(FILENAME "${INPUT_FILE}" NAME_WE)
string(TOUPPER "${FILENAME}" TARGET_NAME_UPPER)
string(MAKE_C_IDENTIFIER "${TARGET_NAME_UPPER}" TARGET_NAME_UPPER)
string(TOLOWER "${FILENAME}" FILENAME)
string(MAKE_C_IDENTIFIER "${FILENAME}" FILENAME)

# Чтение входного файла и преобразование в hex
file(READ ${INPUT_FILE} BYTES HEX)
string(REGEX REPLACE "(..)" "0x\\1, " BYTES "${BYTES}")
string(JOIN "\n" ARRAY_DECLARATIONS 
    "const std::uint8_t ${FILENAME}[] = { ${BYTES} };"
)

# Формирование заголовка
set(BASIC_HEADER "\
// NOLINT
#ifndef ${TARGET_NAME_UPPER}_EMBED_FILES
#define ${TARGET_NAME_UPPER}_EMBED_FILES

#include <cstdint>
namespace embedded {
    ${ARRAY_DECLARATIONS}
}
#endif // ${TARGET_NAME_UPPER}_EMBED_FILES
")

# Запись заголовка в файл
file(WRITE ${OUTPUT_HEADER} "${BASIC_HEADER}")

message(STATUS "Successfully generated header file: ${OUTPUT_HEADER}")
