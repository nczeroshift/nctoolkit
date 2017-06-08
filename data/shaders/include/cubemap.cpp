
vec4 cubemap_reflect(samplerCube tex){
    vec3 n = normalize(v_mm_normal);
    vec3 e = normalize(v_mm_eye);
    vec3 v = reflect(e, n);
    return textureCube(tex, v);
}
    
vec4 cubemap_refract(samplerCube tex, float factor){
    vec3 n = normalize(v_mm_normal);
    vec3 e = normalize(v_mm_eye);
    vec3 v = refract(e, n, factor);
    return textureCube(tex, v);
}