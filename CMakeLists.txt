# CMakeList.txt: проект CMake для Compressor; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.10)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Compressor")

find_package(CLI11 CONFIG REQUIRED)
find_package(zstd CONFIG REQUIRED)

# Link [compress.dll]
add_library(compress SHARED "compress/compress.cpp")

set_target_properties(compress PROPERTIES 
	DEFINE_SYMBOL COMPRESS_EXPORTS
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

target_include_directories(compress PUBLIC ${CMAKE_SOURCE_DIR}/compress)

target_link_libraries(compress PRIVATE
	zstd::libzstd
)

# Compile EXECUTABLE
add_executable (Compressor "source/main.cpp")

set_target_properties(Compressor PROPERTIES 
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

target_link_libraries(Compressor PRIVATE compress CLI11::CLI11)
# TODO: Добавьте тесты и целевые объекты, если это необходимо.
