
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckGraphics_dx9.h"
#include "nckUtils.h"
#include "nckException.h"

_GRAPHICS_BEGIN

#if defined(NCK_DIRECTX9)

RTManager_DX9::RTManager_DX9(Device_DX9 *dev)
{
	m_Device = dev;
	m_DSSurface = NULL;
}

RTManager_DX9::~RTManager_DX9()
{
	SafeRelease(m_DSSurface);	
}

bool RTManager_DX9::Enable()
{
	if(FAILED(m_Device->m_D3Ddevice->SetDepthStencilSurface(m_DSSurface)))
		return false;

	m_Device->RestartScene();

	return true;
}

bool RTManager_DX9::Attach(unsigned int target,Texture2D *tex)
{
	int max_rtt = 0;
	m_Device->GetProperty(PROPERTY_MAX_SIMULTANEOUS_RT,&max_rtt);

	if(target>(unsigned)max_rtt)
		return false;

	Texture2D_DX9* tex2d = dynamic_cast<Texture2D_DX9*>(tex);

	if(!tex2d->m_Surface)
		return false;

	if(FAILED(m_Device->m_D3Ddevice->SetRenderTarget(target,tex2d->m_Surface)))
		return false;

	return true;
}

bool RTManager_DX9::Attach(unsigned int target,unsigned int face,TextureCubeMap *tex)
{
	int max_rtt = 0;
	m_Device->GetProperty(PROPERTY_MAX_SIMULTANEOUS_RT,&max_rtt);

	if(target>(unsigned)max_rtt)
		return false;

	TextureCubemap_DX9* texCB = dynamic_cast<TextureCubemap_DX9*>(tex);

	if(!texCB->m_Surface[face])
		return false;

	if(FAILED(m_Device->m_D3Ddevice->SetRenderTarget(target,texCB->m_Surface[face])))
		return false;

	return true;
}

bool RTManager_DX9::Disable()
{
	m_Device->FinalizeScene();
	
	int max_rtt = 0;
	m_Device->GetProperty(PROPERTY_MAX_SIMULTANEOUS_RT,&max_rtt);

	for(int i = 1 ; i < max_rtt ; i++)
		m_Device->m_D3Ddevice->SetRenderTarget(i,NULL);

	if(FAILED(m_Device->m_D3Ddevice->SetRenderTarget(0,m_Device->m_RenderSurface)))
		return false;

	if(FAILED(m_Device->m_D3Ddevice->SetDepthStencilSurface(m_Device->m_DSSurface)))
		return false;

	m_Device->RestartScene();
	return true;
}

#endif // #if defined(NCK_DIRECTX9)

_GRAPHICS_END

