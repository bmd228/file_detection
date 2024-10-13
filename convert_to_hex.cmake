# convert_to_hex.cmake
# ���������� �������� ��� ���������� ��������� ����������
cmake_policy(SET CMP0053 NEW)

# ������� ����� ��� �������� � ��������� ������
set(INPUT_FILE ${RESOURCE_FILE_PATH})
set(OUTPUT_HEADER ${GENERATED_HEADER})

# �������� ������� �������� �����
if(NOT EXISTS ${INPUT_FILE})
    message(FATAL_ERROR "Input file does not exist: ${INPUT_FILE}")
endif()

# ��������� ����� �����
get_filename_component(FILENAME "${INPUT_FILE}" NAME_WE)
string(TOUPPER "${FILENAME}" TARGET_NAME_UPPER)
string(MAKE_C_IDENTIFIER "${TARGET_NAME_UPPER}" TARGET_NAME_UPPER)
string(TOLOWER "${FILENAME}" FILENAME)
string(MAKE_C_IDENTIFIER "${FILENAME}" FILENAME)

# ������ �������� ����� � �������������� � hex
file(READ ${INPUT_FILE} BYTES HEX)
string(REGEX REPLACE "(..)" "0x\\1, " BYTES "${BYTES}")
string(JOIN "\n" ARRAY_DECLARATIONS 
    "const std::uint8_t ${FILENAME}[] = { ${BYTES} };"
)

# ������������ ���������
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

# ������ ��������� � ����
file(WRITE ${OUTPUT_HEADER} "${BASIC_HEADER}")

message(STATUS "Successfully generated header file: ${OUTPUT_HEADER}")
