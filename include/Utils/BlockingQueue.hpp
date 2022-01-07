//
// Created by Denis Wagner on 4/11/19.
//

#ifndef PERSONAL_ASSISTANT_BLOCKINGQUEUE_HPP
#define PERSONAL_ASSISTANT_BLOCKINGQUEUE_HPP

#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <deque>

namespace Utils
{
// thread-safe std::queue wrapper
    template <typename T>
    class BlockingQueue
    {
        std::deque<T>          queue;
        mutable QMutex         mutex;
        mutable QWaitCondition bufferNotEmpty;

    public:
        BlockingQueue()
                : queue( std::deque<T>() )
        {
        }

        void pop_back()
        {
            QMutexLocker lock( &mutex );

            while ( queue.size() == 0 )
                bufferNotEmpty.wait( &mutex );

            queue.pop_back();
        }

        void pop_front()
        {
            QMutexLocker lock( &mutex );

            while ( queue.size() == 0 )
                bufferNotEmpty.wait( &mutex );

            queue.pop_front();
        }

        void push_back( T&& item )
        {
            QMutexLocker lock( &mutex );

            queue.push_back( std::move( item ) );
            bufferNotEmpty.wakeAll();
        }

        void push_back( const T& item )
        {
            QMutexLocker lock( &mutex );

            queue.push_back( item );
            bufferNotEmpty.wakeAll();
        }

        void push_front( T&& item )
        {
            QMutexLocker lock( &mutex );

            queue.push_front( std::move( item ) );
            bufferNotEmpty.wakeAll();
        }

        void push_front( const T& item )
        {
            QMutexLocker lock( &mutex );

            queue.push_front( item );
            bufferNotEmpty.wakeAll();
        }

        T& front()
        {
            QMutexLocker lock( &mutex );

            while ( queue.size() == 0 )
                bufferNotEmpty.wait( &mutex );

            bufferNotEmpty.wakeAll();

            return queue.front();
        }

        const T& front() const
        {
            QMutexLocker lock( &mutex );

            while ( queue.size() == 0 )
                bufferNotEmpty.wait( &mutex );

            bufferNotEmpty.wakeAll();

            return queue.front();
        }

        T& back()
        {
            QMutexLocker lock( &mutex );

            while ( queue.size() == 0 )
                bufferNotEmpty.wait( &mutex );

            bufferNotEmpty.wakeAll();

            return queue.back();
        }

        const T& back() const
        {
            QMutexLocker lock( &mutex );

            while ( queue.size() == 0 )
                bufferNotEmpty.wait( &mutex );

            bufferNotEmpty.wakeAll();

            return queue.back();
        }

        size_t size() const
        {
            QMutexLocker lock( &mutex );

            return queue.size();
        }

        T& operator[]( size_t index )
        {
            QMutexLocker lock( &mutex );

            while ( queue.size() <= index )
                bufferNotEmpty.wait( &mutex );

            bufferNotEmpty.wakeAll();

            return queue[index];
        }

        const T& operator[]( size_t index ) const
        {
            QMutexLocker lock( &mutex );

            while ( queue.size() <= index )
                bufferNotEmpty.wait( &mutex );

            bufferNotEmpty.wakeAll();

            return queue[index];
        }
    };
}
#endif //PERSONAL_ASSISTANT_BLOCKINGQUEUE_HPP
