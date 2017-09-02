
#define MAX_LIGHTS 8
uniform vec4 lamp_pos_mv[MAX_LIGHTS];
uniform vec4 lamp_dir_mv[MAX_LIGHTS];
uniform vec4 lamp_color[MAX_LIGHTS];
uniform vec4 lamp_params[MAX_LIGHTS];

#ifdef FSH

//------------------------------------------------------------------------------
// Applies the filmic curve from John Hable's presentation
// More details at : http://filmicgames.com/archives/75
vec3 ToneMapFilmicALU( vec3 color )
{
    color = max( vec3( 0.0f, 0.0f, 0.0f ), color - vec3(0.004f, 0.004f, 0.004f) );
    color = ( color * ( 6.2f * color + vec3( 0.5f, 0.5f, 0.5f ) ) ) / (color * (6.2f * color + vec3( 1.7f, 1.7f, 1.7f ) ) + vec3( 0.06f, 0.06f, 0.06f ) );
    return color;
    // Result has 1/2.2 baked in
}

// From: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
// This curve does not approximate gamma 2.2, so an explicit sRGB transform
// should be performed before display.
vec3 ToneMapACESFilm( vec3 x )
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0 );
}

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
            spec = pow(clamp(dot(R,E),0.0,1.0), gphSpecularPower );
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
    
    diff = ret_diff * gphDiffuseColor.w;
    spec = ret_spec * gphSpecularColor.w;
}

#endif