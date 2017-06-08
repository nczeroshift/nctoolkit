
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

uniform vec4 gphCameraPos;      /* Camera position in the world */
uniform mat4 gphModelMatrix;    /* Object model matrix */

varying vec3 v_nm_nor;      /* Normal view matrix vertex normal */
varying vec3 v_mv_pos;      /* Model view matrix vertex position */

varying vec3 v_mm_normal;   /* Model matrix vertex normal */
varying vec3 v_mm_eye;      /* Model matrix eye (vertex-camera) */

