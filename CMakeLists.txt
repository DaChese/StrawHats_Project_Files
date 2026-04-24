cmake_minimum_required(VERSION 3.16)
project(strawhats_chocan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(strawhats
    src/main.cpp

    # UI
    src/ui/ProviderTerminalUI.cpp
    src/ui/ManagerTerminalUI.cpp
    src/ui/OperatorTerminalUI.cpp

    # Controllers
    src/controllers/ProviderLoginController.cpp
    src/controllers/MemberValidationController.cpp
    src/controllers/ServiceEntryController.cpp
    src/controllers/OperatorController.cpp

    # Repositories
    src/repositories/MemberRepository.cpp
    src/repositories/ProviderRepository.cpp
    src/repositories/ServiceRepository.cpp
    src/repositories/ServiceRecordRepository.cpp

    # Generators
    src/generators/ProviderDirectoryGenerator.cpp
    src/generators/WeeklyBatchGenerator.cpp

    # Utilities
    src/util/Csv.cpp
    src/util/StringUtil.cpp
)

target_include_directories(strawhats PRIVATE src)
