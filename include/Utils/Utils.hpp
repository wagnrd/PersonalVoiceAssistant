//
// Created by Denis Wagner on 4/11/19.
//

#ifndef PERSONAL_ASSISTANT_UTILS_HPP
#define PERSONAL_ASSISTANT_UTILS_HPP

#include <QtCore/QByteArray>
#include <math.h>

namespace Utils
{

    /**
     * Merges each n bytes of a QByteArray in big endian byte order to its corresponding T value for machines with
     * big endian memory byte order.
     *
     * @param      input  contains the unformatted byte
     * @param[out] output will be filled with the merged Ts. Has to be properly sized.
     */
    template <int bytesToBeMerged, typename T>
    void mergeBytesBE( const QByteArray& input, T* output )
    {
        int outputIndex;
        int mergeIndex;
        int inverseMergeIndex;

        for ( int inputIndex = 0; inputIndex < input.size(); ++inputIndex )
        {
            // use narrowing conversions to get corresponding int array index
            outputIndex       = inputIndex / bytesToBeMerged;
            mergeIndex        = inputIndex % bytesToBeMerged;
            inverseMergeIndex = bytesToBeMerged - 1 - mergeIndex;

            if ( mergeIndex == 0 )
                output[outputIndex] = ( input.at( inputIndex ) & 0xff ) << ( bytesToBeMerged - 1 - inverseMergeIndex )
                                                                           * 8;
            else
                output[outputIndex] += ( input.at( inputIndex ) & 0xff ) << ( bytesToBeMerged - 1 - inverseMergeIndex )
                                                                            * 8;

        }
    }

    /**
     * Merges each n bytes of a QByteArray in big endian byte order to its corresponding T value for machines with
     * little endian memory byte order.
     *
     * @param      input  contains the unformatted byte
     * @param[out] output will be filled with the merged Ts. Has to be properly sized.
     */
    template <int bytesToBeMerged, typename T>
    void mergeBytesLE( const QByteArray& input, T* output )
    {
        int outputIndex;
        int mergeIndex;

        for ( int inputIndex = 0; inputIndex < input.size(); ++inputIndex )
        {
            // use narrowing conversions to get corresponding int array index
            outputIndex = inputIndex / bytesToBeMerged;
            mergeIndex  = inputIndex % bytesToBeMerged;

            if ( mergeIndex == 0 )
                output[outputIndex] = input.at( inputIndex );
            else
                output[outputIndex] += input.at( inputIndex ) & 0xff;

            // shift 8 bits (1 byte) left, except for the last time
            if ( mergeIndex != bytesToBeMerged - 1 )
                output[outputIndex] <<= 8;
        }
    }

    /**
     * Merges each n bytes of a QByteArray in big endian byte order to its corresponding T value.
     *
     * @param      input  contains the unformatted byte
     * @param[out] output will be filled with the merged Ts. Has to be properly sized.
     */
    template <int bytesToBeMerged, typename T>
    void mergeBytes( const QByteArray& input, T* output )
    {
        static std::function<void( const QByteArray&, T* )> mergeFunction;

        // determine endianness of the machine if not already set
        // (taken from https://www.geeksforgeeks.org/little-and-big-endian-mystery/)
        if ( !mergeFunction )
        {
            unsigned short testNr = 1;
            char* firstByte = (char*) &testNr;

            if ( *firstByte )
                mergeFunction = mergeBytesLE<bytesToBeMerged, T>; // little endian
            else
                mergeFunction = mergeBytesBE<bytesToBeMerged, T>; // big endian
        }

        mergeFunction( input, output );
    }

    // TODO: add big endian version
    /**
     * Split each T value of the T* (Array) to its corresponding byte value and returns the new QByteArray
     * (Reverse function of mergeBytes())
     *
     * @param      input  contains the merged bytes in form of Ts
     * @param[out] output will be filled with the split Ts
     */
    template <int bytesToBeSplit, typename T>
    void splitToBytes( T* input, size_t inputSize, QByteArray& output )
    {
        QByteArray orderingArray( bytesToBeSplit, '\0' );

        for ( int inputIndex = 0; inputIndex < inputSize; ++inputIndex )
        {
            T currentMerge = input[inputIndex];

            for ( int splitIndex = 0; splitIndex < bytesToBeSplit; ++splitIndex )
            {
                unsigned char split = currentMerge & 0xff;
                orderingArray[bytesToBeSplit - splitIndex - 1] = split;
                currentMerge >>= 8;
            }

            output.push_back( orderingArray );
        }
    }

    /**
     * Initializes an arbitrary array with zeroes.
     *
     * @tparam T    array type
     * @param array input array which should be zero initialized
     * @param size  array size
     */
    template <typename T>
    void initWithZeros( T* array, size_t size )
    {
        for ( size_t i = 0; i < size; ++i )
            array[i] = 0;
    }

    /**
     * Convert an arbitrary numeric non-float array to 64bit double array containing values in the range [1:-1]
     *
     * @tparam     T       array type (has to be signed non-float)
     * @param      input   input array which contains anything other than double values
     * @param      size    size of the input/output array
     * @param[out] output  properly sized output array that will contain the transformed double values
     */
    template <typename T>
    void convertToDoubleArray( const T* input, size_t size, double* output )
    {
        const int  inputTypeBitSize = sizeof( *input ) * 8;
        const long minTypeValue     = pow( -2, inputTypeBitSize - 1 ) / 2;
        const long maxTypeValue     = ( -minTypeValue - 1 ) / 2;

        for ( size_t i = 0; i < size; ++i )
        {
            if ( input[i] > 0 )
                output[i] = static_cast<double>(input[i]) / maxTypeValue;
            else if ( input[i] < 0 )
                output[i] = -( static_cast<double>(input[i]) / minTypeValue );
            else
                output[i] = 0;
        }
    }

    /**
     * Convert an 64bit double array containing values in the range [1:-1] to an arbitrary numeric non-float array
     *
     * @tparam     T       array type (has to be signed non-float)
     * @param      input   input array which contains the formerly transformed double values
     * @param      size    size of the input/output array
     * @param[out] output  properly sized output array that will contain the transformed T values
     */
    template <typename T>
    void convertFromDoubleArray( const double* input, size_t size, T* output )
    {
        const int  inputTypeBitSize = sizeof( *output ) * 8;
        const long minTypeValue     = pow( -2, inputTypeBitSize - 1 );
        const long maxTypeValue     = -minTypeValue - 1;

        for ( size_t i = 0; i < size; ++i )
        {
            if ( input[i] > 0 )
                output[i] = input[i] * maxTypeValue;
            else if ( input[i] < 0 )
                output[i] = -( input[i] * minTypeValue );
            else
                output[i] = 0;
        }
    }

}

#endif //PERSONAL_ASSISTANT_UTILS_HPP
