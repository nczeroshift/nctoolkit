
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckGraphics_dx9.h"
#include "nckException.h"
#include "nckUtils.h"

_GRAPHICS_BEGIN

#if defined(NCK_DIRECTX9)

D3DTEXTUREFILTERTYPE Texture_DX9::ConvertFilterMode(FilterMode mode){
	if(mode == FILTER_NONE)
		return D3DTEXF_NONE;
	else if(mode == FILTER_NEAREST)
		return D3DTEXF_POINT;
	else
		return D3DTEXF_LINEAR;
}

void Texture_DX9::SetFilter(FilterType type,FilterMode mode){
	if(type == FILTER_MINIFICATION)
		m_MinFilter = ConvertFilterMode(mode);
	else if(type == FILTER_MAGNIFICATION)
		m_MagFilter = ConvertFilterMode(mode);
	else if(type == FILTER_MIPMAPPING)
		m_MipFilter = ConvertFilterMode(mode);
}

void Texture_DX9::SetAdressMode(AdressMode mode){
	if(mode==ADRESS_CLAMP)
		m_WrapMode = D3DTADDRESS_CLAMP;
	else
		m_WrapMode = D3DTADDRESS_WRAP;
}

void Texture_DX9::SetFilterAndWrapping(unsigned int sampler)
{
	m_Device->m_D3Ddevice->SetSamplerState(sampler,D3DSAMP_MAGFILTER,m_MagFilter);
	m_Device->m_D3Ddevice->SetSamplerState(sampler,D3DSAMP_MINFILTER,m_MinFilter);
	m_Device->m_D3Ddevice->SetSamplerState(sampler,D3DSAMP_MIPFILTER,m_MipFilter);
	m_Device->m_D3Ddevice->SetSamplerState(sampler,D3DSAMP_ADDRESSU,m_WrapMode );
	m_Device->m_D3Ddevice->SetSamplerState(sampler,D3DSAMP_ADDRESSV,m_WrapMode );
}

Texture2D_DX9::Texture2D_DX9(Device_DX9 *dev){
	m_Surface = NULL;
	m_Texture = NULL;
	m_Device = dev; 
	m_PFormat = FORMAT_RGBA_8B;
	m_Texture = NULL;
	m_MinFilter = D3DTEXF_LINEAR;
	m_MagFilter = D3DTEXF_LINEAR;
	m_MipFilter = D3DTEXF_NONE;
	m_WrapMode = D3DTADDRESS_WRAP;
}

Texture2D_DX9::~Texture2D_DX9(){
	SafeRelease(m_Surface);
	m_Device->m_TextureCache.Clear();
	SafeRelease(m_Texture);
}

void Texture2D_DX9::Enable(unsigned int sampler_id){
	if(sampler_id >= 8)	return;
	SetFilterAndWrapping(sampler_id);
	if(m_Device->m_TextureCache.GetTexture(sampler_id) != (Texture2D*)this)
		m_Device->m_D3Ddevice->SetTexture(sampler_id,m_Texture);
	m_Device->m_TextureCache.SetTexture((Texture2D*)this,true,sampler_id);
}

void Texture2D_DX9::Disable(unsigned int sampler_id){
	if(sampler_id >= 8)	return;
	if(!m_Device->m_TextureCache.GetTextureState(sampler_id))
			m_Device->m_D3Ddevice->SetTexture(sampler_id,NULL);
	m_Device->m_TextureCache.SetTexture((Texture2D*)this,false,sampler_id);
}

void *Texture2D_DX9::Lock(unsigned int level){
	D3DLOCKED_RECT m_lockedRect;
	m_Texture->LockRect(0, &m_lockedRect, 0, 0);
	return m_lockedRect.pBits;
}

void Texture2D_DX9::Unlock(unsigned int level){
	m_Texture->UnlockRect(level);
}

TextureType Texture2D_DX9::GetType(){
	return TEXTURE_2D;
}

Format Texture2D_DX9::GetFormat(){
	return m_PFormat;
}

unsigned int Texture2D_DX9::GetWidth(){
	D3DSURFACE_DESC m_desc;
	m_Texture->GetLevelDesc(0, &m_desc); 
	return m_desc.Width;
}

unsigned int Texture2D_DX9::GetHeight(){
	D3DSURFACE_DESC m_desc;
	m_Texture->GetLevelDesc(0, &m_desc); 
	return m_desc.Height;
}

void Texture2D_DX9::SetFilter(FilterType type,FilterMode mode){
	Texture_DX9::SetFilter(type,mode);
}

void Texture2D_DX9::SetAdressMode(AdressMode mode){
	Texture_DX9::SetAdressMode(mode);
}

TextureCubemap_DX9::TextureCubemap_DX9(Device_DX9 *dev){
	for(int i = 0 ;i <6 ;i ++) m_Surface[i] = NULL;
	m_Texture =NULL;
	m_Device = dev; 
	m_PFormat = FORMAT_RGBA_8B;
	m_Texture = NULL;
	m_MinFilter = D3DTEXF_LINEAR;
	m_MagFilter = D3DTEXF_LINEAR;
	m_MipFilter = D3DTEXF_NONE;
	m_WrapMode = D3DTADDRESS_WRAP;
}

TextureCubemap_DX9::~TextureCubemap_DX9(){
	for(int i = 0 ;i <6 ;i ++) SafeRelease(m_Surface[i]);
	m_Device->m_TextureCache.Clear();
	SafeRelease(m_Texture);
}

void TextureCubemap_DX9::Enable(unsigned int sampler_id){
	if(sampler_id >= 8)	return;
	
	SetFilterAndWrapping(sampler_id);
	if(m_Device->m_TextureCache.GetTexture(sampler_id) != (Texture2D*)this)
		m_Device->m_D3Ddevice->SetTexture(sampler_id,m_Texture);

	m_Device->m_TextureCache.SetTexture((TextureCubeMap*)this,true,sampler_id);
}

void TextureCubemap_DX9::Disable(unsigned int sampler_id){
	if(sampler_id >= 8)	return;
	if(!m_Device->m_TextureCache.GetTextureState(sampler_id))
			m_Device->m_D3Ddevice->SetTexture(sampler_id,NULL);
	m_Device->m_TextureCache.SetTexture((Texture2D*)this,false,sampler_id);
}

void *TextureCubemap_DX9::Lock(unsigned int face,unsigned int level){
	D3DCUBEMAP_FACES cfaces[6] = {D3DCUBEMAP_FACE_POSITIVE_X,
									D3DCUBEMAP_FACE_NEGATIVE_X,
									D3DCUBEMAP_FACE_POSITIVE_Y,
									D3DCUBEMAP_FACE_NEGATIVE_Y,
									D3DCUBEMAP_FACE_POSITIVE_Z,
									D3DCUBEMAP_FACE_NEGATIVE_Z};
	D3DLOCKED_RECT m_lockedRect;
	m_Texture->LockRect(cfaces[face],level, &m_lockedRect, 0, 0);
	return m_lockedRect.pBits;
}

void TextureCubemap_DX9::Unlock(unsigned int face,unsigned int level){
	D3DCUBEMAP_FACES cfaces[6] = {D3DCUBEMAP_FACE_POSITIVE_X,
								D3DCUBEMAP_FACE_NEGATIVE_X,
								D3DCUBEMAP_FACE_POSITIVE_Y,
								D3DCUBEMAP_FACE_NEGATIVE_Y,
								D3DCUBEMAP_FACE_POSITIVE_Z,
								D3DCUBEMAP_FACE_NEGATIVE_Z};
	m_Texture->UnlockRect(cfaces[face],level);
}

TextureType TextureCubemap_DX9::GetType(){
	return TEXTURE_CUBEMAP;
}

Format TextureCubemap_DX9::GetFormat(){
	return m_PFormat;
}

unsigned int TextureCubemap_DX9::GetWidth(){
	D3DSURFACE_DESC m_desc;
	m_Texture->GetLevelDesc(0, &m_desc); 
	return m_desc.Width;
}

unsigned int TextureCubemap_DX9::GetHeight(){
	D3DSURFACE_DESC m_desc;
	m_Texture->GetLevelDesc(0, &m_desc); 
	return m_desc.Height;
}

void TextureCubemap_DX9::SetFilter(FilterType type,FilterMode mode){
	Texture_DX9::SetFilter(type,mode);
}

void TextureCubemap_DX9::SetAdressMode(AdressMode mode){
	Texture_DX9::SetAdressMode(mode);
}

#endif // #if defined(NCK_DIRECTX9)

_GRAPHICS_END
