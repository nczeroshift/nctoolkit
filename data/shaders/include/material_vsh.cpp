
void material_vsh_prepare(mat4 mv, mat3 nm, vec3 pos, vec3 nor){
    /*gl_ModelViewMatrix*/
	v_mv_pos = (mv * vec4(pos,1.0)).xyz;
    /*gl_NormalMatrix*/
	v_nm_nor = (nm * nor);
}
