
#pragma vertex_shader_glx2

varying vec3 lightVec;
varying vec3 halfVec;
varying vec3 eyeVec;
	
void main()
{
	vec4 position 	= gl_Vertex;
	vec3 normal 	= gl_Normal;
	vec2 uv 		= gl_MultiTexCoord0.xy;
	vec3 tangent 	= gl_MultiTexCoord1.xyz;
	
	vec3 n = normalize (gl_NormalMatrix * normal);
	vec3 t = normalize (gl_NormalMatrix * tangent);
	vec3 b = cross (n, t);
	
	vec3 lightPos = vec3(gl_ModelViewMatrix * vec4(-5,-10,14,0)); // no mesmo espaço?
	
	vec3 vertexPosition = vec3(gl_ModelViewMatrix *  position);
	vec3 lightDir = normalize(lightPos - vertexPosition);
	
	vec3 v;
	v.x = dot (lightDir, t);
	v.y = dot (lightDir, b);
	v.z = dot (lightDir, n);
	lightVec = normalize (v);
	
	v.x = dot (vertexPosition, t);
	v.y = dot (vertexPosition, b);
	v.z = dot (vertexPosition, n);
	eyeVec = normalize (v);
	
	vertexPosition = normalize(vertexPosition);
	
	vec3 halfVector = normalize(vertexPosition + lightDir);
	v.x = dot (halfVector, t);
	v.y = dot (halfVector, b);
	v.z = dot (halfVector, n);

	halfVec = v ; 
		
	gl_TexCoord[0].xy = uv;
	gl_Position = gl_ModelViewProjectionMatrix * position;
	gl_FrontColor = gl_Color;
}

#pragma fragment_shader_glx2

uniform sampler2D gphTexture0;
uniform sampler2D gphTexture1;

varying vec3 lightVec;
varying vec3 halfVec;
varying vec3 eyeVec;

void main()
{
	vec3 normal = 2.0 * texture2D (gphTexture1, gl_TexCoord[0].st).rgb - 1.0;
	normal = normalize (normal);
	
	// compute diffuse lighting
	float lamberFactor= max (dot (lightVec, normal), 0.0) ;
	vec4 diffuseMaterial = 0.0;
	vec4 diffuseLight  = 0.0;
	
	// compute specular lighting
	vec4 specularMaterial ;
	vec4 specularLight ;
	float shininess ;
  
	// compute ambient
	vec4 ambientLight = vec4(0.1,0.1,0.1,1);	
	
	if (lamberFactor > 0.0)
	{
		diffuseMaterial = texture2D (gphTexture0, gl_TexCoord[0].st);
		diffuseLight  = vec4(1.0,1.0,1.0,1.0);
		
		// In doom3, specular value comes from a texture 
		specularMaterial =  vec4(1.0,1.0,1.0,1.0)*0.5;
		specularLight = gl_LightSource[0].specular;
		shininess = pow (max (dot (halfVec, normal), 0.0), 4.0)  ;
		 
		gl_FragColor =	diffuseMaterial * diffuseLight * lamberFactor ;
		gl_FragColor +=	specularMaterial * specularLight * shininess ;				
	}
	
	gl_FragColor +=	ambientLight;
}


