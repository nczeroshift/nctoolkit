#pragma vertex_shader_glx2

uniform mat4 bones_matrix[32];
varying vec3 mv_normal;

void main()
{
	vec3 P = vec3(0.0);
	vec3 N = vec3(0.0);
	
    gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;
    
    vec4 bId = gl_MultiTexCoord1;
    vec4 bWeight = gl_MultiTexCoord2;
   
    for(int i = 0; i < 4;i ++){
        float w = bWeight.x;
        int id = int(bId.x);
        if(w <= 0.0001) continue;
        
        P += (bones_matrix[id] * (vec4(gl_Vertex.xyz, 1.0)) * w).xyz;
        N += (bones_matrix[id] * (vec4(gl_Normal, 0.0)) * w).xyz;
        
        bWeight = bWeight.yzwx;
        bId = bId.yzwx;
    }

    N = normalize(N);
    
    mv_normal = normalize(gl_NormalMatrix * N);
    
	gl_Position = gl_ModelViewProjectionMatrix * vec4(P, 1.0);
}

#pragma fragment_shader_glx2
varying vec3 mv_normal;

void main()
{
	gl_FragColor = vec4(vec3(max(normalize(mv_normal).z,0.0)), 1.0);
}


