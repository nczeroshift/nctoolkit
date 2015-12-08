
/**
 * NCtoolKit © 2007-2015 Lu’s F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_SHADER_SOURCE_H
#define NCK_SHADER_SOURCE_H

#include "nckGraphicsConfig.h"
#include <string>
#include <map>

_GRAPHICS_BEGIN

class ShaderParser{
public:
	/**
	* Parse shader source code and map the different shaders, each shader is stored
	* in the map with the keys "vertex_shader_XXX", "pixel_shader_XXX"/"fragment_shader_XXX"
	* and geometry_shader_XXX", the XXX is the graphics device type, e.g. GL2, GLX2, GLES2,
	* DX9, DX11.
	* @param srcText Shader source code.
	* @return Map with the diferent shaders
	*/
	static std::map<std::string,std::string> Map(const std::string & srcText);
};

_GRAPHICS_END

#endif // #ifndef NCK_SHADER_SOURCE_H
