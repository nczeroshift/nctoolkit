
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckGraphics_dx9.h"
#include "nckUtils.h"
#include "nckException.h"

_GRAPHICS_BEGIN

#if defined(NCK_DIRECTX9)

Program_DX9::Program_DX9(Device_DX9 *dev)
{
	m_Device = dev;
}

Program_DX9::~Program_DX9()
{
	ListFor(Shader_DX9_Shader*,m_Shaders,i)
		delete (*i);
}

void Program_DX9::Enable_Internal()
{
	ListFor(Shader_DX9_Shader*,m_Shaders,i)
		(*i)->Enable();
}

void Program_DX9::Enable(){
	m_Device->m_ActiveProgram = this;
	
}

void Program_DX9::Disable(){
	ListFor(Shader_DX9_Shader*,m_Shaders,i)
		(*i)->Disable();
	m_Device->m_ActiveProgram = NULL;
	//m_Device->UpdateClipPlanes();
}

bool Program_DX9::SetVariable1i(const std::string & name,int value){
	int found = 0;
	ListFor(Shader_DX9_Shader*,m_Shaders,i)
		if((*i)->SetVariable(name,Shader_DX9_Variable::INT1,1,sizeof(int),(void*)&value)) found = 1;
	if(found == 0)return false;
	return true;
}

bool Program_DX9::SetVariable1f(const std::string & name,float value){
	int found = 0;
	ListFor(Shader_DX9_Shader*,m_Shaders,i)
		if((*i)->SetVariable(name,Shader_DX9_Variable::FLOAT1,1,sizeof(int),(void*)&value)) found = 1;
	if(found == 0)return false;
	return true;
}

bool Program_DX9::SetVariable4i(const std::string & name,int x,int y,int z,int w){
	int found = 0;
	int var[4] = {x,y,z,w};
	ListFor(Shader_DX9_Shader*,m_Shaders,i)
		if((*i)->SetVariable(name,Shader_DX9_Variable::INT4,1,sizeof(float)*4,(void*)var)) found = 1;
	if(found == 0)return false;
	return true;
}

bool Program_DX9::SetVariable4f(const std::string & name,float x,float y,float z,float w){
	int found = 0;
	float var[4] = {x,y,z,w};
	ListFor(Shader_DX9_Shader*,m_Shaders,i)
		if((*i)->SetVariable(name,Shader_DX9_Variable::FLOAT4,1,sizeof(float)*4,(void*)var)) found = 1;
	if(found == 0)return false;
	return true;
}

bool Program_DX9::SetArray1f(const std::string & name,int count,const float *value){
	int found = 0;
	ListFor(Shader_DX9_Shader*,m_Shaders,i)
		if((*i)->SetVariable(name,Shader_DX9_Variable::FLOAT1_ARRAY,count,sizeof(float),(void*)value)) found = 1;
	if(found == 0)return false;
	return true;
}

bool Program_DX9::SetArray4f(const std::string & name,int count,const float *value){
	int found = 0;
	ListFor(Shader_DX9_Shader*,m_Shaders,i)
		if((*i)->SetVariable(name,Shader_DX9_Variable::FLOAT4_ARRAY,count,sizeof(float)*4,(void*)value)) found = 1;
	if(found == 0)return false;
	return true;
}

bool Program_DX9::SetMatrix(const std::string & name,const float *mat){
	int found = 0;
	ListFor(Shader_DX9_Shader*,m_Shaders,i)
		if((*i)->SetVariable(name,Shader_DX9_Variable::MATRIX,1,sizeof(float)*16,(void*)mat)) found = 1;
	if(found == 0)return false;
	return true;
}

bool Program_DX9::SetMatrixArray(const std::string & name,int count,const float *v){
	int found = 0;
	ListFor(Shader_DX9_Shader*,m_Shaders,i)
		if((*i)->SetVariable(name,Shader_DX9_Variable::MATRIX_ARRAY,count,sizeof(float)*16,(void*)v)) found = 1;
	if(found == 0)return false;
	return true;
}




void Shader_DX9_Variable::Bind(LPD3DXCONSTANTTABLE constants, LPDIRECT3DDEVICE9 dev)
{
	switch(m_Type)
	{
	case FLOAT1:{
			float *fdata = ( float* ) m_Data; if ( m_Global ) fdata = ( float* ) m_Global->m_Data;
			constants->SetFloat ( dev, m_Handle, *fdata ); return; }
	case FLOAT4:{
			float *fdata = ( float* ) m_Data; if ( m_Global ) fdata = ( float* ) m_Global->m_Data;
			constants->SetFloatArray ( dev, m_Handle, fdata, 4 ); return; }
	case FLOAT4_ARRAY:{
			float *fdata = ( float* ) m_Data; if ( m_Global ) fdata = ( float* ) m_Global->m_Data;
			constants->SetFloatArray ( dev, m_Handle, fdata, 4*m_Count ); return; }
	case INT1:{
			int *idata = ( int* ) m_Data; if(m_Global) idata = ( int* ) m_Global->m_Data;
			constants->SetInt(dev,m_Handle,*idata); return; }
	case INT4:{
			int *idata = ( int* ) m_Data; if(m_Global) idata = ( int* ) m_Global->m_Data;
			constants->SetIntArray(dev,m_Handle,idata,4); return; }
	case INT4_ARRAY:{
			float *fdata = ( float* ) m_Data; if ( m_Global ) fdata = ( float* ) m_Global->m_Data;
			constants->SetFloatArray ( dev, m_Handle, fdata, 4*m_Count ); return; }
	case MATRIX:{
			float *fdata = ( float* ) m_Data; if ( m_Global ) fdata = ( float* ) m_Global->m_Data;
			constants->SetMatrix( dev, m_Handle, (D3DXMATRIX*)fdata ); return; }
	case MATRIX_ARRAY:{
			float *fdata = ( float* ) m_Data; if ( m_Global ) fdata = ( float* ) m_Global->m_Data;
			constants->SetMatrixArray ( dev, m_Handle, (D3DXMATRIX*)fdata, m_Count ); return; }
	};
}

bool Shader_DX9_Shader::SetVariable(const std::string & name,Shader_DX9_Variable::Type type,
									   unsigned int count,unsigned int esize, void *data)
{
	Shader_DX9_Variable *var = NULL;

	ListFor(Shader_DX9_Variable*,m_Variables,i)
	{
		if(name == (*i)->m_Name && type == (*i)->m_Type){
			var = (*i);
			break;
		}
	}

	if(var)
	{	
		if(count != var->m_Count)
			THROW_EXCEPTION("Incoherent dimensions for the \"" + name + "\" variable");
		
		if(var->m_Data)
			memcpy(var->m_Data,data,esize*count);
		else
			THROW_EXCEPTION("No allocated memory for the \"" + name + "\" variable");
		
		return true;
	}
	else
	{
		D3DXHANDLE handle = m_Constants->GetConstantByName(NULL,name.c_str());

		if(!handle) // Variable not found.
			return false;
		
		Shader_DX9_Variable *nvar = new Shader_DX9_Variable();
		nvar->m_Count = count;
		nvar->m_Type = type;
		nvar->m_Name = name;
		nvar->m_Handle = handle;
		nvar->m_Data = malloc(esize*count);
		memcpy(nvar->m_Data,data,esize*count);

		m_Variables.push_back(nvar);
		return true;
	}

	return false;
}

Shader_DX9_Vextex::Shader_DX9_Vextex(Device_DX9 *dev)
{
	m_pName = "Main";
	m_gDevice = dev;
	m_Shader = NULL;
	m_ModelMatrix = NULL;
	m_ModelViewMatrix = NULL;
	m_ProjectionModelViewMatrix = NULL;
	m_NormalMatrix = NULL;
	m_DiffuseColor = NULL;
	m_SpecularColor = NULL;
	m_AmbientColor = NULL;
	m_SpecularPower = NULL;
	m_BoneMatrix = NULL;
	m_BoneCount = NULL;
	m_ProjectionViewMatrix = NULL;
	m_ViewMatrix = NULL;
	m_Constants = NULL;
}

Shader_DX9_Vextex::~Shader_DX9_Vextex()
{
	ListFor(Shader_DX9_Variable*,m_Variables,i)
		delete (*i);
	m_Variables.clear();
	if(m_Constants)
		m_Constants->Release();
	SafeRelease(m_Shader);
}

bool Shader_DX9_Vextex::Load(const std::string & code_src, ShaderModelType sm )
{
	std::string shd_format = "";

	switch(sm)
	{
	case SHADER_MODEL_1:
		shd_format = "vs_1_1";
		break;
	case SHADER_MODEL_2:
		shd_format = "vs_2_0";
		break;
	case SHADER_MODEL_2A:
		shd_format = "vs_2_a";
		break;
	case SHADER_MODEL_2B:
		shd_format = "vs_2_b";
		break;
	case SHADER_MODEL_3:
		shd_format = "vs_3_0";
		break;
	case SHADER_MODEL_4:
		shd_format = "vs_4_0";
		break;
	};

	LPD3DXBUFFER	l_CompiledCode;
	DWORD			l_ShaderFlags = 0;
	LPD3DXBUFFER	l_BufferErrors = NULL;

	if(SUCCEEDED(D3DXCompileShader( code_src.c_str(),(UINT)code_src.length(), NULL, NULL, "gphMainVSH",
							shd_format.c_str(), l_ShaderFlags, &l_CompiledCode,
							&l_BufferErrors, &m_Constants )))
	{
		if(FAILED(m_gDevice->m_D3Ddevice->CreateVertexShader(
			(DWORD*)l_CompiledCode->GetBufferPointer(), &m_Shader )))
			THROW_EXCEPTION("Unable to create vertex shader");
	}
	else
	{
		LPVOID pCompilErrors = l_BufferErrors->GetBufferPointer();
		THROW_EXCEPTION(std::string((const char*)pCompilErrors));
	}

	m_ViewMatrix = m_Constants->GetConstantByName(NULL,"gphViewMatrix");
	m_ModelMatrix = m_Constants->GetConstantByName(NULL,"gphModelMatrix");
	m_ModelViewMatrix = m_Constants->GetConstantByName(NULL,"gphModelViewMatrix");
	m_ProjectionModelViewMatrix = m_Constants->GetConstantByName(NULL,"gphProjectionModelViewMatrix");
	m_NormalMatrix = m_Constants->GetConstantByName(NULL,"gphNormalMatrix");
	m_DiffuseColor = m_Constants->GetConstantByName(NULL,"gphDiffuseColor");
	m_SpecularColor = m_Constants->GetConstantByName(NULL,"gphSpecularColor");
	m_AmbientColor = m_Constants->GetConstantByName(NULL,"gphAmbientColor");
	m_SpecularPower= m_Constants->GetConstantByName(NULL,"gphSpecularPower");
	m_ProjectionViewMatrix = m_Constants->GetConstantByName(NULL,"gphProjectionViewMatrix");

	return true;
}

void Shader_DX9_Vextex::Disable()
{
	m_gDevice->m_D3Ddevice->SetVertexShader(NULL);
}

void Shader_DX9_Vextex::Enable()
{
	LPDIRECT3DDEVICE9 dev = m_gDevice->m_D3Ddevice;

	D3DXMATRIX l_ViewMatrix;
	D3DXMATRIX l_ModelMatrix;

	dev->GetTransform(D3DTS_VIEW,&l_ViewMatrix);
	dev->GetTransform(D3DTS_WORLD,&l_ModelMatrix);

	if(m_ViewMatrix)
		m_Constants->SetMatrix(dev,	m_ViewMatrix,
								&(l_ViewMatrix) );
	
	if(m_ProjectionViewMatrix)
		 m_Constants->SetMatrix(dev,	m_ProjectionViewMatrix,
								&(m_gDevice->m_ProjectionViewMatrix) );

	if(m_ModelMatrix)
		m_Constants->SetMatrix( dev, m_ModelMatrix, &l_ModelMatrix );

	if(m_ModelViewMatrix)
		m_Constants->SetMatrix( dev, m_ModelViewMatrix,
								&(l_ModelMatrix*l_ViewMatrix) );

	if(m_ProjectionModelViewMatrix) m_Constants->SetMatrix(dev,	m_ProjectionModelViewMatrix,
								&(l_ModelMatrix*m_gDevice->m_ProjectionViewMatrix) );

	if(m_NormalMatrix){
		D3DXMATRIX m_IT,m_IT_A;
		D3DXMatrixInverse( &m_IT_A, NULL, &( l_ModelMatrix * l_ViewMatrix  ) );
		D3DXMatrixTranspose( &m_IT, &m_IT_A );
		m_Constants->SetMatrix( dev, m_NormalMatrix, &m_IT);
	}

	if(m_DiffuseColor) m_Constants->SetFloatArray( dev,	m_DiffuseColor,	(float*)&m_gDevice->m_DiffuseColor,4);
	if(m_SpecularColor) m_Constants->SetFloatArray( dev, m_SpecularColor, (float*)&m_gDevice->m_SpecularColor,4);
	if(m_AmbientColor) m_Constants->SetFloatArray( dev,	m_AmbientColor, (float*)&m_gDevice->m_AmbientColor,4);
	if(m_SpecularPower) m_Constants->SetFloatArray( dev,m_AmbientColor, (float*)&m_gDevice->m_SpecularPower,1);

	ListFor(Shader_DX9_Variable*,m_Variables,i)
		(*i)->Bind(m_Constants,dev);

	dev->SetVertexShader(m_Shader);
}


Shader_DX9_Fragment::Shader_DX9_Fragment(Device_DX9 *dev)
{
	m_pName = "Main";
	m_gDevice = dev;
	m_DiffuseColor = NULL;
	m_SpecularColor = NULL;
	m_AmbientColor = NULL;
	m_SpecularPower = NULL;
	m_Shader = NULL;
	m_Constants = NULL;
}

Shader_DX9_Fragment::~Shader_DX9_Fragment()
{
	ListFor(Shader_DX9_Variable*,m_Variables,i)
		delete (*i);
	m_Variables.clear();

	if(m_Constants)
		m_Constants->Release();
	SafeRelease(m_Shader);
}

bool Shader_DX9_Fragment::Load(const std::string & code_src, ShaderModelType sm )
{
	std::string shd_format = "";

	switch(sm)
	{
	case SHADER_MODEL_1:
		shd_format = "ps_1_1";break;
	case SHADER_MODEL_2:
		shd_format = "ps_2_0";break;
	case SHADER_MODEL_2A:
		shd_format = "ps_2_a";break;
	case SHADER_MODEL_2B:
		shd_format = "ps_2_b";break;
	case SHADER_MODEL_3:
		shd_format = "ps_3_0";break;
	case SHADER_MODEL_4:
		shd_format = "ps_4_0";break;
	};

	LPD3DXBUFFER	l_CompiledCode;
	DWORD			l_ShaderFlags = 0;
	LPD3DXBUFFER	l_BufferErrors = NULL;

	if(SUCCEEDED(D3DXCompileShader( code_src.c_str(),(UINT)code_src.length(), NULL, NULL, "gphMainPSH",
							shd_format.c_str(), l_ShaderFlags, &l_CompiledCode,
							&l_BufferErrors, &m_Constants )))
	{
		if(FAILED(m_gDevice->m_D3Ddevice->CreatePixelShader(
					(DWORD*)l_CompiledCode->GetBufferPointer(), &m_Shader )))
				THROW_EXCEPTION("Unable to create \"fragment\" shader");
	}
	else
	{
		LPVOID pCompilErrors = l_BufferErrors->GetBufferPointer();
		THROW_EXCEPTION(std::string((const char*)pCompilErrors));
		return false;
	}
	m_DiffuseColor = m_Constants->GetConstantByName(NULL,"gphDiffuseColor");
	m_SpecularColor = m_Constants->GetConstantByName(NULL,"gphSpecularColor");
	m_AmbientColor = m_Constants->GetConstantByName(NULL,"gphAmbientColor");
	m_SpecularPower= m_Constants->GetConstantByName(NULL,"gphSpecularPower");
	return true;

}

void Shader_DX9_Fragment::Disable()
{
	m_gDevice->m_D3Ddevice->SetPixelShader(NULL);
}

void Shader_DX9_Fragment::Enable()
{
	LPDIRECT3DDEVICE9 dev = m_gDevice->m_D3Ddevice;

	if(m_DiffuseColor) m_Constants->SetFloatArray( dev,	m_DiffuseColor,	(float*)&m_gDevice->m_DiffuseColor,4);
	if(m_SpecularColor) m_Constants->SetFloatArray( dev, m_SpecularColor, (float*)&m_gDevice->m_SpecularColor,4);
	if(m_AmbientColor) m_Constants->SetFloatArray( dev,	m_AmbientColor, (float*)&m_gDevice->m_AmbientColor,4);
	if(m_SpecularPower) m_Constants->SetFloatArray( dev,m_SpecularPower, (float*)&m_gDevice->m_SpecularPower,1);

	ListFor(Shader_DX9_Variable*,m_Variables,i)
		(*i)->Bind(m_Constants,dev);

	dev->SetPixelShader(m_Shader);
}

#endif // #if defined(NCK_DIRECTX9)

_GRAPHICS_END
