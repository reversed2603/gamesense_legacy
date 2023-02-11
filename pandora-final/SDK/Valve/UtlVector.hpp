#pragma once
#include <cassert>

#include "UtlMemory.hpp"

template< class T, class A = CUtlMemory< T > >
class CUtlVector {
	typedef A CAllocator;

	typedef T* iterator;
	typedef const T* const_iterator;
public:
	T& operator[]( int i );
	const T& operator[]( int i ) const;

	T& Element( int i )
	{
		return m_Memory[ i ];
	}

	const T& Element( int i ) const
	{
		//assert( IsValidIndex( i ) );
		return m_Memory[ i ];
	}

	T* Base( )
	{
		return m_Memory.Base( );
	}

	int Count( ) const
	{
		return m_Size;
	}

	void RemoveAll( )
	{
		for( int i = m_Size; --i >= 0; )
			Destruct( &Element( i ) );

		m_Size = 0;
	}

	bool IsValidIndex( int i ) const
	{
		return ( i >= 0 ) && ( i < m_Size );
	}

	void ShiftElementsRight( int elem, int num = 1 )
	{
		//assert( IsValidIndex( elem ) || ( m_Size == 0 ) || ( num == 0 ) );
		int numToMove = m_Size - elem - num;
		if( ( numToMove > 0 ) && ( num > 0 ) )
		{
			memmove( &Element( elem + num ), &Element( elem ), numToMove * sizeof( T ) );
		}
	}

	void ShiftElementsLeft( int elem, int num = 1 )
	{
		//assert( IsValidIndex( elem ) || ( m_Size == 0 ) || ( num == 0 ) );
		int numToMove = m_Size - elem - num;
		if( ( numToMove > 0 ) && ( num > 0 ) )
		{
			memmove( &Element( elem ), &Element( elem + num ), numToMove * sizeof( T ) );
		}
	}

	void GrowVector( int num = 1 )
	{
		if( m_Size + num > m_Memory.NumAllocated( ) )
		{
			m_Memory.Grow( m_Size + num - m_Memory.NumAllocated( ) );
		}

		m_Size += num;
	}

	int InsertBefore( int elem )
	{
		// Can insert at the end
		//assert( ( elem == Count( ) ) || IsValidIndex( elem ) );

		GrowVector( );
		ShiftElementsRight( elem );
		Construct( &Element( elem ) );
		return elem;
	}

	int AddToHead( )
	{
		return InsertBefore( 0 );
	}

	int AddToTail( ) {
		return InsertBefore( m_Size );
	}

	int InsertBefore( int elem, const T& src )
	{
		// Can't insert something that's in the list... reallocation may hose us
		assert( ( Base( ) == NULL ) || ( &src < Base( ) ) || ( &src >= ( Base( ) + Count( ) ) ) );

		// Can insert at the end
		//assert( ( elem == Count( ) ) || IsValidIndex( elem ) );

		GrowVector( );
		ShiftElementsRight( elem );
		CopyConstruct( &Element( elem ), src );
		return elem;
	}

	int AddToTail( const T& src )
	{
		// Can't insert something that's in the list... reallocation may hose us
		return InsertBefore( m_Size, src );
	}

	int Find( const T& src ) const
	{
		for( int i = 0; i < Count( ); ++i )
		{
			if( Element( i ) == src )
			{
				return i;
			}
		}
		return -1;
	}

	void Remove( int elem )
	{
		Destruct( &Element( elem ) );
		ShiftElementsLeft( elem );
		--m_Size;
	}

	bool FindAndRemove( const T& src )
	{
		int elem = Find( src );
		if( elem != -1 )
		{
			Remove( elem );
			return true;
		}
		return false;
	}

	iterator begin( )
	{
		return Base( );
	}

	const_iterator begin( ) const
	{
		return Base( );
	}

	iterator end( )
	{
		return Base( ) + Count( );
	}

	const_iterator end( ) const
	{
		return Base( ) + Count( );
	}

	//protected:
	CAllocator m_Memory;
	int m_Size;
	T* m_pElements;
};

template< typename T, class A >
T& CUtlVector< T, A >::operator[]( int i )
{
	assert( i < m_Size );
	return m_Memory[ i ];
}

template< typename T, class A >
const T& CUtlVector< T, A >::operator[]( int i ) const
{
	assert( i < m_Size );
	return m_Memory[ i ];
}