//
// Created by Denis Wagner on 4/15/19.
//

#ifndef PERSONAL_ASSISTANT_MEAN_HPP
#define PERSONAL_ASSISTANT_MEAN_HPP

#include <cstddef>
#include <iostream>

namespace Utils
{
    class Mean
    {
        double sumOfValues    = 0;
        size_t numberOfValues = 0;
        bool   fastDown;

    public:
        Mean( bool fastDown = false )
                : fastDown( fastDown )
        {
        }

        explicit Mean( double value, bool fastDown = false )
                : sumOfValues( value ), numberOfValues( 1 ), fastDown( fastDown )
        {
        }

        double inline operator()() const
        {
            return mean();
        }

        double inline operator()( double value )
        {
            if ( fastDown && value < mean() )
            {
                // dividing value with the factor between mean() and value
                // makes value even smaller and mean() sink faster
                value /= mean() / value;
            }

            sumOfValues += value;
            numberOfValues++;

            return mean();
        }

        void inline reduceByFactor( size_t factor )
        {
            const size_t numberOfMissingValues = numberOfValues % factor;

            if ( numberOfMissingValues != 0 )
            {
                // pad with current mean (neutral value) to evenly divide with the given factor
                sumOfValues += mean() * numberOfMissingValues;
                numberOfValues += numberOfMissingValues;
            }

            sumOfValues /= factor;
            numberOfValues /= factor;
        }

        void inline reset()
        {
            sumOfValues    = 0;
            numberOfValues = 0;
        }

        size_t inline getNumberOfValues() const
        {
            return numberOfValues;
        }

    private:
        double inline mean() const
        {
            return sumOfValues / numberOfValues;
        }
    };
}

#endif //PERSONAL_ASSISTANT_MEAN_HPP
