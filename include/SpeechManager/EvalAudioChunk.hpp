//
// Created by Denis Wagner on 4/20/19.
//

#ifndef PERSONAL_ASSISTANT_EVALAUDIOCHUNK_HPP
#define PERSONAL_ASSISTANT_EVALAUDIOCHUNK_HPP

#include <cstddef>
#include <algorithm>

struct EvalAudioChunk
{
    short* audioData;
    size_t dataSize;
    bool   passedFilter;
    long   deltaTime;

    EvalAudioChunk( short* audioData, size_t dataSize, long deltaTime, bool passedFilter = false )
            : audioData( audioData ), dataSize( dataSize ), deltaTime( deltaTime ), passedFilter( passedFilter )
    {
    }

    explicit EvalAudioChunk( const EvalAudioChunk& other )
            : audioData( new short[other.dataSize] ), dataSize( other.dataSize ), passedFilter( other.passedFilter ),
              deltaTime( other.deltaTime )
    {
        std::copy( other.audioData, other.audioData + other.dataSize, audioData );
    }

    EvalAudioChunk( EvalAudioChunk&& other ) noexcept
            : audioData( other.audioData ), dataSize( other.dataSize ), passedFilter( other.passedFilter ),
              deltaTime( other.deltaTime )
    {
        other.audioData    = nullptr;
        other.dataSize     = 0;
        other.passedFilter = false;
        other.deltaTime    = 0;
    }

    EvalAudioChunk& operator=( const EvalAudioChunk& other )
    {
        if ( this == &other )
            return *this;

        audioData = new short[other.dataSize];
        std::copy( other.audioData, other.audioData + other.dataSize, audioData );
        dataSize     = other.dataSize;
        passedFilter = other.passedFilter;
        deltaTime    = other.deltaTime;

        return *this;
    }

    EvalAudioChunk& operator=( EvalAudioChunk&& other ) noexcept
    {
        if ( this == &other )
            return *this;

        audioData    = other.audioData;
        dataSize     = other.dataSize;
        passedFilter = other.passedFilter;
        deltaTime    = other.deltaTime;

        other.audioData    = nullptr;
        other.dataSize     = 0;
        other.passedFilter = false;
        other.deltaTime    = 0;

        return *this;
    }

    ~EvalAudioChunk()
    {
        delete[] audioData;
    }
};

#endif //PERSONAL_ASSISTANT_EVALAUDIOCHUNK_HPP
