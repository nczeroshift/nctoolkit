// depends "lights.cpp"

varying vec3 v_light_bm[MAX_LIGHTS];
varying vec3 v_eye;

#ifdef VSH

void bumpmap_compute(vec3 P, vec3 N, vec3 T){
    vec3 B = cross(N, T);
    for (int i=0;i < MAX_LIGHTS; i++)
    {
        if(lamp_pos_mv[i].w < 0.0)
            break;
        vec3 L = lamp_pos_mv[i].xyz - V;
        lightVec[i] = vec3(dot(L, T),dot(L, B),dot(L, N));
    }
    
    vec3 E = -V;
    v_eye = vec3(dot(L, T),dot(L, B),dot(L, N));
}

#endif