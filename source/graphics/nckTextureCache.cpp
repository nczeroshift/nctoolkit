
/**
 * NCtoolKit © 2007-2015 Lu’s F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckTextureCache.h"
#include <stdio.h>

_GRAPHICS_BEGIN

TextureCache::TextureCache()
{
	for(unsigned int i = 0;i<8;i++){
		m_Textures[i] = NULL;
		m_Status[i] = false;
	}

	m_MaxEnabled = 0;
}

TextureCache::~TextureCache(){}

void TextureCache::Clear()
{
	for(unsigned int i = 0;i<m_MaxEnabled;i++){
		if(!m_Textures[i])
			continue;
		m_Textures[i]->Disable(i);
		m_Textures[i] = NULL;
		m_Status[i] = false;
	}
	m_MaxEnabled = 0;
}

void TextureCache::SetTexture(Texture *tex, bool status,unsigned int sampler)
{
	if(m_MaxEnabled<sampler+1)
		m_MaxEnabled = sampler+1;

	m_Textures[sampler] = tex;
	m_Status[sampler] = status;
}

Texture* TextureCache::GetTexture(unsigned int sampler)
{
	return m_Textures[sampler];
}

void TextureCache::Check()
{
	unsigned int last=0;

	for(unsigned int i = 0;i<m_MaxEnabled;i++){
		if(!m_Status[i] && m_Textures[i]){
			m_Textures[i]->Disable(i);
			m_Textures[i]=NULL;
			last = i;
		}
	}

	m_MaxEnabled = last;
}

bool TextureCache::GetTextureState(unsigned int sampler)
{
	return m_Status[sampler];
}

unsigned int TextureCache::GetEnabledTextures()
{
	return m_MaxEnabled;
}

_GRAPHICS_END
