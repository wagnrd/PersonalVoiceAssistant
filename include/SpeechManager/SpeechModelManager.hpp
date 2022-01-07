//
// Created by Denis Wagner on 4/25/19.
//

#ifndef PERSONAL_ASSISTANT_SPEECHMODELMANAGER_HPP
#define PERSONAL_ASSISTANT_SPEECHMODELMANAGER_HPP

#include <string>
#include <include/Utils/BlockingQueue.hpp>
#include <QtCore/QThread>
#include <include/Utils/Config.hpp>

struct ModelState;

class SpeechModelManager : public QThread
{
    const int SAMPLE_RATE = Config::getSampleRate();

    const char* modelPath    = "../model/output_graph.pbmm";
    const char* alphabetPath = "../model/alphabet.txt";
    const char* triePath     = "../model/trie";
    const char* lmBinPath    = "../model/lm.binary";

    // constants taken from DeepSpeech/client.py (Github Repo)
    const unsigned int mfccFeatures      = 26;
    const unsigned int contextWindowSize = 9;
    const unsigned int beamWidth         = 500;
    const float        lmAlpha           = 0.75;
    const float        lmBeta            = 1.85;

    ModelState* model;

    Utils::BlockingQueue<std::string>& inferredTextBuffer;
    Utils::BlockingQueue<std::vector<short>> speechDataBuffer;

public:
    explicit SpeechModelManager( Utils::BlockingQueue<std::string>& inferredTextBuffer );
    void feedSpeechData( std::vector<short> speechData );

private:
    virtual void run() override;
};

#endif //PERSONAL_ASSISTANT_SPEECHMODELMANAGER_HPP
