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


/// @author Matthew Herrmann 2007
/// @author Vsevolod Vlaskine 2010-2011

#ifndef COMMA_PACKED_BITS_HEADER_H_
#define COMMA_PACKED_BITS_HEADER_H_

#include <string.h>
#include <boost/static_assert.hpp>
#include <comma/packed/field.h>

namespace comma { namespace packed {

/// packed bit-field structure
/// @note had to use memcpy(), since reinterpret_cast gives a compilation warning
template < typename B, unsigned char Default = 0 >
struct bits : public packed::field< bits< B, Default >, B, sizeof( unsigned char ) >
{
    enum { size = sizeof( unsigned char ) };

    BOOST_STATIC_ASSERT( size == 1 );
    
    BOOST_STATIC_ASSERT( sizeof( B ) == 1 );

    typedef B type;

    typedef packed::field< bits< B, Default >, B, sizeof( unsigned char ) > base_type;

    static type default_value() { static const unsigned char d = Default; type t; ::memcpy( &t, &d, 1 ); return t; }

    static void pack( char* storage, type value ) { ::memcpy( storage, &value, 1 ); }

    static type unpack( const char* storage ) { type t; ::memcpy( &t, storage, 1 ); return t; }

    const bits& operator=( const bits& rhs ) { return base_type::operator=( rhs ); }

    const bits& operator=( type rhs ) { return base_type::operator=( rhs ); }
    
    const bits& operator=( unsigned char rhs ) { type t; ::memcpy( &t, &rhs, 1 ); return base_type::operator=( t ); }
    
    type& fields() { return *( reinterpret_cast< type* >( this ) ); }
    
    const type& fields() const { return *( reinterpret_cast< const type* >( this ) ); }
};

} } // namespace comma { namespace packed {

#endif // COMMA_PACKED_BITS_HEADER_H_
