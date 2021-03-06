// This file is part of comma, a generic and flexible library
// Copyright (c) 2011 The University of Sydney
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the The University of Sydney.
// 4. Neither the name of the The University of Sydney nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


/// @author vsevolod vlaskine

#ifndef COMMA_CONTAINERS_CYCLIC_BUFFER_H_
#define COMMA_CONTAINERS_CYCLIC_BUFFER_H_

#include <vector>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <comma/base/exception.h>
#include <comma/math/cyclic.h>

namespace comma {

/// an arbitrary cyclic buffer on top of the given container;
/// allocates size copies of given instance (or default-constructed T);
/// see unit test for more usage;
/// @todo: maybe, allocate buffers not during the construction,
///        but on push() and deallocate on pop()
template < typename T >
class cyclic_buffer
{
    public:
        /// constructor
        cyclic_buffer( std::size_t size, const T& t = T() );
        
        /// copy constructor
        cyclic_buffer( const cyclic_buffer& rhs ) { operator=( rhs ); }
        
        /// assignment
        const cyclic_buffer& operator=( const cyclic_buffer& rhs );
        
        /// return front
        T& front();
        
        /// return front
        const T& front() const;
        
        /// return back
        T& back();
        
        /// return back
        
        const T& back() const;
        
        /// push a new element at the end of the list
        void push( const T& t );
        
        /// push a new element at the end of the list
        template < typename Iterator >
        void push( Iterator begin, Iterator end );
        
        /// pop the new element at the front of the list
        void pop( std::size_t n = 1 );
        
        /// return current size
        std::size_t size() const;
        
        /// return capacity
        std::size_t capacity() const;
        
        /// return true, if empty
        bool empty() const;
        
        /// clear
        void clear();
        
    protected:
        std::vector< T > vector_;
        math::cyclic< std::size_t > begin_;
        math::cyclic< std::size_t > end_;
        bool empty_;
};

/// an arbitrary cyclic buffer of fixed size on top of the given container,
/// i.e. you cannot Push() and Pop() on it, but it is preallocated
template < typename T, std::size_t S >
class fixed_cyclic_buffer : public cyclic_buffer< T >
{
    public:
        /// constructor
        fixed_cyclic_buffer( const T& t = T() );
        
        /// copy constructor
        fixed_cyclic_buffer( const fixed_cyclic_buffer& rhs ) { operator=( rhs ); }
        
        /// shift indices up by given number
        void operator>>( std::size_t i );
        
        /// shift indices down by given number
        void operator<<( std::size_t i );
        
        /// return reference to element relative to the current begin
        T& operator[]( std::size_t i );
        
        /// return reference to element relative to the current begin
        const T& operator[]( std::size_t i ) const;
        
    private:
        void push();
        void pop();
        void clear();
};

template < typename T >
inline cyclic_buffer< T >::cyclic_buffer( std::size_t size, const T& t )
    : vector_( size, t )
    , begin_( 0, size )
    , end_( 0, size )
    , empty_( true )
{
    assert( size > 0 );
}

template < typename T >
inline T& cyclic_buffer< T >::front()
{ 
    if( empty() ) { COMMA_THROW( comma::exception, "empty" ); }
    return vector_[ begin_() ];
}

template < typename T >
inline const T& cyclic_buffer< T >::front() const
{ 
    if( empty() ) { COMMA_THROW( comma::exception, "empty" ); }
    return vector_[ begin_() ];
}

template < typename T >
inline T& cyclic_buffer< T >::back()
{ 
    if( empty() ) { COMMA_THROW( comma::exception, "empty" ); }
    return vector_[ ( end_ - 1 )() ];
}

template < typename T >
inline const T& cyclic_buffer< T >::back() const
{ 
    if( empty() ) { COMMA_THROW( comma::exception, "empty" ); }
    return vector_[ ( end_ - 1 )() ];
}

template < typename T >
inline std::size_t cyclic_buffer< T >::capacity() const { return vector_.size(); }

template < typename T >
inline void cyclic_buffer< T >::push( const T& t )
{
    if( size() == vector_.size() ) { COMMA_THROW( comma::exception, "full" ); }
    vector_[ end_() ] = t;
    ++end_;
    empty_ = false;
}

template < typename T >
template < typename Iterator >
inline void cyclic_buffer< T >::push( Iterator begin, Iterator end )
{
    for( Iterator it = begin; it != end; ++it ) { push( *it ); }
}

template < typename T >
inline void cyclic_buffer< T >::pop( std::size_t n )
{
    if( empty() ) { return; }
    if( n >= size() ) { clear(); return; }
    begin_ += n;
    empty_ = begin_ == end_;
}

template < typename T >
inline std::size_t cyclic_buffer< T >::size() const
{
    std::size_t s = ( end_ - begin_ )();
    return s == 0 && !empty_ ? vector_.size() : s;
}

template < typename T >
inline bool cyclic_buffer< T >::empty() const { return empty_; }

template < typename T >
inline void cyclic_buffer< T >::clear() { empty_ = true; end_ = begin_; }

template < typename T, std::size_t S >
inline fixed_cyclic_buffer< T, S >::fixed_cyclic_buffer( const T& t )
    : cyclic_buffer< T >( S, t )
{
    this->empty_ = false;
}

template < typename T, std::size_t S >
inline void fixed_cyclic_buffer< T, S >::operator>>( std::size_t i )
{
    this->begin_ += i;
    this->end_ = this->begin_;
}

template < typename T, std::size_t S >
inline void fixed_cyclic_buffer< T, S >::operator<<( std::size_t i )
{
    this->begin_ -= i;
    this->end_ = this->begin_;
}

template < typename T, std::size_t S >
inline T& fixed_cyclic_buffer< T, S >::operator[]( std::size_t i )
{
    return this->vector_[ ( this->begin_ + i )() ];
}

template < typename T, std::size_t S >
inline const T& fixed_cyclic_buffer< T, S >::operator[]( std::size_t i ) const
{
    return this->vector_[ ( this->begin_ + i )() ];
}

} // namespace comma {

#endif // COMMA_CONTAINERS_CYCLIC_BUFFER_H_
