
void prepare_cubemap(vec4 P, vec3 N){
    v_mm_normal = normalize(vec3(gphModelMatrix * vec4(N,0.0)));
    vec3 mmpos = (gphModelMatrix * P).xyz;
    v_mm_eye = normalize(mmpos-gphCameraPos);
}