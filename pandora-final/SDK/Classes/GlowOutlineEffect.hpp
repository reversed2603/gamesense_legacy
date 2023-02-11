#pragma once
#include "../Valve/UtlVector.hpp"

class GlowObjectDefinition_t {
public:
	GlowObjectDefinition_t( ) { memset( this, 0, sizeof( *this ) ); }

	class IClientEntity* m_pEntity; //0x0000
	Vector4D m_vGlowColor;			 //0x0004
	uint8_t pad_0014[ 4 ];            //0x0014
	float m_flSomeFloat;            //0x0018
	uint8_t pad_001C[ 4 ];            //0x001C
	float m_flAnotherFloat;         //0x0020
	bool m_bRenderWhenOccluded;     //0x0024
	bool m_bRenderWhenUnoccluded;   //0x0025
	bool m_bFullBloomRender;        //0x0026
	uint8_t pad_0027[ 5 ];            //0x0027
	int32_t m_nGlowStyle;           //0x002C
	int32_t m_nSplitScreenSlot;     //0x0030
	int32_t m_nNextFreeSlot;        //0x0034

	bool IsUnused( ) const { return m_nNextFreeSlot != GlowObjectDefinition_t::ENTRY_IN_USE; }

	static const int END_OF_FREE_LIST = -1;
	static const int ENTRY_IN_USE = -2;
}; //Size: 0x0038 (56)

class CGlowObjectManager {
public:
	int RegisterGlowObject( IClientEntity* pEntity, const Vector4D& vGlowColor, bool bRenderWhenOccluded, bool bRenderWhenUnoccluded, int nSplitScreenSlot ) {
		int nIndex;
		if( m_nFirstFreeSlot == GlowObjectDefinition_t::END_OF_FREE_LIST ) {
			return -1;
		}
		else {
			nIndex = m_nFirstFreeSlot;
			m_nFirstFreeSlot = m_GlowObjectDefinitions.m_Memory.m_pMemory[ nIndex ].m_nNextFreeSlot;
		}

		m_GlowObjectDefinitions.m_Memory.m_pMemory[ nIndex ].m_pEntity = pEntity;
		m_GlowObjectDefinitions.m_Memory.m_pMemory[ nIndex ].m_vGlowColor = vGlowColor;
		m_GlowObjectDefinitions.m_Memory.m_pMemory[ nIndex ].m_bRenderWhenOccluded = bRenderWhenOccluded;
		m_GlowObjectDefinitions.m_Memory.m_pMemory[ nIndex ].m_bRenderWhenUnoccluded = bRenderWhenUnoccluded;
		m_GlowObjectDefinitions.m_Memory.m_pMemory[ nIndex ].m_nSplitScreenSlot = nSplitScreenSlot;
		m_GlowObjectDefinitions.m_Memory.m_pMemory[ nIndex ].m_nNextFreeSlot = GlowObjectDefinition_t::ENTRY_IN_USE;

		return nIndex;
	}

	void UnregisterGlowObject( int nGlowObjectHandle ) {
		m_GlowObjectDefinitions.m_Memory.m_pMemory[ nGlowObjectHandle ].m_nNextFreeSlot = m_nFirstFreeSlot;
		m_GlowObjectDefinitions.m_Memory.m_pMemory[ nGlowObjectHandle ].m_pEntity = nullptr;
		m_nFirstFreeSlot = nGlowObjectHandle;
	}

	void SetEntity( int nGlowObjectHandle, IClientEntity* pEntity ) {
		m_GlowObjectDefinitions.m_Memory.m_pMemory[ nGlowObjectHandle ].m_pEntity = pEntity;
	}

	void SetColor( int nGlowObjectHandle, const Vector4D& vGlowColor ) {
		m_GlowObjectDefinitions.m_Memory.m_pMemory[ nGlowObjectHandle ].m_vGlowColor = vGlowColor;
	}

	void SetAlpha( int nGlowObjectHandle, float flAlpha ) {
		m_GlowObjectDefinitions.m_Memory.m_pMemory[ nGlowObjectHandle ].m_vGlowColor.w = flAlpha;
	}

	void SetRenderFlags( int nGlowObjectHandle, bool bRenderWhenOccluded, bool bRenderWhenUnoccluded ) {
		m_GlowObjectDefinitions.m_Memory.m_pMemory[ nGlowObjectHandle ].m_bRenderWhenOccluded = bRenderWhenOccluded;
		m_GlowObjectDefinitions.m_Memory.m_pMemory[ nGlowObjectHandle ].m_bRenderWhenUnoccluded = bRenderWhenUnoccluded;
	}

	bool IsRenderingWhenOccluded( int nGlowObjectHandle ) const {
		return m_GlowObjectDefinitions.m_Memory.m_pMemory[ nGlowObjectHandle ].m_bRenderWhenOccluded;
	}

	bool IsRenderingWhenUnoccluded( int nGlowObjectHandle ) const {
		return m_GlowObjectDefinitions.m_Memory.m_pMemory[ nGlowObjectHandle ].m_bRenderWhenUnoccluded;
	}

	bool HasGlowEffect( IClientEntity* pEntity ) const {
		for( int i = 0; i < m_GlowObjectDefinitions.m_Size; ++i ) {
			if( !m_GlowObjectDefinitions.m_Memory.m_pMemory[ i ].IsUnused( ) && m_GlowObjectDefinitions.m_Memory.m_pMemory[ i ].m_pEntity == pEntity ) {
				return true;
			}
		}

		return false;
	}

	CUtlVector< GlowObjectDefinition_t > m_GlowObjectDefinitions; //0x0000
	int m_nFirstFreeSlot;                                         //0x000C
};

