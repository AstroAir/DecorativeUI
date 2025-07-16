set(SOURCES
    FileWatcher.cpp
    HotReloadManager.cpp
)

add_library(HotReload ${SOURCES} FileWatcher.hpp HotReloadManager.hpp)
target_include_directories(HotReload PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
