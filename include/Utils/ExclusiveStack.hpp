//
// Created by Denis Wagner on 5/9/19.
//

#ifndef PERSONAL_ASSISTANT_EXCLUSIVESTACK_HPP
#define PERSONAL_ASSISTANT_EXCLUSIVESTACK_HPP

#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QWaitCondition>
#include <vector>
#include <stack>

template <typename T>
class ExclusiveStack
{
    std::deque<T>          stack;
    mutable QMutex         mutex;

public:
    bool empty() const
    {
        QMutexLocker lock( &mutex );

        return stack.empty();
    }

    size_t size() const
    {
        QMutexLocker lock( &mutex );

        return stack.size();
    }

    T& top()
    {
        QMutexLocker lock( &mutex );

        return stack.back();
    }

    const T& top() const
    {
        QMutexLocker lock( &mutex );

        return stack.back();
    }

    void push( const T& item )
    {
        QMutexLocker lock( &mutex );

        deleteDuplicates( item );
        stack.push_back( item );
    }

    void push( T&& item )
    {
        QMutexLocker lock( &mutex );

        deleteDuplicates( item );
        stack.push_back( item );
    }

    void pop()
    {
        QMutexLocker lock( &mutex );

        stack.pop_back();
    }

    void erase( const T& item )
    {
        QMutexLocker lock( &mutex );

        deleteDuplicates( item );
    }

    void erase( T&& item )
    {
        QMutexLocker lock( &mutex );

        deleteDuplicates( item );
    }

private:
    void deleteDuplicates( const T& item )
    {
        // deleting duplicate entries to maintain the exclusiveness of an entry
        for ( auto iterator = stack.begin(); iterator != stack.end(); ++iterator )
        {
            if ( item == *iterator )
            {
                stack.erase( iterator );

                // returns instantly because the exclusive stack can't have more than one same item by definition
                return;
            }
        }
    }
};

#endif //PERSONAL_ASSISTANT_EXCLUSIVESTACK_HPP
