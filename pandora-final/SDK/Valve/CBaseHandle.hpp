#pragma once
#include "../sdk.hpp"
#include "../Classes/entity.hpp"
#include "../../source.hpp"

class CBaseHandle
{
public:
  CBaseHandle( );
  CBaseHandle( const CBaseHandle &other );
  CBaseHandle( unsigned long value );
  CBaseHandle( int iEntry, int iSerialNumber );

  void Init( int iEntry, int iSerialNumber );
  void Init( int iEntry, int iSerialNumber, int iBits );
  void Term( );

  // Even if this returns true, Get() still can return return a non-null value.
  // This just tells if the handle has been initted with any values.
  bool IsValid( ) const;

  int GetEntryIndex( ) const;
  int GetSerialNumber( ) const;

  int ToInt( ) const;
  bool operator !=( const CBaseHandle &other ) const;
  bool operator ==( const CBaseHandle &other ) const;
  bool operator ==( const IHandleEntity* pEnt ) const;
  bool operator !=( const IHandleEntity* pEnt ) const;
  bool operator <( const CBaseHandle &other ) const;
  bool operator <( const IHandleEntity* pEnt ) const;

  // Assign a value to the handle.
  const CBaseHandle& operator=( const IHandleEntity *pEntity );
  const CBaseHandle& Set( const IHandleEntity *pEntity );

  // Use this to dereference the handle.
  // Note: this is implemented in game code (ehandle.h)
  IHandleEntity* Get( ) const;

protected:
  // The low NUM_SERIAL_BITS hold the index. If this value is less than MAX_EDICTS, then the entity is networkable.
  // The high NUM_SERIAL_NUM_BITS bits are the serial number.
  unsigned long	m_Index;
};

inline CBaseHandle::CBaseHandle( ) {
  m_Index = INVALID_EHANDLE_INDEX;
}

inline CBaseHandle::CBaseHandle( const CBaseHandle &other ) {
  m_Index = other.m_Index;
}

inline CBaseHandle::CBaseHandle( unsigned long value ) {
  m_Index = value;
}

inline CBaseHandle::CBaseHandle( int iEntry, int iSerialNumber ) {
  Init( iEntry, iSerialNumber );
}

inline void CBaseHandle::Init( int iEntry, int iSerialNumber ) {
  m_Index = iEntry | ( iSerialNumber << NUM_ENT_ENTRY_BITS );
}

inline void CBaseHandle::Init( int iEntry, int iSerialNumber, int iBits ) {
  m_Index = iEntry | ( iSerialNumber << iBits );
}

inline void CBaseHandle::Term( ) {
  m_Index = INVALID_EHANDLE_INDEX;
}

inline bool CBaseHandle::IsValid( ) const {
  return m_Index != INVALID_EHANDLE_INDEX;
}

inline int CBaseHandle::GetEntryIndex( ) const {
  return m_Index & ENT_ENTRY_MASK;
}

inline int CBaseHandle::GetSerialNumber( ) const {
  return m_Index >> NUM_ENT_ENTRY_BITS;
}

inline int CBaseHandle::ToInt( ) const {
  return ( int )m_Index;
}

inline bool CBaseHandle::operator !=( const CBaseHandle &other ) const {
  return m_Index != other.m_Index;
}

inline bool CBaseHandle::operator ==( const CBaseHandle &other ) const {
  return m_Index == other.m_Index;
}

inline bool CBaseHandle::operator ==( const IHandleEntity* pEnt ) const {
  return Get( ) == pEnt;
}

inline bool CBaseHandle::operator !=( const IHandleEntity* pEnt ) const {
  return Get( ) != pEnt;
}

inline bool CBaseHandle::operator <( const CBaseHandle &other ) const {
  return m_Index < other.m_Index;
}

inline bool CBaseHandle::operator <( const IHandleEntity *pEntity ) const {
  unsigned long otherIndex = ( pEntity ) ? pEntity->GetRefEHandle( ).m_Index : INVALID_EHANDLE_INDEX;
  return m_Index < otherIndex;
}

inline const CBaseHandle& CBaseHandle::operator=( const IHandleEntity *pEntity ) {
  return Set( pEntity );
}

inline const CBaseHandle& CBaseHandle::Set( const IHandleEntity *pEntity ) {
  if( pEntity ) {
	 *this = pEntity->GetRefEHandle( );
  } else {
	 m_Index = INVALID_EHANDLE_INDEX;
  }

  return *this;
}

inline IHandleEntity* CBaseHandle::Get( ) const {
  return ( IHandleEntity* )m_pEntList->GetClientEntityFromHandle( *this );
}

template < class T >
class CHandle : public CBaseHandle {
public:
  CHandle( );
  CHandle( int iEntry, int iSerialNumber );
  CHandle( const CBaseHandle& handle );
  CHandle( T* pVal );

  // The index should have come from a call to ToInt(). If it hasn't, you're in trouble.
  static CHandle< T > FromIndex( int index );

  T* Get( ) const;
  void Set( const T* pVal );

  operator T*( );
  operator T*( ) const;

  bool operator!( ) const;
  bool operator==( T* val ) const;
  bool operator!=( T* val ) const;
  const CBaseHandle& operator=( const T* val );

  T* operator->( ) const;
};

template < class T >
CHandle< T >::CHandle( ) {
}

template < class T >
CHandle< T >::CHandle( int iEntry, int iSerialNumber ) {
  Init( iEntry, iSerialNumber );
}

template < class T >
CHandle< T >::CHandle( const CBaseHandle& handle ) :
  CBaseHandle( handle ) {
}

template < class T >
CHandle< T >::CHandle( T* pObj ) {
  Term( );
  Set( pObj );
}

template < class T >
inline CHandle< T > CHandle< T >::FromIndex( int index ) {
  CHandle< T > ret;
  ret.m_Index = index;
  return ret;
}

template < class T >
inline T* CHandle< T >::Get( ) const {
  return ( T* )CBaseHandle::Get( );
}

template < class T >
inline CHandle< T >::operator T*( ) {
  return Get( );
}

template < class T >
inline CHandle< T >::operator T*( ) const {
  return Get( );
}

template < class T >
inline bool CHandle< T >::operator!( ) const {
  return !Get( );
}

template < class T >
inline bool CHandle< T >::operator==( T* val ) const {
  return Get( ) == val;
}

template < class T >
inline bool CHandle< T >::operator!=( T* val ) const {
  return Get( ) != val;
}

template < class T >
void CHandle< T >::Set( const T* pVal ) {
  CBaseHandle::Set( reinterpret_cast< const IHandleEntity* >( pVal ) );
}

template < class T >
inline const CBaseHandle& CHandle< T >::operator=( const T* val ) {
  Set( val );
  return *this;
}

template < class T >
T* CHandle< T >::operator->( ) const {
  return Get( );
}
