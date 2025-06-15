# 15.06.2025 17:20:45 Genrated by cbp2cmake V2.12.0 Finale X64 
project(hsguard LANGUAGES C)
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set (CMAKE_INSTALL_PREFIX /usr )
include_directories(../../tools)
add_executable(hsguard ../../tools/ctools/c_crc32.c ../../tools/tools.c g_cfg.c g_color.c g_finddupes.c g_helper.c g_info.c g_list.c g_longtable.c g_remove.c g_sql.c g_testdb.c g_update.c hsguard.c)
find_package(SQLite3)
if (SQLite3_FOUND)
  include_directories(${SQLite3_INCLUDE_DIRS})
  target_link_libraries(hsguard ${SQLite3_LIBRARIES})
else()
     message( FATAL_ERROR "SQLite3 not found. make sure you have: apt install sqlite3 libsqlite3-dev\n" )
endif()
install(TARGETS hsguard RUNTIME DESTINATION bin)
add_custom_target(
	hsguard.man ALL
	DEPENDS hsguard.8
  VERBATIM
add_custom_command(TARGET hsguard.man
	COMMAND ${CMAKE_COMMAND} -E rm -f hsguard.8.gz
  COMMAND gzip -k -9 hsguard.8
  VERBATIM
)
install(FILES hsguard.8.gz DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man8)
