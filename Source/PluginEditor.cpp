#include "PluginEditor.h"

// ── colour palette ────────────────────────────────────────────────────────────
static const juce::Colour cBody    (0xFF111110);
static const juce::Colour cWood    (0xFF5C3A1E);
static const juce::Colour cText    (0xFFE8D898);

static const juce::Colour cCtrlFill(0xFF1A1510);
static const juce::Colour cCtrlAcc (0xFF886644);
static const juce::Colour cOscFill (0xFF2A2010);
static const juce::Colour cOscAcc  (0xFFCC8800);
static const juce::Colour cMixFill (0xFF222015);
static const juce::Colour cMixAcc  (0xFFAA9850);
static const juce::Colour cFltFill (0xFF101828);
static const juce::Colour cFltAcc  (0xFF2288CC);
static const juce::Colour cFEnvFill(0xFF281808);
static const juce::Colour cFEnvAcc (0xFFCC4400);
static const juce::Colour cAEnvFill(0xFF1E1208);
static const juce::Colour cAEnvAcc (0xFFAA3300);
static const juce::Colour cModFill (0xFF101A10);
static const juce::Colour cModAcc  (0xFF44AA44);

// ── layout constants ──────────────────────────────────────────────────────────
static const int WOOD  = 20;
static const int GAP   = 4;
static const int SECT_Y = 46;

static const int xCtrl = WOOD;             static const int wCtrl = 110;
static const int xOsc  = xCtrl+wCtrl+GAP; static const int wOsc  = 272;
static const int xMix  = xOsc+wOsc+GAP;   static const int wMix  = 126;
static const int xFlt  = xMix+wMix+GAP;   static const int wFlt  = 212;
static const int xFEnv = xFlt+wFlt+GAP;   static const int wFEnv = 142;
static const int xAEnv = xFEnv+wFEnv+GAP; static const int wAEnv = 142;
static const int xMod  = xAEnv+wAEnv+GAP; static const int wMod  = 224;
static const int TOTAL_W = xMod + wMod + WOOD;   // ~1290
static const int TOTAL_H = 440;

// ── preset table ─────────────────────────────────────────────────────────────
const Kam00gEditor::PresetData Kam00gEditor::presets[6] =
{
    // name          o1w o1r  o1t  o1lv  o2w o2r  o2t   o2lv  o3w o3r  o3t   o3lv  noise
    //               cut    res   famt  ktrk  fatk   fdcy  fsus  frel   aatk   adcy  asus  arel
    //               mamt  mflt  mosc  mmix  lrate lwave  mtune  glide  vol

    { "Init",
      3,3, 0.f,0.8f,  3,3, 0.f,  0.f,  1,3, 0.f,  0.f, 0.f,
      2000.f,0.f,0.5f,0.5f, 0.01f,0.2f,0.7f,0.3f, 0.01f,0.1f,0.8f,0.3f,
      0.f,0.f,0.f,0.f, 4.f,0, 0.f,0.f,0.7f },

    { "Fat Bass",
      3,3, 0.f,0.9f,  3,4,-0.08f,0.7f, 1,3, 0.f,  0.f, 0.f,
      600.f,0.6f,0.7f,0.3f, 0.005f,0.15f,0.1f,0.12f, 0.005f,0.12f,0.f,0.1f,
      0.3f,0.f,0.f,0.f, 2.f,0, 0.f,0.f,0.75f },

    { "Bright Lead",
      5,3, 0.f,0.9f,  3,3, 0.07f,0.6f, 1,3, 0.f,  0.f, 0.f,
      3500.f,0.55f,0.6f,0.6f, 0.008f,0.25f,0.6f,0.35f, 0.008f,0.08f,0.7f,0.25f,
      0.2f,0.f,1.f,0.f, 5.5f,0, 0.f,0.f,0.72f },

    { "Brass",
      3,3, 0.f,0.85f, 3,3, 0.03f,0.8f, 1,3, 0.f,  0.f, 0.f,
      1200.f,0.4f,0.75f,0.4f, 0.02f,0.18f,0.5f,0.2f, 0.015f,0.15f,0.6f,0.22f,
      0.15f,1.f,0.f,0.f, 3.5f,0, 0.f,0.f,0.73f },

    { "Strings",
      1,3, 0.f,0.75f, 3,3, 0.04f,0.8f, 3,3,-0.04f,0.4f, 0.f,
      4000.f,0.15f,0.4f,0.6f, 0.4f,0.3f,0.8f,0.6f, 0.35f,0.2f,0.85f,0.55f,
      0.f,0.f,0.f,0.f, 4.f,0, 0.f,0.f,0.68f },

    { "Filter Sweep",
      3,3, 0.f,0.8f,  3,3, 0.05f,0.7f, 3,3,-0.05f,0.5f, 0.25f,
      800.f,0.5f,0.9f,0.5f, 0.01f,0.3f,0.6f,0.4f, 0.01f,0.15f,0.75f,0.35f,
      0.6f,1.f,0.f,0.f, 0.4f,0, 0.f,0.f,0.70f },
};

// ── loadPreset ────────────────────────────────────────────────────────────────
void Kam00gEditor::loadPreset(int id)
{
    if (id < 1 || id > 6) return;
    const PresetData& p = presets[id - 1];
    auto& apvts = processor.apvts;

    auto set = [&](const char* paramId, float val)
    {
        if (auto* param = apvts.getParameter(paramId))
            param->setValueNotifyingHost(param->convertTo0to1(val));
    };

    set("osc1wave",    p.osc1wave);   set("osc1range",   p.osc1range);
    set("osc1tune",    p.osc1tune);   set("osc1level",   p.osc1level);
    set("osc2wave",    p.osc2wave);   set("osc2range",   p.osc2range);
    set("osc2tune",    p.osc2tune);   set("osc2level",   p.osc2level);
    set("osc3wave",    p.osc3wave);   set("osc3range",   p.osc3range);
    set("osc3tune",    p.osc3tune);   set("osc3level",   p.osc3level);
    set("noiselevel",  p.noiselevel);
    set("cutoff",      p.cutoff);     set("resonance",   p.resonance);
    set("filterenvamt",p.filterenvamt);set("keytrack",   p.keytrack);
    set("fattack",     p.fattack);    set("fdecay",      p.fdecay);
    set("fsustain",    p.fsustain);   set("frelease",    p.frelease);
    set("aattack",     p.aattack);    set("adecay",      p.adecay);
    set("asustain",    p.asustain);   set("arelease",    p.arelease);
    set("modamt",      p.modamt);     set("modtofilter", p.modtofilter);
    set("modtoosc",    p.modtoosc);   set("modmix",      p.modmix);
    set("lforate",     p.lforate);    set("lfowave",     p.lfowave);
    set("mastertune",  p.mastertune); set("glide",       p.glide);
    set("volume",      p.volume);
}

// ── constructor ───────────────────────────────────────────────────────────────
Kam00gEditor::Kam00gEditor(Kam00gProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&laf);
    auto& apvts = processor.apvts;

    masterTune.setup(apvts, "mastertune", "M. Tune",   this, &laf);
    glideKnob.setup(apvts,  "glide",      "Glide",     this, &laf);
    bendRange.setup(apvts,  "bendrange",  "Bend Range",this, &laf);

    osc1Wave.setup(apvts,  "osc1wave",  "Wave",  this, &laf);
    osc1Range.setup(apvts, "osc1range", "Range", this, &laf);
    osc1Tune.setup(apvts,  "osc1tune",  "Tune",  this, &laf);
    osc2Wave.setup(apvts,  "osc2wave",  "Wave",  this, &laf);
    osc2Range.setup(apvts, "osc2range", "Range", this, &laf);
    osc2Tune.setup(apvts,  "osc2tune",  "Tune",  this, &laf);
    osc3Wave.setup(apvts,  "osc3wave",  "Wave",  this, &laf);
    osc3Range.setup(apvts, "osc3range", "Range", this, &laf);
    osc3Tune.setup(apvts,  "osc3tune",  "Tune",  this, &laf);

    osc1Level.setup(apvts, "osc1level",  "OSC 1",  this, &laf);
    osc2Level.setup(apvts, "osc2level",  "OSC 2",  this, &laf);
    osc3Level.setup(apvts, "osc3level",  "OSC 3",  this, &laf);
    noiseLevel.setup(apvts,"noiselevel", "Noise",  this, &laf);

    cutoff.setup(apvts,       "cutoff",       "Cutoff",  this, &laf);
    resonance.setup(apvts,    "resonance",    "Res",     this, &laf);
    filterEnvAmt.setup(apvts, "filterenvamt", "Env Amt", this, &laf);
    keyTrack.setup(apvts,     "keytrack",     "Key Trk", this, &laf);

    fAttack.setup(apvts,  "fattack",  "Attack",  this, &laf);
    fDecay.setup(apvts,   "fdecay",   "Decay",   this, &laf);
    fSustain.setup(apvts, "fsustain", "Sustain", this, &laf);
    fRelease.setup(apvts, "frelease", "Release", this, &laf);

    aAttack.setup(apvts,  "aattack",  "Attack",  this, &laf);
    aDecay.setup(apvts,   "adecay",   "Decay",   this, &laf);
    aSustain.setup(apvts, "asustain", "Sustain", this, &laf);
    aRelease.setup(apvts, "arelease", "Release", this, &laf);

    modAmt.setup(apvts,    "modamt",      "Mod Amt",  this, &laf);
    modMix.setup(apvts,    "modmix",      "Mod Mix",  this, &laf);
    lfoRate.setup(apvts,   "lforate",     "LFO Rate", this, &laf);
    lfoWave.setup(apvts,   "lfowave",     "LFO Wave", this, &laf);
    modToFilter.setup(apvts,"modtofilter","→ Filter", this);
    modToOsc.setup(apvts,  "modtoosc",   "→ OSC",    this);

    drive.setup(apvts,       "drive",       "Drive",   this, &laf);
    chorusRate.setup(apvts,  "chorusrate",  "Ch. Rate", this, &laf);
    chorusDepth.setup(apvts, "chorusdepth", "Ch. Depth",this, &laf);
    chorusMix.setup(apvts,   "chorusmix",   "Ch. Mix",  this, &laf);

    volume.setup(apvts, "volume", "Volume", this, &laf);

    // Preset combo — not an APVTS param, drives loadPreset()
    presetCombo.addItem("Init",         1);
    presetCombo.addItem("Fat Bass",     2);
    presetCombo.addItem("Bright Lead",  3);
    presetCombo.addItem("Brass",        4);
    presetCombo.addItem("Strings",      5);
    presetCombo.addItem("Filter Sweep", 6);
    presetCombo.setLookAndFeel(&laf);
    addAndMakeVisible(presetCombo);
    presetCombo.onChange = [this] { loadPreset(presetCombo.getSelectedId()); };

    presetLabel.setText("PRESET", juce::dontSendNotification);
    presetLabel.setJustificationType(juce::Justification::centredRight);
    presetLabel.setLookAndFeel(&laf);
    addAndMakeVisible(presetLabel);

    setSize(TOTAL_W, TOTAL_H);
}

Kam00gEditor::~Kam00gEditor()
{
    setLookAndFeel(nullptr);
}

// ── paint ─────────────────────────────────────────────────────────────────────
void Kam00gEditor::paintSection(juce::Graphics& g, int x, int y, int w, int h,
                                 juce::Colour fill, juce::Colour accent,
                                 const juce::String& title)
{
    g.setColour(fill);
    g.fillRoundedRectangle((float)x, (float)y, (float)w, (float)h, 5.0f);

    g.setColour(accent);
    g.fillRoundedRectangle((float)x + 5, (float)y + 3, (float)w - 10, 4.0f, 2.0f);

    g.setColour(accent.withAlpha(0.35f));
    g.drawRoundedRectangle((float)x+0.5f,(float)y+0.5f,(float)w-1,(float)h-1, 5.0f, 1.0f);

    g.setColour(cText.withAlpha(0.75f));
    g.setFont(juce::Font(9.5f, juce::Font::bold));
    g.drawText(title, x + 4, y + 9, w - 8, 14, juce::Justification::centred);
}

void Kam00gEditor::paint(juce::Graphics& g)
{
    g.fillAll(cBody);

    // Wood side panels
    juce::ColourGradient woodGrad(cWood.brighter(0.2f), 0, 0, cWood.darker(0.3f), 0, TOTAL_H, false);
    g.setGradientFill(woodGrad);
    g.fillRoundedRectangle(0, 0, WOOD, TOTAL_H, 4.0f);
    g.fillRoundedRectangle(TOTAL_W - WOOD, 0, WOOD, TOTAL_H, 4.0f);
    g.setColour(cWood.darker(0.5f).withAlpha(0.5f));
    for (int i = 14; i < TOTAL_H; i += 9)
    {
        g.drawLine(2, (float)i, WOOD - 3, (float)(i + 4), 0.5f);
        g.drawLine(TOTAL_W - WOOD + 2, (float)i, TOTAL_W - 3, (float)(i + 4), 0.5f);
    }

    // Plugin name
    g.setColour(cText);
    g.setFont(juce::Font(20.0f, juce::Font::bold | juce::Font::italic));
    g.drawText("kam00g", xCtrl, 6, 160, 26, juce::Justification::centredLeft);
    g.setColour(cText.withAlpha(0.4f));
    g.setFont(juce::Font(8.5f));
    g.drawText("minimoog-inspired synthesizer", xCtrl, 28, 240, 14, juce::Justification::centredLeft);

    const int secH = TOTAL_H - SECT_Y - 8;
    paintSection(g, xCtrl, SECT_Y, wCtrl, secH, cCtrlFill, cCtrlAcc, "CONTROLLERS");
    paintSection(g, xOsc,  SECT_Y, wOsc,  secH, cOscFill,  cOscAcc,  "OSCILLATOR BANK");
    paintSection(g, xMix,  SECT_Y, wMix,  secH, cMixFill,  cMixAcc,  "MIXER");
    paintSection(g, xFlt,  SECT_Y, wFlt,  secH, cFltFill,  cFltAcc,  "FILTER");
    paintSection(g, xFEnv, SECT_Y, wFEnv, secH, cFEnvFill, cFEnvAcc, "FILTER CONTOUR");
    paintSection(g, xAEnv, SECT_Y, wAEnv, secH, cAEnvFill, cAEnvAcc, "LOUDNESS CONTOUR");
    // MOD/LFO left half, FX right half — split visually inside the section
    paintSection(g, xMod,  SECT_Y, wMod,  secH, cModFill,  cModAcc,  "MOD / LFO / FX");

    // Vertical divider between MOD and FX halves
    int divX = xMod + wMod / 2 + 4;
    g.setColour(cModAcc.withAlpha(0.25f));
    g.drawVerticalLine(divX, (float)(SECT_Y + 10), (float)(SECT_Y + secH - 4));
    g.setColour(cText.withAlpha(0.5f));
    g.setFont(juce::Font(8.5f, juce::Font::bold));
    g.drawText("FX", divX + 4, SECT_Y + 10, 30, 12, juce::Justification::centredLeft);

    // OSC row dividers
    const int oscTop  = SECT_Y + 22;
    const int oscRowH = (secH - 22) / 3;
    g.setColour(cOscAcc.withAlpha(0.2f));
    g.drawHorizontalLine(oscTop + oscRowH,     (float)(xOsc + 4), (float)(xOsc + wOsc - 4));
    g.drawHorizontalLine(oscTop + oscRowH * 2, (float)(xOsc + 4), (float)(xOsc + wOsc - 4));

    // OSC row labels
    g.setColour(cOscAcc.withAlpha(0.9f));
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("1", xOsc + 4, oscTop + 6,                  18, 14, juce::Justification::centred);
    g.drawText("2", xOsc + 4, oscTop + oscRowH + 6,        18, 14, juce::Justification::centred);
    g.drawText("3", xOsc + 4, oscTop + oscRowH * 2 + 6,    18, 14, juce::Justification::centred);
}

// ── resized ───────────────────────────────────────────────────────────────────
void Kam00gEditor::resized()
{
    const int secH    = TOTAL_H - SECT_Y - 8;
    const int oscTop  = SECT_Y + 22;
    const int oscRowH = (secH - 22) / 3;

    // Preset combo — top right of header
    presetLabel.setBounds(TOTAL_W - WOOD - 230, 10, 60, 14);
    presetCombo.setBounds(TOTAL_W - WOOD - 168, 6, 148, 26);

    // ── CONTROLLERS ──────────────────────────────────────────────────────────
    const int ctrlKS = 46;
    const int ctrlX  = xCtrl + (wCtrl - ctrlKS) / 2;
    const int ctrlGap = 28;
    masterTune.place(ctrlX, SECT_Y + 22,                           ctrlKS, ctrlKS);
    glideKnob.place (ctrlX, SECT_Y + 22 + ctrlKS + ctrlGap,       ctrlKS, ctrlKS);
    bendRange.place (ctrlX, SECT_Y + 22 + (ctrlKS + ctrlGap) * 2, ctrlKS, ctrlKS);

    // ── OSC BANK ─────────────────────────────────────────────────────────────
    const int comboX = xOsc + 24;    // leave room for OSC number label
    const int rangeX = comboX + 88 + 5;
    const int tuneX  = rangeX + 66 + 6;

    auto placeOscRow = [&](int row, ComboGroup& wave, ComboGroup& range, KnobGroup& tune)
    {
        int rowY  = oscTop + row * oscRowH;
        int midCY = rowY + (oscRowH - 24) / 2 - 8;
        wave.place (comboX, midCY,       88, 24);
        range.place(rangeX, midCY,       66, 24);
        tune.place (tuneX,  rowY + 6,   50, 50);
    };

    placeOscRow(0, osc1Wave, osc1Range, osc1Tune);
    placeOscRow(1, osc2Wave, osc2Range, osc2Tune);
    placeOscRow(2, osc3Wave, osc3Range, osc3Tune);

    // ── MIXER ────────────────────────────────────────────────────────────────
    const int mKS = 50;
    const int mx1 = xMix + 10;
    const int mx2 = xMix + wMix / 2 + 4;
    const int my1 = SECT_Y + 26;
    const int my2 = my1 + mKS + 34;

    osc1Level.place(mx1, my1, mKS, mKS);
    osc2Level.place(mx2, my1, mKS, mKS);
    osc3Level.place(mx1, my2, mKS, mKS);
    noiseLevel.place(mx2, my2, mKS, mKS);

    // ── FILTER ───────────────────────────────────────────────────────────────
    const int fKS  = 56;
    const int fY1  = SECT_Y + 24;
    const int fY2  = fY1 + fKS + 36;
    const int fGap = (wFlt - 3 * fKS) / 4;

    cutoff.place      (xFlt + fGap,               fY1, fKS, fKS);
    resonance.place   (xFlt + fGap*2 + fKS,       fY1, fKS, fKS);
    filterEnvAmt.place(xFlt + fGap*3 + fKS*2,     fY1, fKS, fKS);
    keyTrack.place    (xFlt + (wFlt - 48) / 2,    fY2, 48, 48);

    // ── FILTER CONTOUR (2×2 ADSR) ────────────────────────────────────────────
    const int eKS = 50;
    const int ex1 = xFEnv + (wFEnv / 2 - eKS) / 2;
    const int ex2 = xFEnv + wFEnv / 2 + (wFEnv / 2 - eKS) / 2;
    const int ey1 = SECT_Y + 26;
    const int ey2 = ey1 + eKS + 34;

    fAttack.place (ex1, ey1, eKS, eKS);
    fDecay.place  (ex2, ey1, eKS, eKS);
    fSustain.place(ex1, ey2, eKS, eKS);
    fRelease.place(ex2, ey2, eKS, eKS);

    // ── LOUDNESS CONTOUR (2×2 ADSR) ──────────────────────────────────────────
    const int ax1 = xAEnv + (wAEnv / 2 - eKS) / 2;
    const int ax2 = xAEnv + wAEnv / 2 + (wAEnv / 2 - eKS) / 2;

    aAttack.place (ax1, ey1, eKS, eKS);
    aDecay.place  (ax2, ey1, eKS, eKS);
    aSustain.place(ax1, ey2, eKS, eKS);
    aRelease.place(ax2, ey2, eKS, eKS);

    // ── MOD / LFO (left half of section) ─────────────────────────────────────
    const int modKS  = 46;
    const int modHW  = wMod / 2;       // half-width of MOD section
    const int modX1  = xMod + 6;
    const int modX2  = xMod + modHW / 2 + modKS / 2 - 4;  // second knob in left half
    const int modY1  = SECT_Y + 22;

    modAmt.place (modX1, modY1,               modKS, modKS);
    modMix.place (modX2, modY1,               modKS, modKS);
    lfoRate.place(modX1, modY1 + modKS + 28,  modKS, modKS);
    lfoWave.place(modX2, modY1 + modKS + 36,  modHW - modX2 + xMod - 8, 22);

    const int togY = modY1 + modKS * 2 + 68;
    modToFilter.button.setBounds(xMod + 4,        togY, modHW - 8, 22);
    modToOsc.button.setBounds   (xMod + 4, togY + 26,   modHW - 8, 22);

    volume.place(xMod + modHW / 2 - modKS / 2, togY + 58, modKS, modKS);

    // ── FX (right half of section) ────────────────────────────────────────────
    const int fxKS  = 44;
    const int fxX   = xMod + modHW + 12;   // right half start + padding
    const int fxGap = 26;

    drive.place      (fxX, modY1,                           fxKS, fxKS);
    chorusRate.place (fxX, modY1 + fxKS + fxGap,            fxKS, fxKS);
    chorusDepth.place(fxX, modY1 + (fxKS + fxGap) * 2,     fxKS, fxKS);
    chorusMix.place  (fxX, modY1 + (fxKS + fxGap) * 3,     fxKS, fxKS);
}

juce::AudioProcessorEditor* Kam00gProcessor::createEditor()
{
    return new Kam00gEditor(*this);
}
