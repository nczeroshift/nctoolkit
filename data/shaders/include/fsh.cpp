
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

uniform vec4 gphDiffuseColor, gphSpecularColor, gphAmbientColor;
uniform float gphSpecularPower;
uniform float gphAlpha;

uniform vec4 lamp_mv_pos;

varying vec3 v_nm_nor;      /* Normal view matrix vertex normal */
varying vec3 v_mv_pos;      /* Model view matrix vertex position */

varying vec3 v_mm_normal;   /* Model matrix vertex normal */
varying vec3 v_mm_eye;      /* Model matrix eye (vertex-camera) */

