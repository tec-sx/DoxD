cmake_minimum_required (VERSION 3.14)
set(CRYENGINE_DIR "C:/Users/tecsx/AppData/Local/Programs/CRYENGINE Launcher/Engines/crytek/cryengine-57-lts/5.7.1" CACHE STRING "CRYENGINE root directory.")
set(TOOLS_CMAKE_DIR "${CRYENGINE_DIR}/Tools/CMake")

set(PROJECT_BUILD 1)
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/../")

include("${TOOLS_CMAKE_DIR}/InitialSetup.cmake")
include("${TOOLS_CMAKE_DIR}/CommonOptions.cmake")

add_subdirectory("${CRYENGINE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/CRYENGINE")

include("${TOOLS_CMAKE_DIR}/Configure.cmake")
start_sources()

sources_platform(ALL)
add_sources("DoxDCore_uber.cpp"
    PROJECTS DoxD.Game
    SOURCE_GROUP "Root"
		"GamePlugin.cpp"
		"StdAfx.cpp"
		"GamePlugin.h"
		"StdAfx.h"
)
add_sources("Actor_uber.cpp"
    PROJECTS DoxD.Game
    SOURCE_GROUP "Actor"
		"Actor/Fate.h"
)
add_sources("Attachments_uber.cpp"
    PROJECTS DoxD.Game
    SOURCE_GROUP "Attachments"
		"Attachments/Attachment.cpp"
		"Attachments/Attachment.h"
)
add_sources("Components_uber.cpp"
    PROJECTS DoxD.Game
    SOURCE_GROUP "Components"
		"Components/SpawnPoint.cpp"
		"Components/Bullet.h"
		"Components/SpawnPoint.h"
)
add_sources("Actor_uber.cpp"
    PROJECTS DoxD.Game
    SOURCE_GROUP "Components\\\\Actor"
		"Components/Actor/Actor.cpp"
		"Components/Actor/ActorComponent.cpp"
		"Components/Actor/ActorControllerComponent.cpp"
		"Components/Actor/Actor.h"
		"Components/Actor/ActorComponent.h"
		"Components/Actor/ActorControllerComponent.h"
		"Components/Actor/ActorDefinitions.h"
)
add_sources("Attachments_uber.cpp"
    PROJECTS DoxD.Game
    SOURCE_GROUP "Components\\\\Attachments"
		"Components/Attachments/AttachmentComponent.cpp"
		"Components/Attachments/AttachmentComponent.h"
)
add_sources("Camera_uber.cpp"
    PROJECTS DoxD.Game
    SOURCE_GROUP "Components\\\\Camera"
		"Components/Camera/CameraModes.cpp"
		"Components/Camera/ICameraMode.cpp"
		"Components/Camera/PlayerCamera.cpp"
		"Components/Camera/CameraModes.h"
		"Components/Camera/ICameraMode.h"
		"Components/Camera/PlayerCamera.h"
)
add_sources("Ladder_uber.cpp"
    PROJECTS DoxD.Game
    SOURCE_GROUP "Components\\\\Ladder"
		"Components/Ladder/LadderComponent.cpp"
		"Components/Ladder/LadderComponent.h"
)
add_sources("Player_uber.cpp"
    PROJECTS DoxD.Game
    SOURCE_GROUP "Components\\\\Player"
		"Components/Player/Player.cpp"
		"Components/Player/PlayerComponent.cpp"
		"Components/Player/Player.h"
		"Components/Player/PlayerComponent.h"
)
add_sources("Input_uber.cpp"
    PROJECTS DoxD.Game
    SOURCE_GROUP "Components\\\\Player\\\\Input"
		"Components/Player/Input/PlayerInputComponent.cpp"
		"Components/Player/Input/PlayerInputComponent.h"
)
add_sources("Console_uber.cpp"
    PROJECTS DoxD.Game
    SOURCE_GROUP "Console"
		"Console/GameCVars.cpp"
		"Console/GameCVars.h"
)
add_sources("Contracts_uber.cpp"
    PROJECTS DoxD.Game
    SOURCE_GROUP "Contracts"
		"Contracts/IActor.h"
		"Contracts/IItem.h"
)
add_sources("DynamicResponseSystem_uber.cpp"
    PROJECTS DoxD.Game
    SOURCE_GROUP "DynamicResponseSystem"
		"DynamicResponseSystem/ConditionDistanceToEntity.cpp"
		"DynamicResponseSystem/ConditionDistanceToEntity.h"
)
add_sources("Utility_uber.cpp"
    PROJECTS DoxD.Game
    SOURCE_GROUP "Utility"
		"Utility/AutoEnum.cpp"
		"Utility/CryHash.cpp"
		"Utility/CryWatch.cpp"
		"Utility/EnumUtils.cpp"
		"Utility/LocalizationUtility.cpp"
		"Utility/StringUtils.cpp"
		"Utility/AutoEnum.h"
		"Utility/CryHash.h"
		"Utility/CryWatch.h"
		"Utility/EnumUtils.h"
		"Utility/LocalizationUtility.h"
		"Utility/StateMachine.h"
		"Utility/StringUtils.h"
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CVarOverrides.h")
    add_sources("NoUberFile"
        PROJECTS Game
        SOURCE_GROUP "Root"
            "CVarOverrides.h"
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/EngineDefineOverrides.h")
    add_sources("NoUberFile"
        PROJECTS Game
        SOURCE_GROUP "Root"
            "EngineDefineOverrides.h"
    )
endif()
end_sources()

CryGameModule(DoxD.Game FORCE_SHARED PCH "StdAfx.cpp" SOLUTION_FOLDER "Project")

target_include_directories(${THIS_PROJECT}
PRIVATE
    "${CRYENGINE_DIR}/Code/CryEngine/CryCommon"
    "${CRYENGINE_DIR}/Code/CryEngine/CryAction"
    "${CRYENGINE_DIR}/Code/CryEngine/CrySchematyc/Core/Interface"
    "${CRYENGINE_DIR}/Code/CryPlugins/CryDefaultEntities/Module"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "[Cc]lang")
    target_compile_options(${THIS_PROJECT} PRIVATE
        -Wno-unused-variable
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-parentheses
        -Wno-switch
        -Wno-format
        -Wno-dynamic-class-memaccess
        -Wno-unused-private-field
        -Wno-unused-value
        -Wno-invalid-offsetof
        -Wno-multichar
        -Wno-char-subscripts
        -Wno-null-conversion
        -Wno-empty-body
        -Wno-unused-lambda-capture
        -Wno-unused-function
        -Wno-tautological-constant-out-of-range-compare
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${THIS_PROJECT} PRIVATE
        -Wno-unused-function
        -Wno-unused-value
        -Wno-unused-variable
        -Wno-aligned-new
        -Wno-int-in-bool-context
        -Wno-invalid-offsetof
        -Wno-comment
        -Wno-sign-compare
        -Wno-unused-but-set-variable
        -Wno-maybe-uninitialized
        -Wno-misleading-indentation
        -Wno-unused-result
        -Wno-unknown-pragmas
        -Wno-multichar
        -Wno-strict-aliasing
        -Wno-char-subscripts
        -Wno-conversion-null
        -Wno-reorder
        -Wno-parentheses
        -Wno-format
        -Wno-switch
    )
endif()


if(OPTION_ENGINE)
    if(NOT EXISTS "${CRYENGINE_DIR}/Code/Sandbox/EditorQt")
        add_library(Editor STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
        set_target_properties(Editor PROPERTIES LINKER_LANGUAGE CXX)
        if (WIN32)
            set_visual_studio_debugger_command(Editor "${CRYENGINE_DIR}/bin/win_x64/Sandbox.exe" "-project \"${PROJECT_DIR}/DoxD.Game.cryproject\"")
        endif()
    endif()
else()
    add_library(GameLauncher STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
    set_target_properties(GameLauncher PROPERTIES LINKER_LANGUAGE CXX)
    if (WIN32)
        set_visual_studio_debugger_command(GameLauncher "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"${PROJECT_DIR}/DoxD.Game.cryproject\"")
    endif()

    add_library(Editor STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
    set_target_properties(Editor PROPERTIES LINKER_LANGUAGE CXX)
    if (WIN32)
        set_visual_studio_debugger_command(Editor "${CRYENGINE_DIR}/bin/win_x64/Sandbox.exe" "-project \"${PROJECT_DIR}/DoxD.Game.cryproject\"")
    endif()

    add_library(GameServer STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
    set_target_properties(GameServer PROPERTIES LINKER_LANGUAGE CXX)
    if (WIN32)
        set_visual_studio_debugger_command(GameServer "${CRYENGINE_DIR}/bin/win_x64/Game_Server.exe" "-project \"${PROJECT_DIR}/DoxD.Game.cryproject\"")
    endif()
endif()

# Set StartUp project in Visual Studio
set_solution_startup_target(GameLauncher)

if (WIN32)
    set_visual_studio_debugger_command( ${THIS_PROJECT} "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"${PROJECT_DIR}/DoxD.Game.cryproject\"" )
endif()

#BEGIN-CUSTOM
# Make any custom changes here, modifications outside of the block will be discarded on regeneration.
#END-CUSTOM