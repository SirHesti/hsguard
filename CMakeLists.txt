# 18.06.2025 12:27:43 Genrated by cbp2cmake V2.13.0 Finale X64 
project(hsguard LANGUAGES C)
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set (CMAKE_INSTALL_PREFIX /usr )
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(hsguard ctools/c_crc32.c tools.c g_cfg.c g_color.c g_finddupes.c g_helper.c g_info.c g_list.c g_longtable.c g_remove.c g_sql.c g_testdb.c g_update.c hsguard.c)
find_package(SQLite3)
if (SQLite3_FOUND)
  include_directories(${SQLite3_INCLUDE_DIRS})
  target_link_libraries(hsguard ${SQLite3_LIBRARIES})
else()
     message( FATAL_ERROR "SQLite3 not found. make sure you have: apt install sqlite3 libsqlite3-dev\n" )
endif()
install(TARGETS hsguard RUNTIME DESTINATION bin)
set(outfiles "")
set(CMAKE_INSTALL_MANDIR "${CMAKE_INSTALL_PREFIX}/share/man")
function(add_manpage file suffix lang)
	if ("" STREQUAL "${lang}")
		set(parsedinput "")
	else()
		set(parsedinput "-${lang}")
	endif()

	set(input  "${CMAKE_CURRENT_SOURCE_DIR}/${file}${parsedinput}.${suffix}")
	set(output "${CMAKE_CURRENT_BINARY_DIR}/${file}${parsedinput}.${suffix}.gz")
	add_custom_command(
		OUTPUT "${output}"
		COMMAND gzip -k -9 -c ${input} > ${output}
		DEPENDS "${input}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		VERBATIM)
	list(APPEND outfiles "${output}")
	set(outfiles "${outfiles}" PARENT_SCOPE)
	install(FILES ${output} DESTINATION ${CMAKE_INSTALL_MANDIR}/${lang}/man${suffix} RENAME "${file}.${suffix}.gz" )
endfunction()
add_manpage(hsguard 8 "")
add_manpage(hsguard 8 "de")
add_custom_target(man ALL DEPENDS "${outfiles}")
