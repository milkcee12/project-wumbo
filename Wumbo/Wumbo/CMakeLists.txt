set(GAMENAME Wumbo)

# This grabs all files in this directory that end with .cpp, .h, and .hpp
file(GLOB source_files CONFIGURE_DEPENDS "*.cpp" "*.h" "*.hpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${source_files})

# Don't change this
add_executable(${GAMENAME} ${source_files} )

target_include_directories(${GAMENAME} PUBLIC "C:/Program Files/mosquitto/devel")
target_link_libraries(${GAMENAME} SDL2 SDL2main SDL2_image SDL2_mixer SDL2_ttf "C:/Program Files/mosquitto/devel/mosquittopp.lib")

# Copy .dll files
if (WIN32)
	file(GLOB_RECURSE
		DLLS 
		"${PROJECT_SOURCE_DIR}/Libraries/*.dll")
	message(Status "Directory being searched was: ${PROJECT_SOURCE_DIR}/Libraries/**/x64/*.dll")
	foreach(CurrentDll IN LISTS DLLS)
		message(STATUS "Current DLL: ${CurrentDll}")
		add_custom_command(TARGET ${GAMENAME} 
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy "${CurrentDll}" "${CMAKE_CURRENT_BINARY_DIR}"
			COMMENT "Copy dll file to ${CMAKE_CURRENT_BINARY_DIR} directory" VERBATIM
		)
	endforeach()

	add_custom_command(TARGET ${GAMENAME} 
		POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy "C:/Program Files/mosquitto/mosquitto.dll" "${CMAKE_CURRENT_BINARY_DIR}"
		COMMENT "Copy dll file to ${CMAKE_CURRENT_BINARY_DIR} directory" VERBATIM
	)
endif()
