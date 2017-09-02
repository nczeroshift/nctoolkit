
//varying vec3 v_light_bm[MAX_LIGHTS];
//varying vec3 v_eye_bm;

varying mat3 v_tbn_bm;

#ifdef VSH

void bumpmap_compute(vec3 P, vec3 N, vec4 T){
    vec3 B = cross( N, T.xyz ) * T.w;
    v_tbn_bm = mat3( T.xyz, B, N );
    
    //vec3 B = cross(N, T);
    /*for (int i=0;i < MAX_LIGHTS; i++)
    {
        if(lamp_pos_mv[i].w < 0.0)
            break;
        vec3 L = lamp_pos_mv[i].xyz - P;
        v_light_bm[i] = vec3(dot(L, T),dot(L, B),dot(L, N));
    }
    
    vec3 E = -P;
    v_eye_bm = vec3(dot(E, T),dot(E, B),dot(E, N));*/
}

#endif

#ifdef FSH

vec3 bumpmap_mix_sample(vec3 N, vec3 texN, float factor){
    vec3 tN = normalize( texN * 2.0 - 1.0 ).xyz;
    vec3 newN = v_tbn_bm * tN;
    return mix(N,newN,factor);
}


#endif