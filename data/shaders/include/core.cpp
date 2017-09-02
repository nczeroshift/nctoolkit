
varying vec3 v_pos_mv;
varying vec3 v_nor_mv;
varying vec3 v_nor_w;

#ifdef VSH

uniform mat4 gphModelMatrix;
uniform mat4 gphViewMatrix;
uniform mat4 gphProjectionMatrix;

void core_transform_to_screen(vec4 P){
    gl_Position = gl_ModelViewProjectionMatrix * P;
}

void core_copy_texCoord(){
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[1] = gl_MultiTexCoord1;
    gl_TexCoord[2] = gl_MultiTexCoord2;
    gl_TexCoord[3] = gl_MultiTexCoord3;
    gl_TexCoord[4] = gl_MultiTexCoord4;
    gl_TexCoord[5] = gl_MultiTexCoord5;
    gl_TexCoord[6] = gl_MultiTexCoord6;
    gl_TexCoord[7] = gl_MultiTexCoord7;
}

#endif

#ifdef FSH

uniform vec4 gphDiffuseColor, gphSpecularColor, gphAmbientColor;
uniform float gphSpecularPower;
uniform float gphAlpha;

const float SRGB_GAMMA = 1.0 / 2.2;
const float SRGB_INVERSE_GAMMA = 2.2;

vec3 rgb_to_srgb_approx(vec3 rgb) {
    return pow(rgb, vec3(SRGB_GAMMA));
}

vec3 srgb_to_rgb_approx(vec3 srgb) {
    return pow(srgb, vec3(SRGB_INVERSE_GAMMA));
}

#endif