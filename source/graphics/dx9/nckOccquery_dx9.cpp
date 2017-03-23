
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckGraphics_dx9.h"
#include "nckException.h"

_GRAPHICS_BEGIN

#if defined(NCK_DIRECTX9)

OcclusionQuery_DX9::OcclusionQuery_DX9(Device_DX9 *dev){
	m_Device= dev;
}

OcclusionQuery_DX9::~OcclusionQuery_DX9(){
	m_OccQuery->Release();
}

void OcclusionQuery_DX9::Begin(){
	m_OccQuery->Issue(D3DISSUE_BEGIN);
}

void OcclusionQuery_DX9::End(){
	m_OccQuery->Issue(D3DISSUE_END);
}

bool OcclusionQuery_DX9::GetDrawnPixels(unsigned int *pixels)
{
	DWORD pxcount;

	if(m_OccQuery->GetData( &pxcount, sizeof(DWORD), D3DGETDATA_FLUSH )  == S_FALSE){
		*pixels = 0;
		return false;
	}
	else{
		*pixels = pxcount;
		return true;
	}
}

#endif // #if defined(NCK_DIRECTX9)

_GRAPHICS_END

