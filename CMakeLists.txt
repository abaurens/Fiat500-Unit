cmake_minimum_required(VERSION 3.16)
project(Fiat500-Unit  VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Gui
    Core
    DBus
    Widgets
    Bluetooth
    Concurrent
    SerialPort
    LinguistTools
)
find_package(Qt6 6.5 OPTIONAL_COMPONENTS
    Widgets
)

qt_standard_project_setup()

qt_add_executable(Fiat500-Unit
    WIN32 MACOSX_BUNDLE

    src/main.cpp

    src/Ref.hpp
    src/Scope.hpp
    src/MainWindow.cpp
    src/MainWindow.hpp

    src/dbus/Types.hpp src/dbus/Types.cpp

    src/dbus/bluez/Object.hpp src/dbus/bluez/Object.cpp
    src/dbus/bluez/Device.hpp src/dbus/bluez/Device.cpp
    src/dbus/bluez/Adapter.hpp src/dbus/bluez/Adapter.cpp
    src/dbus/bluez/Manager.hpp src/dbus/bluez/Manager.cpp

    src/widgets/sizes.hpp

    src/widgets/LogPanel.hpp src/widgets/LogPanel.cpp
    src/widgets/DevicePanel.hpp src/widgets/DevicePanel.cpp
    src/widgets/AdapterPanel.hpp src/widgets/AdapterPanel.cpp
)

qt_add_translations(
    TARGETS Fiat500-Unit
    TS_FILES lang/Fiat500-Unit_fr_FR.ts
    TS_FILES lang/Fiat500_Unit_en_US.ts
)

target_include_directories(Fiat500-Unit PRIVATE
    src
    include
)

target_link_libraries(Fiat500-Unit PRIVATE
    Qt::Core
    Qt::DBus
    Qt::Widgets
    Qt::Bluetooth
)

# Add Windows specific sources
if(WIN32)
    target_sources(Fiat500-Unit PUBLIC
    )

    target_link_libraries(Fiat500-Unit PRIVATE
        user32
    )
endif()

# Add linux specific sources
if(UNIX AND NOT MACOS)
    target_sources(Fiat500-Unit PUBLIC
    )
endif()

install(TARGETS Fiat500-Unit
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Fiat500-Unit
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})
