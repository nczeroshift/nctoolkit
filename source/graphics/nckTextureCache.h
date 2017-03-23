
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_TEXTURE_CACHE_H
#define NCK_TEXTURE_CACHE_H

#include "nckGraphics.h"

_GRAPHICS_BEGIN

/// Texture Cache 
class TextureCache
{
private:
	/// Reference to each texture in different samplers.
	Texture *m_Textures[8];

	/// State of each texture.
	bool m_Status[8];

	/// Number of textures enabled.
	unsigned int m_MaxEnabled;

public:
	// Constructor.
	TextureCache();

	// Destructor.
	~TextureCache();

	/// Get the number of enabled textures.
	unsigned int GetEnabledTextures();

	/// Get texture state from a sampler.
	bool GetTextureState(unsigned int sampler);

	/// Clean texture cache.
	void Clear();

	/// Assign a texture to a sampler.
	void SetTexture(Texture *tex, bool status,unsigned int sampler);

	/// Get the texture from the sampler.
	Texture* GetTexture(unsigned int sampler);

	/// Check textures availability before render the model.
	void Check();
};

_GRAPHICS_END

#endif // #ifndef NCK_TEXTURE_CACHE_H
