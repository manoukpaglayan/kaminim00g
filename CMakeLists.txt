cmake_minimum_required(VERSION 3.22)
project(kam00g VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory($ENV{HOME}/JUCE JUCE)

juce_add_plugin(kam00g
    COMPANY_NAME "kam"
    PLUGIN_MANUFACTURER_CODE Kamg
    PLUGIN_CODE Km00
    FORMATS VST3 AU
    PRODUCT_NAME "kam00g"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES "Instrument|Synth"
    AU_MAIN_TYPE "kAudioUnitType_MusicDevice"
)

juce_generate_juce_header(kam00g)

target_sources(kam00g PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Oscillator.cpp
    Source/LadderFilter.cpp
    Source/Envelope.cpp
    Source/SynthVoice.cpp
    Source/KamLookAndFeel.cpp
)

target_compile_definitions(kam00g PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(kam00g PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_plugin_client
    juce::juce_dsp
    juce::juce_gui_basics
    juce::juce_graphics
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
