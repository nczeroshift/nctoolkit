
#define MAX_LIGHTS 8
uniform vec4 lamp_pos_mv[MAX_LIGHTS];
uniform vec4 lamp_dir_mv[MAX_LIGHTS];
uniform vec4 lamp_color[MAX_LIGHTS];
uniform vec4 lamp_params[MAX_LIGHTS];

#ifdef FSH

void lights_compute(vec3 P, vec3 N, inout vec3 diff, inout vec3 spec){
    vec3 ret_diff = vec3(0.0);
    vec3 ret_spec = vec3(0.0);
    
    vec3 E = normalize(-P);
    
    for(int i = 0; i < MAX_LIGHTS;i++){
        float energy = lamp_pos_mv[i].w;
        if(energy < 0) break;
        
        float lampType = lamp_dir_mv[i].w;
        float distance = lamp_color[i].w;
        vec3 dirL = lamp_dir_mv[i].xyz;
        
        float diff = 0.0, spec = 0.0, intensity = 1.0;
        
        vec3 vL = lamp_pos_mv[i].xyz - P;
        float dist = length(vL);
        vec3 L = normalize(vL);
               
        if(lampType < 0.5){
            // Point light
            diff = max(dot(N,L),0.0);
            
            // Blender Quadratic Model
            float d2 = pow(distance,2.0);
            float q = 1.0;
            intensity = energy  * (d2 / (d2 + q * pow(dist,2.0)));
	
            vec3 R = normalize(reflect(-L,N));
            spec = pow(clamp(dot(R,E),0.0,1.0), gphSpecularPower);
        }
        else if(lampType < 1.5){
            // Spot light
        }
        else if(lampType < 2.5){
            // Sun/Directional light
     
            intensity = energy;
            diff = max(dot(N,dirL),0.0);
            
            vec3 R = normalize(reflect(-dirL,N));
            spec = pow(clamp(dot(R,E),0.0,1.0), gphSpecularPower);
        }
        else{
            // Area light
        }
        
        vec3 lampColor = rgb_to_srgb_approx(lamp_color[i].rgb);
        
        ret_diff += lampColor * diff * intensity;
        ret_spec += lampColor * spec * intensity;
    }
    
    diff = ret_diff * rgb_to_srgb_approx(gphDiffuseColor.rgb) * gphDiffuseColor.w;
    spec = ret_spec * rgb_to_srgb_approx(gphSpecularColor.rgb) * gphSpecularColor.w;
}

#endif