cmake_minimum_required( VERSION 3.0 )
project( commander
         VERSION 1.0.0
         LANGUAGES CXX
         HOMEPAGE_URL https://github.com/olorkeaz/Commander
         DESCRIPTION "Parsing end execute string command"
         )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

add_library( ox::commander INTERFACE )
target_include_directories( ox::commander INTERFACE
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                            $<BUILD_INTERFACE:include>
                            )


include( CMakePackageConfigHelpers )
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/commanderConfigVersion.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
	)

install( TARGETS ox::commander EXPORT commanderTargets )

install( EXPORT commanderTargets
         FILE commanderTargets.cmake
         NAMESPACE ox::commander::
         DESTINATION lib/cmake/ox/commander
         )

install( DIRECTORY include/ DESTINATION include )

install(
	FILES "${CMAKE_CURRENT_BINARY_DIR}/commanderConfigVersion.cmake"
	DESTINATION lib/cmake/ox/commander
	)