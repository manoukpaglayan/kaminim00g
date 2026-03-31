#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::String pid(const char* s) { return juce::String(s); }

juce::AudioProcessorValueTreeState::ParameterLayout Kam00gProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    auto pct  = [](float v) { return juce::NormalisableRange<float>(0.0f, 1.0f); (void)v; return juce::NormalisableRange<float>(0.0f, 1.0f); };
    (void)pct;

    // OSC waveforms (0–6 maps to OscWaveform enum: Sine,Tri,RevSaw,Saw,Sq,Wide,Narrow)
    params.push_back(std::make_unique<juce::AudioParameterChoice>("osc1wave", "OSC1 Wave",
        juce::StringArray{"Sine","Triangle","RevSaw","Sawtooth","Square","WidePulse","NarrowPulse"}, 3));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("osc2wave", "OSC2 Wave",
        juce::StringArray{"Sine","Triangle","RevSaw","Sawtooth","Square","WidePulse","NarrowPulse"}, 3));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("osc3wave", "OSC3 Wave",
        juce::StringArray{"Sine","Triangle","RevSaw","Sawtooth","Square","WidePulse","NarrowPulse"}, 1));

    // OSC ranges (0=Lo,1=32',2=16',3=8',4=4',5=2')
    params.push_back(std::make_unique<juce::AudioParameterChoice>("osc1range", "OSC1 Range",
        juce::StringArray{"Lo","32'","16'","8'","4'","2'"}, 3));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("osc2range", "OSC2 Range",
        juce::StringArray{"Lo","32'","16'","8'","4'","2'"}, 3));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("osc3range", "OSC3 Range",
        juce::StringArray{"Lo","32'","16'","8'","4'","2'"}, 3));

    // OSC detune
    params.push_back(std::make_unique<juce::AudioParameterFloat>("osc1tune", "OSC1 Tune",
        juce::NormalisableRange<float>(-7.0f, 7.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("osc2tune", "OSC2 Tune",
        juce::NormalisableRange<float>(-7.0f, 7.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("osc3tune", "OSC3 Tune",
        juce::NormalisableRange<float>(-7.0f, 7.0f, 0.01f), 0.0f));

    // Mixer
    params.push_back(std::make_unique<juce::AudioParameterFloat>("osc1level", "OSC1 Level",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("osc2level", "OSC2 Level",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("osc3level", "OSC3 Level",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("noiselevel", "Noise Level",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    // Filter
    params.push_back(std::make_unique<juce::AudioParameterFloat>("cutoff", "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 18000.0f, 0.1f, 0.3f), 2000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("resonance", "Resonance",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("filterenvamt", "Filter Env Amt",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("keytrack", "Key Track",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    // Filter envelope
    params.push_back(std::make_unique<juce::AudioParameterFloat>("fattack",  "F Attack",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.4f), 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("fdecay",   "F Decay",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.4f), 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("fsustain", "F Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("frelease", "F Release",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.4f), 0.3f));

    // Amp envelope
    params.push_back(std::make_unique<juce::AudioParameterFloat>("aattack",  "A Attack",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.4f), 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("adecay",   "A Decay",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.4f), 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("asustain", "A Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("arelease", "A Release",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.4f), 0.3f));

    // Modulation
    params.push_back(std::make_unique<juce::AudioParameterFloat>("modamt", "Mod Amount",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("modtofilter", "Mod→Filter", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("modtoosc",    "Mod→OSC",    false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("modmix", "Mod Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    // LFO
    params.push_back(std::make_unique<juce::AudioParameterFloat>("lforate", "LFO Rate",
        juce::NormalisableRange<float>(0.1f, 20.0f, 0.01f, 0.4f), 4.0f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("lfowave", "LFO Wave",
        juce::StringArray{"Sine","Triangle","Saw","Square"}, 0));

    // Master tune
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mastertune", "Master Tune",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f));

    // Pitch bend range (semitones)
    params.push_back(std::make_unique<juce::AudioParameterFloat>("bendrange", "Bend Range",
        juce::NormalisableRange<float>(1.0f, 12.0f, 1.0f), 2.0f));

    // Glide
    params.push_back(std::make_unique<juce::AudioParameterFloat>("glide", "Glide",
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 0.0f));

    // Drive (soft saturation)
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drive", "Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    // Chorus
    params.push_back(std::make_unique<juce::AudioParameterFloat>("chorusrate", "Chorus Rate",
        juce::NormalisableRange<float>(0.1f, 5.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("chorusdepth", "Chorus Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("chorusmix", "Chorus Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    // Master volume
    params.push_back(std::make_unique<juce::AudioParameterFloat>("volume", "Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));

    return { params.begin(), params.end() };
}

Kam00gProcessor::Kam00gProcessor()
    : AudioProcessor(BusesProperties()
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "STATE", createParameterLayout())
{
}

Kam00gProcessor::~Kam00gProcessor() {}

void Kam00gProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    voice.setSampleRate(sampleRate);
    chorus.prepare(sampleRate);
}

void Kam00gProcessor::releaseResources() {}

void Kam00gProcessor::updateVoiceParameters()
{
    auto wave = [](int i) { return static_cast<OscWaveform>(i); };
    auto rng  = [](int i) { return static_cast<OscRange>(i); };

    voice.setOsc1Waveform(wave((int)*apvts.getRawParameterValue("osc1wave")));
    voice.setOsc2Waveform(wave((int)*apvts.getRawParameterValue("osc2wave")));
    voice.setOsc3Waveform(wave((int)*apvts.getRawParameterValue("osc3wave")));

    voice.setOsc1Range(rng((int)*apvts.getRawParameterValue("osc1range")));
    voice.setOsc2Range(rng((int)*apvts.getRawParameterValue("osc2range")));
    voice.setOsc3Range(rng((int)*apvts.getRawParameterValue("osc3range")));

    voice.setOsc1FineTune(*apvts.getRawParameterValue("osc1tune"));
    voice.setOsc2FineTune(*apvts.getRawParameterValue("osc2tune"));
    voice.setOsc3FineTune(*apvts.getRawParameterValue("osc3tune"));

    voice.setOsc1Level(*apvts.getRawParameterValue("osc1level"));
    voice.setOsc2Level(*apvts.getRawParameterValue("osc2level"));
    voice.setOsc3Level(*apvts.getRawParameterValue("osc3level"));
    voice.setNoiseLevel(*apvts.getRawParameterValue("noiselevel"));

    voice.setFilterCutoff(*apvts.getRawParameterValue("cutoff"));
    voice.setFilterResonance(*apvts.getRawParameterValue("resonance"));
    voice.setFilterEnvAmount(*apvts.getRawParameterValue("filterenvamt"));
    voice.setKeyTrack(*apvts.getRawParameterValue("keytrack"));

    voice.setFilterAttack(*apvts.getRawParameterValue("fattack"));
    voice.setFilterDecay(*apvts.getRawParameterValue("fdecay"));
    voice.setFilterSustain(*apvts.getRawParameterValue("fsustain"));
    voice.setFilterRelease(*apvts.getRawParameterValue("frelease"));

    voice.setAmpAttack(*apvts.getRawParameterValue("aattack"));
    voice.setAmpDecay(*apvts.getRawParameterValue("adecay"));
    voice.setAmpSustain(*apvts.getRawParameterValue("asustain"));
    voice.setAmpRelease(*apvts.getRawParameterValue("arelease"));

    voice.setModAmount(*apvts.getRawParameterValue("modamt"));
    voice.setModToFilter(*apvts.getRawParameterValue("modtofilter") > 0.5f);
    voice.setModToOsc(*apvts.getRawParameterValue("modtoosc") > 0.5f);
    voice.setModMix(*apvts.getRawParameterValue("modmix"));
    voice.setLfoRate(*apvts.getRawParameterValue("lforate"));
    voice.setLfoWaveform(static_cast<LfoWaveform>((int)*apvts.getRawParameterValue("lfowave")));
    voice.setMasterTune(*apvts.getRawParameterValue("mastertune"));
    voice.setGlideTime(*apvts.getRawParameterValue("glide"));
}

void Kam00gProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    updateVoiceParameters();

    float volume      = *apvts.getRawParameterValue("volume");
    float drive       = *apvts.getRawParameterValue("drive");
    bendRangeSemi     = *apvts.getRawParameterValue("bendrange");

    chorus.setRate (*apvts.getRawParameterValue("chorusrate"));
    chorus.setDepth(*apvts.getRawParameterValue("chorusdepth"));
    chorus.setMix  (*apvts.getRawParameterValue("chorusmix"));

    // Soft-clip helper: tanh saturation normalised to unity gain at input=1
    auto applyDrive = [&](float x) -> float
    {
        if (drive < 0.001f) return x;
        float gain = 1.0f + drive * 9.0f;
        return std::tanh(x * gain) / std::tanh(gain);
    };

    int pos = 0;
    for (const auto metadata : midiMessages)
    {
        auto msg      = metadata.getMessage();
        int  msgSample = metadata.samplePosition;

        // Render audio up to this MIDI event
        for (int i = pos; i < msgSample && i < buffer.getNumSamples(); ++i)
        {
            float mono = voice.isActive() ? applyDrive(voice.process()) * volume : 0.0f;
            auto [L, R] = chorus.process(mono);
            buffer.setSample(0, i, L);
            buffer.setSample(1, i, R);
        }
        pos = msgSample;

        if (msg.isNoteOn())
        {
            voice.noteOn(msg.getNoteNumber(), msg.getFloatVelocity());
            currentNote = msg.getNoteNumber();
            noteIsOn    = true;
        }
        else if (msg.isNoteOff())
        {
            if (msg.getNoteNumber() == currentNote)
            {
                voice.noteOff();
                noteIsOn = false;
            }
        }
        else if (msg.isPitchWheel())
        {
            float norm = (msg.getPitchWheelValue() - 8192) / 8192.0f;  // -1 to +1
            voice.setPitchBend(norm * bendRangeSemi);
        }
        else if (msg.isController())
        {
            if (msg.getControllerNumber() == 1)  // mod wheel
                voice.setModWheel(msg.getControllerValue() / 127.0f);
        }
        else if (msg.isAllNotesOff() || msg.isAllSoundOff())
        {
            voice.noteOff();
            noteIsOn = false;
        }
    }

    // Render remaining samples
    for (int i = pos; i < buffer.getNumSamples(); ++i)
    {
        float mono = voice.isActive() ? applyDrive(voice.process()) * volume : 0.0f;
        auto [L, R] = chorus.process(mono);
        buffer.setSample(0, i, L);
        buffer.setSample(1, i, R);
    }
}

void Kam00gProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Kam00gProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Kam00gProcessor();
}
