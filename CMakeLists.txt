cmake_minimum_required(VERSION 3.13)
project(personal_voice_assistant)

set(CMAKE_CXX_STANDARD 17)


###############################
#### Qt Meta Object System ####
###############################

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)


######################
#### Dependencies ####
######################

# enable threading for standard library
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# find Qt packages
find_package(Qt5Core REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5Widgets REQUIRED)
#find_package(Qt5Gui REQUIRED)

# find eSpeak
find_path(LIBESPEAK_INCLUDE_DIRS
        NAMES speak_lib.h
        HINTS /usr/include/espeak)
find_library(LIBESPEAK_LIBRARIES
        NAMES espeak
        HINTS /usr/lib/ /usr/x86_64-linux-gnu/
        PATH_SUFFIXES lib)

# find DeepSpeech
find_library(LIBDEEPSPEECH_LIBRARIES libdeepspeech)
file(GLOB LIBDEEPSPEECH_INCLUDE_DIRS deepspeech/native_client/*.h)


#################
#### Linking ####
#################

set(INCLUDES
        # SpeechManager
        include/SpeechManager/SpeechManager.hpp
        include/SpeechManager/SpeechListener.hpp
        include/SpeechManager/SpeechListenerConsoleMock.hpp
        include/SpeechManager/EvalAudioChunk.hpp
        include/SpeechManager/OutputListener.hpp
        include/SpeechManager/SpeechModelManager.hpp

        # ConversationManager
        include/ConversationManager/ConversationManager.hpp
        include/ConversationManager/InputListener.hpp

        # SkillManager
        include/SkillManager/SkillManager.hpp
        include/SkillManager/SkillAPI.hpp
        include/SkillManager/Skill.hpp

        # Skills
        include/SkillManager/Skills/Echo/Echo.hpp
        include/SkillManager/Skills/EchoEn/EchoEn.hpp
        include/SkillManager/Skills/Remember/ConversationState.hpp
        include/SkillManager/Skills/Remember/Remember.hpp
        include/SkillManager/Skills/Remember/StartState.hpp
        include/SkillManager/Skills/Remember/TimeState.hpp
        include/SkillManager/Skills/Remember/ThingState.hpp
        include/SkillManager/Skills/RememberEn/ConversationStateEn.hpp
        include/SkillManager/Skills/RememberEn/RememberEn.hpp
        include/SkillManager/Skills/RememberEn/StartStateEn.hpp
        include/SkillManager/Skills/RememberEn/TimeStateEn.hpp
        include/SkillManager/Skills/RememberEn/ThingStateEn.hpp

        # Utils
        include/Utils/BlockingQueue.hpp
        include/Utils/ExclusiveStack.hpp
        include/Utils/Utils.hpp
        include/Utils/Mean.hpp
        include/Utils/Config.hpp

        # Utils Skills
        include/Utils/Skills/StateMachine.hpp
        include/Utils/Skills/State.hpp
        include/Utils/Skills/AsyncCall.hpp
        )

set(SOURCES
        src/main.cpp

        # SpeechManager
        src/SpeechManager/SpeechManager.cpp
        src/SpeechManager/SpeechListener.cpp
        src/SpeechManager/SpeechListenerConsoleMock.cpp
        src/SpeechManager/OutputListener.cpp
        src/SpeechManager/SpeechModelManager.cpp

        # ConversationManager
        src/ConversationManager/ConversationManager.cpp
        src/ConversationManager/InputListener.cpp

        # SkillManager
        src/SkillManager/SkillManager.cpp

        #Skills
        src/SkillManager/Skills/Remember/Remember.cpp
        src/SkillManager/Skills/Remember/StartState.cpp
        src/SkillManager/Skills/Remember/TimeState.cpp
        src/SkillManager/Skills/Remember/ThingState.cpp
        src/SkillManager/Skills/RememberEn/RememberEn.cpp
        src/SkillManager/Skills/RememberEn/StartStateEn.cpp
        src/SkillManager/Skills/RememberEn/TimeStateEn.cpp
        src/SkillManager/Skills/RememberEn/ThingStateEn.cpp

        # Utils
        src/Utils/Config.cpp
        )

add_executable(personal_voice_assistant

        ${LIBESPEAK_INCLUDE_DIRS}
        ${LIBDEEPSPEECH_INCLUDE_DIRS}

        ${INCLUDES}
        ${SOURCES}
        )

# add Qt packages
qt5_use_modules(personal_voice_assistant Core Multimedia Widgets)

# add Aquila
target_link_libraries(personal_voice_assistant Aquila Ooura_fft)

# add eSpeak
target_link_libraries(personal_voice_assistant ${LIBESPEAK_LIBRARIES})

# add DeepSpeech
target_link_libraries(personal_voice_assistant ${LIBDEEPSPEECH_LIBRARIES})

# add threads
target_link_libraries(personal_voice_assistant Threads::Threads)
