#pragma once
template <typename I>
struct UtlRBTreeLinks_t {
   I m_Left;
   I m_Right;
   I m_Parent;
   I m_Tag;
};

template <typename T, typename I>
struct UtlRBTreeNode_t : UtlRBTreeLinks_t<I> {
   T m_Data;
};

template <typename T, typename I = unsigned short, typename L = bool( * )( const T&, const T& ), typename M = CUtlMemory<UtlRBTreeNode_t<T, I>, I>>
class CUtlRBTree {
public:
   T& Element( I i ) {
	  return m_Elements.Element( i ).m_Data;
   }

   const T& Element( I i ) const {
	  return m_Elements.Element( i ).m_Data;
   }

   I MaxElement( ) const {
	  return I( m_Elements.NumAllocated( ) );
   }

   I LeftChild( I i ) const {
	  return Links( i ).m_Left;
   }

   I RightChild( I i ) const {
	  return Links( i ).m_Right;
   }

   I Parent( I i ) const {
	  return Links( i ).m_Parent;
   }

   bool IsRightChild( I i ) const {
	  return RightChild( Parent( i ) ) == i;
   }

   bool IsValidIndex( I i ) const {
	  if ( i < 0 )
		 return false;

	  if ( i > m_LastAlloc.index )
		 return false;

	  return LeftChild( i ) != i;
   }

   I Find( const T& Search ) const {
	  I Current = m_Root;
	  while ( Current != I( -1 ) ) {
		 if ( m_LessFunc( Search, Element( Current ) ) )
			Current = LeftChild( Current );
		 else if ( m_LessFunc( Element( Current ), Search ) )
			Current = RightChild( Current );
		 else
			break;
	  }

	  return Current;
   }

   I FirstInorder( ) const {
	  I i = m_Root;
	  while ( LeftChild( i ) != I( -1 ) )
		 i = LeftChild( i );

	  return i;
   }

   I NextInorder( I i ) const {
	  if ( RightChild( i ) != I( -1 ) ) {
		 i = RightChild( i );
		 while ( LeftChild( i ) != I( -1 ) )
			i = LeftChild( i );

		 return i;
	  }

	  I p = Parent( i );
	  while ( IsRightChild( i ) ) {
		 i = p;
		 if ( i == I( -1 ) )
			break;

		 p = Parent( i );
	  }

	  return p;
   }

protected:
   const UtlRBTreeLinks_t<I>& Links( I i ) const {
	  static UtlRBTreeLinks_t<I> s_Sentinel = { I( -1 ), I( -1 ), I( -1 ), I( 1 ) };
	  return i == I( -1 ) ? s_Sentinel : *reinterpret_cast< const UtlRBTreeLinks_t<I>* >( &m_Elements.Element( i ) );
   }

   L m_LessFunc;

   M m_Elements;
   I m_Root;
   I m_NumElements;
   I m_FirstFree;
   typename M::Iterator_t m_LastAlloc;

   UtlRBTreeNode_t<T, I>* m_pElements;
};

template <typename K, typename T, typename I = int>
class CUtlMap {
public:
   using LessFunc_t = bool( __cdecl* )( const K&, const K& );

   T& operator[]( I i ) {
	  return m_Tree.Element( i ).Element;
   }

   K& Key( I i ) {
	  return m_Tree.Element( i ).Key;
   }

   const K& Key( I i ) const {
	  return m_Tree.Element( i ).Key;
   }

   I MaxElement( ) const {
	  return m_Tree.MaxElement( );
   }

   bool IsValidIndex( I i ) const {
	  return m_Tree.IsValidIndex( i );
   }

   I Find( const K& Key ) const {
	  Node_t Dummy;
	  Dummy.Key = Key;
	  return m_Tree.Find( Dummy );
   }

   I FirstInorder( ) const {
	  return m_Tree.FirstInorder( );
   }

   I NextInorder( I i ) const {
	  return m_Tree.NextInorder( i );
   }

   struct Node_t {
	  K Key;
	  T Element;
   };

   class CKeyLess {
   public:
	  explicit CKeyLess( LessFunc_t lessFunc ) : m_LessFunc( lessFunc ) { }

	  bool operator!( ) const {
		 return !m_LessFunc;
	  }

	  bool operator()( const Node_t& left, const Node_t& right ) const {
		 return m_LessFunc( left.Key, right.Key );
	  }

	  LessFunc_t m_LessFunc;
   };

protected:
   CUtlRBTree<Node_t, I, CKeyLess> m_Tree;
};