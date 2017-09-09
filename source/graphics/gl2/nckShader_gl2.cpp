
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckGraphics_gl2.h"
#include "nckException.h"
#include "nckUtils.h"
#include <string.h>
#include "nckDataIO.h"
#include "../nckShaderParser.h"

#include <stdio.h>

_GRAPHICS_BEGIN

#if defined(NCK_OPENGL2)
	
Program_GL2::Program_GL2(Device_GL2 *dev){
	m_Device = dev;
	m_VertexShader = 0;
	m_FragmentShader = 0;
	m_Active = false;
	m_ToRemove = false;

    m_LastModified = 0;
	m_ViewMatrix = -1;
	m_ModelMatrix = -1;
	m_ModelViewMatrix = -1;
	m_ProjectionModelViewMatrix = -1;
	m_ProjectionViewMatrix = -1;
	m_NormalMatrix = -1;
	for(int i=0;i<8;i++)
		m_TextureSampler[i] = -1;
	m_DiffuseColor = -1;
	m_SpecularColor = -1;
	m_AmbientColor = -1;
	m_SpecularPower = -1;
	m_BoneMatrix = -1;
	m_BoneCount = -1;
#ifdef NCK_GRAPH_RES_PROXY
	m_Proxy = NULL;
#endif
}

Program_GL2::~Program_GL2()
{
	
#ifdef NCK_GRAPH_RES_PROXY
	// Check if GPU resource can be released.
	if(!m_Device->RemoveProgramProxy(this))
		return;
#endif

	for(std::map<std::string,Shader_GL2_Variable*>::iterator i = m_Variables.begin();i!=m_Variables.end();i++){
		SafeDelete(i->second);
	}
	
	if(m_Device->m_ActiveProgram == this){
		DisableInternal();
	}

	if(m_VertexShader)
		glDeleteShader(m_VertexShader);

	if(m_FragmentShader)
		glDeleteShader(m_FragmentShader);

	if(m_Program)
		glDeleteShader(m_Program);
}

void Program_GL2::Load(const std::string & source){
	std::list<std::string> vertex;
	std::list<std::string> fragment;

	std::map<std::string,std::list<std::string>> shaderMap = ShaderParser::Map(source);

	if(shaderMap.find("vertex_shader_glx2")!=shaderMap.end())
	{
		vertex = shaderMap.find("vertex_shader_glx2")->second;

		if(shaderMap.find("fragment_shader_glx2")!=shaderMap.end())
		{
			fragment = shaderMap.find("fragment_shader_glx2")->second;
		}
	}

	if(vertex.size()==0 || vertex.back().size() == 0)
		THROW_EXCEPTION("Vertex shader required");
    
	// Load vertex shader
	{
        int count = vertex.size();
        char **sources = new char*[count + 2];
        sources[0] = "#version 120\n#define VSH\n";
        sources[count+1] = NULL;

        int index = 1;
        ListFor(std::string, vertex, i) {
            const std::string src = *i;
            sources[index] = (char*)calloc((*i).size() + 1,1);
            memcpy(sources[index], src.c_str(), src.size());
            index++;
        }

        GLuint vshd = m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vshd, count+1, sources, NULL);
        glCompileShader(vshd);

        int isCompiled;
        glGetShaderiv(vshd, GL_COMPILE_STATUS, &isCompiled);

        for (int i = 1; i < count; i++)
            free(sources[i]);
        
        delete[] sources;

        if (!isCompiled) {
            int maxLength = 4096;
            char buffer[4096] = "";
            glGetShaderiv(vshd, GL_INFO_LOG_LENGTH, &maxLength);
            glGetShaderInfoLog(vshd, maxLength, &maxLength, buffer);
            THROW_EXCEPTION(std::string("Vertex shader compiled error : ") + std::string(buffer));
        }
	}

	// Load fragment shader
	if(fragment.size()>0)
	{
        int count = fragment.size();
        char **sources = new char*[count + 2];
        sources[0] = "#version 120\n#define FSH\n";
        sources[count + 1] = NULL;

        int index = 1;
        ListFor(std::string, fragment, i) {
            const std::string src = *i;
            sources[index] = (char*)calloc((*i).size() + 1, 1);
            memcpy(sources[index], src.c_str(), src.size());
            index++;
        }

        GLuint fshd = m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fshd, count+1, sources, NULL);
        glCompileShader(fshd);

        int isCompiled;
        glGetShaderiv(count+1, GL_COMPILE_STATUS, &isCompiled);

        for (int i = 1; i < count; i++)
            free(sources[i]);

        delete[] sources;

        if (!isCompiled) {
            int maxLength = 4096;
            char buffer[4096] = "";
            glGetShaderiv(fshd, GL_INFO_LOG_LENGTH, &maxLength);
            glGetShaderInfoLog(fshd, maxLength, &maxLength, buffer);

            THROW_EXCEPTION(std::string("Fragment shader compiled error : ") + std::string(buffer));
        }
	}

	m_Program =  glCreateProgram();
	glAttachShader( m_Program, m_VertexShader);
	if(m_FragmentShader)
		glAttachShader( m_Program, m_FragmentShader);

	glLinkProgram(m_Program);

	int isLinked;
	glGetProgramiv(m_Program,GL_LINK_STATUS,&isLinked);
	if(!isLinked){
		int maxLength = 4096;
		char buffer[4096]="";
		glGetProgramiv(m_Program, GL_INFO_LOG_LENGTH, &maxLength);
		glGetProgramInfoLog(m_Program, maxLength, &maxLength, buffer);

		glDeleteShader(m_VertexShader);
		if(m_FragmentShader)
			glDeleteShader(m_FragmentShader);

		glDeleteProgram(m_Program);

		THROW_EXCEPTION(std::string("Program link error : ")+std::string(buffer));
	}

    BindUniforms();
}

void Program_GL2::BindUniforms() {

    m_TextureSampler[0] = glGetUniformLocation(m_Program, "gphTexture0");
    m_TextureSampler[1] = glGetUniformLocation(m_Program, "gphTexture1");
    m_TextureSampler[2] = glGetUniformLocation(m_Program, "gphTexture2");
    m_TextureSampler[3] = glGetUniformLocation(m_Program, "gphTexture3");
    m_TextureSampler[4] = glGetUniformLocation(m_Program, "gphTexture4");
    m_TextureSampler[5] = glGetUniformLocation(m_Program, "gphTexture5");
    m_TextureSampler[6] = glGetUniformLocation(m_Program, "gphTexture6");
    m_TextureSampler[7] = glGetUniformLocation(m_Program, "gphTexture7");

    for (int i = 0; i<8; i++)
        if (m_TextureSampler[i] != -1)
            glUniform1i(m_TextureSampler[i], i);

    m_DiffuseColor = glGetUniformLocation(m_Program, "gphDiffuseColor");
    m_SpecularColor = glGetUniformLocation(m_Program, "gphSpecularColor");
    m_SpecularPower = glGetUniformLocation(m_Program, "gphSpecularPower");
    m_AmbientColor = glGetUniformLocation(m_Program, "gphAmbientColor");
    m_ModelMatrix = glGetUniformLocation(m_Program, "gphModelMatrix");
    m_ModelViewMatrix = glGetUniformLocation(m_Program, "gphModelViewMatrix");
    m_NormalMatrix = glGetUniformLocation(m_Program, "gphNormalMatrix");
    m_Alpha = glGetUniformLocation(m_Program, "gphAlpha");

    m_ProjectionModelViewMatrix = glGetUniformLocation(m_Program, "gphPMVMatrix");
    m_ProjectionViewMatrix = glGetUniformLocation(m_Program, "gphProjectionViewMatrix");
    m_ViewMatrix = glGetUniformLocation(m_Program, "gphViewMatrix");

    GLint m_ProjMatrix = glGetUniformLocation(m_Program, "gphProjectionMatrix");

    GLint m_ModelNormalMatrix = glGetUniformLocation(m_Program, "gphModelNormalMatrix");

    if (m_ProjMatrix > -1) {
        float mat[16];
        glGetFloatv(GL_PROJECTION_MATRIX, mat);
        glUniformMatrix4fv(m_ProjMatrix, 1, false, (float*)mat);
    }

    if (m_ModelNormalMatrix > -1) {
        Math::Mat44 inv,trans;
        Math::Invert(m_Device->m_ModelMatrix, &inv);
        trans = Math::Transpose(inv);
        glUniformMatrix4fv(m_ModelNormalMatrix, 1, false, (float*)&trans);
    }

    if (m_ModelMatrix > -1)
        glUniformMatrix4fv(m_ModelMatrix, 1, false, (float*)&m_Device->m_ModelMatrix);

    if (m_ViewMatrix > -1)
        glUniformMatrix4fv(m_ViewMatrix, 1, false, (float*)&m_Device->m_ViewMatrix);

    if(m_Alpha > -1)
        glUniform1f(m_Alpha, m_Device->m_Alpha);

    if (m_DiffuseColor > -1)
        glUniform4f(m_DiffuseColor, m_Device->m_DiffuseColor[0],
            m_Device->m_DiffuseColor[1],
            m_Device->m_DiffuseColor[2],
            m_Device->m_DiffuseColor[3]);

    if (m_SpecularColor > -1)
        glUniform4f(m_SpecularColor, m_Device->m_SpecularColor[0],
            m_Device->m_SpecularColor[1],
            m_Device->m_SpecularColor[2],
            m_Device->m_SpecularColor[3]);

    if (m_SpecularPower > -1)
        glUniform1f(m_SpecularPower, m_Device->m_SpecularPower);

    if (m_AmbientColor > -1)
        glUniform4f(m_AmbientColor, m_Device->m_AmbientColor[0],
            m_Device->m_AmbientColor[1],
            m_Device->m_AmbientColor[2],
            m_Device->m_AmbientColor[3]);


    /*
    if(m_ModelMatrix>-1)
    glUniformMatrix4fv(m_ModelMatrix,1,false,(float*)&m_Device->m_ModelMatrix);

    if(m_ModelViewMatrix>-1)
    glUniformMatrix4fv(m_ModelViewMatrix,1,false,(float*)&m_Device->m_ModelViewMatrix);

    if(m_ProjectionViewMatrix>-1)
    glUniformMatrix4fv(m_ProjectionViewMatrix,1,false,(float*)&m_Device->m_ProjectionViewMatrix);

    if(m_ProjectionModelViewMatrix>-1)
    glUniformMatrix4fv(m_ProjectionViewMatrix,1,false,(float*)&m_Device->m_ProjectionViewModelMatrix);*/
}

int Program_GL2::Reload() {
    int64_t currentModified = Core::GetFileLastModified(m_Filename);

    if (m_LastModified == 0 || currentModified - m_LastModified < 1) {
        return 0;
    }

    Core::DataReader * f = Core::DataReader::Open(m_Filename);

    if (!f) {
        THROW_EXCEPTION("File \"" + m_Filename+"\" not found");
    }

    int size = f->Length();//(int)Core::FileReader::Size(m_Filename);

    char * buffer = new char[size + 1];

    f->Read(buffer, size);

    buffer[size] = '\0';

    delete f;

    std::string source = std::string(buffer);

    delete[] buffer;

    GLint oldProgram = m_Program;
    GLint oldVsh = m_VertexShader;
    GLint oldFsh = m_FragmentShader;
    try{
        Load(source);
    }
    catch (const Core::Exception & e) {
        THROW_EXCEPTION_STACK("Unable to reload shader" + m_Filename, e);
    }

    if (oldProgram != m_Program) {
        
        for (std::map<std::string, Shader_GL2_Variable*>::iterator i = m_Variables.begin(); i != m_Variables.end(); i++) {
            SafeDelete(i->second);
        }
        m_Variables.clear();

        if (m_Device->m_ActiveProgram == this) 
            DisableInternal();

        BindUniforms();

        if (oldVsh) glDeleteShader(oldVsh);
        if (oldFsh) glDeleteShader(oldFsh);
        if (oldProgram) glDeleteShader(oldProgram);

        m_LastModified = currentModified;

        return 1;
    } 

    return -1;
}

void Program_GL2::LoadFromFilename(const std::string & filename)
{
#ifdef NCK_GRAPH_RES_PROXY
	ResourceProxy<Program_GL2>  * proxy = NULL;
	Program_GL2 * refProg = NULL;

	if((proxy = m_Device->GetProgramProxy(filename)) != NULL && (refProg = proxy->GetReference()) != NULL){
		m_VertexShader = refProg->m_VertexShader;
		m_FragmentShader = refProg->m_FragmentShader;
		m_Active = false;
		m_ToRemove = false;

		m_ViewMatrix = refProg->m_ViewMatrix ;
		m_ModelMatrix = refProg->m_ModelMatrix;
		m_ModelViewMatrix = refProg->m_ModelViewMatrix;
		m_ProjectionModelViewMatrix = refProg->m_ProjectionModelViewMatrix ;
		m_ProjectionViewMatrix = refProg->m_ProjectionViewMatrix;
		m_NormalMatrix = refProg->m_NormalMatrix;
		
		for(int i=0;i<8;i++)
			m_TextureSampler[i] = refProg->m_TextureSampler[i];
			
		m_DiffuseColor = refProg->m_DiffuseColor;
		m_SpecularColor = refProg->m_SpecularColor;
		m_AmbientColor = refProg->m_AmbientColor;
		m_SpecularPower = refProg->m_SpecularPower;
		m_BoneMatrix = refProg->m_BoneMatrix;
		m_BoneCount = refProg->m_BoneCount;
		
		m_Device->AddProgramProxy(filename,this);	
		
		return;
	}
#endif 
	Core::DataReader * f = Core::DataReader::Open(filename);

	if(!f)
		THROW_EXCEPTION("File \"" + filename + "\" not found");

    m_Filename = filename;
    m_LastModified = Core::GetFileLastModified(filename);

    int size = f->Length();//(int)Core::FileReader::Size(filename);

	char * buffer = new char[size+1];

	f->Read(buffer,size);

	buffer[size]='\0';

	delete f;

	std::string source = std::string(buffer);

	delete [] buffer;

	Load(source);
	
#ifdef NCK_GRAPH_RES_PROXY
	m_Proxy = m_Device->AddProgramProxy(filename,this);	
#endif
}

void Program_GL2::EnableInternal(){
    if(!m_Active){
        glUseProgram(m_Program);
		for(std::map<std::string,Shader_GL2_Variable*>::iterator i = m_Variables.begin();i!=m_Variables.end();i++){
			(*i).second->Bind();
		}
	}
	else{
		for(std::map<std::string,Shader_GL2_Variable*>::iterator i = m_Variables.begin();i!=m_Variables.end();i++){
			if((*i).second->m_NeedsUpdate)
				(*i).second->Bind();
		}
	}

    BindUniforms();
    
	m_Active = true;
}

void Program_GL2::Enable(){
	if(m_Device->m_ActiveProgram && m_Device->m_ActiveProgram!=this){
		m_Device->m_ActiveProgram->DisableInternal();
		m_Active = false;
	}

	m_Device->m_ActiveProgram = this;
	m_ToRemove = false;
}

void Program_GL2::Disable(){
	m_ToRemove = true;
}

void Program_GL2::DisableInternal(){
	m_ToRemove = false;
	m_Active = false;
	glUseProgram(GL_ZERO);
	m_Device->m_ActiveProgram = NULL;
}

Shader_GL2_Variable * entryGetter(const std::map<std::string,Shader_GL2_Variable*> & map, const std::string & key)
{
	std::map<std::string,Shader_GL2_Variable*>::const_iterator it = map.find(key);

	if(it == map.end())
		return NULL;
	else
		return (*it).second;
}

int Program_GL2::GetVariableId(const std::string & name){
	return glGetUniformLocation(m_Program,name.c_str());
}

bool Program_GL2::SetVariable1i(const std::string & name,int value){

	Shader_GL2_Variable *var = entryGetter(m_Variables,name);
	if(!var){
		GLint id = glGetUniformLocation(m_Program,name.c_str());
		if(id==-1)
			return false;
		else
		{
			var = new Shader_GL2_Variable();
			var->m_Name = name;
			var->m_Handle = id;
			var->m_Type = Shader_GL2_Variable::INT1;
			var->m_Count = 1;
			var->m_Data = new int;
			*((int*)var->m_Data) = value;
			m_Variables.insert(std::pair<std::string,Shader_GL2_Variable*>(name,var));
		}
	}
	*((int*)var->m_Data) = value;
	return true;
}

bool Program_GL2::SetVariable1f(const std::string & name,float value){
	Shader_GL2_Variable *var = entryGetter(m_Variables,name);
	if(!var){
		GLint id = glGetUniformLocation(m_Program,name.c_str());
		if(id==-1)
			return false;
		else
		{
			var = new Shader_GL2_Variable();
			var->m_Name = name;
			var->m_Handle = id;
			var->m_Type = Shader_GL2_Variable::FLOAT1;
			var->m_Count = 1;
			var->m_Data = new float;
			m_Variables.insert(std::pair<std::string,Shader_GL2_Variable*>(name,var));
		}
	}
	*((float*)var->m_Data) = value;
	return true;
}

bool Program_GL2::SetVariable4i(const std::string & name,int x,int y,int z,int w){
	Shader_GL2_Variable *var = entryGetter(m_Variables,name);
	if(!var){
		GLint id = glGetUniformLocation(m_Program,name.c_str());
		if(id==-1)
			return false;
		else
		{
			var = new Shader_GL2_Variable();
			var->m_Name = name;
			var->m_Handle = id;
			var->m_Type = Shader_GL2_Variable::INT4;
			var->m_Count = 1;
			var->m_Data = new int[4];
			m_Variables.insert(std::pair<std::string,Shader_GL2_Variable*>(name,var));
		}
	}
	((int*)var->m_Data)[0] = x;
	((int*)var->m_Data)[1] = y;
	((int*)var->m_Data)[2] = z;
	((int*)var->m_Data)[3] = w;
	return true;
}

bool Program_GL2::SetVariable4f(const std::string & name,float x,float y,float z,float w){
	Shader_GL2_Variable *var = entryGetter(m_Variables,name);
	if(!var){
		GLint id = glGetUniformLocation(m_Program,name.c_str());
		if(id==-1)
			return false;
		else
		{
			var = new Shader_GL2_Variable();
			var->m_Name = name;
			var->m_Handle = id;
			var->m_Type = Shader_GL2_Variable::FLOAT4;
			var->m_Count = 1;
			var->m_Data = new float[4];
			m_Variables.insert(std::pair<std::string,Shader_GL2_Variable*>(name,var));
		}
	}
	((float*)var->m_Data)[0] = x;
	((float*)var->m_Data)[1] = y;
	((float*)var->m_Data)[2] = z;
	((float*)var->m_Data)[3] = w;
	return true;
}

bool Program_GL2::SetArray1f(const std::string & name,int count,const float *value){
	Shader_GL2_Variable *var = entryGetter(m_Variables,name);
	if(!var){
		GLint id = glGetUniformLocation(m_Program,name.c_str());
		if(id==-1)
			return false;
		else
		{
			var = new Shader_GL2_Variable();
			var->m_Name = name;
			var->m_Handle = id;
			var->m_Type = Shader_GL2_Variable::FLOAT1_ARRAY;
			var->m_Count = count;
			var->m_Data = new float[count];
			m_Variables.insert(std::pair<std::string,Shader_GL2_Variable*>(name,var));
		}
	}
	memcpy(var->m_Data,value,sizeof(float)*count);
	return true;
}

bool Program_GL2::SetArray4f(const std::string & name,int count,const float *value){
	Shader_GL2_Variable *var = entryGetter(m_Variables,name);
	if(!var){
		GLint id = glGetUniformLocation(m_Program,name.c_str());
		if(id==-1)
			return false;
		else
		{
			var = new Shader_GL2_Variable();
			var->m_Name = name;
			var->m_Handle = id;
			var->m_Type = Shader_GL2_Variable::FLOAT4_ARRAY;
			var->m_Count = count;
			var->m_Data = new float[4* count];
			m_Variables.insert(std::pair<std::string,Shader_GL2_Variable*>(name,var));
		}
	}
	memcpy(var->m_Data,value,sizeof(float)*4*count);
	return true;
}

bool Program_GL2::SetMatrix(const std::string & name,const float *mat){
	Shader_GL2_Variable *var = entryGetter(m_Variables,name);
	if(!var){
		GLint id = glGetUniformLocation(m_Program,name.c_str());
		if(id==-1)
			return false;
		else
		{
			var = new Shader_GL2_Variable();
			var->m_Name = name;
			var->m_Handle = id;
			var->m_Type = Shader_GL2_Variable::MATRIX;
			var->m_Count = 1;
			var->m_Data = new float[16];
			m_Variables.insert(std::pair<std::string,Shader_GL2_Variable*>(name,var));
		}
	}
	memcpy(var->m_Data,mat,sizeof(float)*16);
	return true;
}

bool Program_GL2::SetMatrixArray(const std::string & name,int count,const float *v){
	Shader_GL2_Variable *var = entryGetter(m_Variables,name);
	if(!var){
		GLint id = glGetUniformLocation(m_Program,name.c_str());
		if(id==-1)
			return false;
		else
		{
			var = new Shader_GL2_Variable();
			var->m_Name = name;
			var->m_Handle = id;
			var->m_Type = Shader_GL2_Variable::MATRIX_ARRAY;
			var->m_Count = count;
			var->m_Data = new float[16*count];
			m_Variables.insert(std::pair<std::string,Shader_GL2_Variable*>(name,var));
		}
	}
	memcpy(var->m_Data,v,sizeof(float)*16*count);
	return true;
}

void Shader_GL2_Variable::Bind()
{
	switch(m_Type)
	{
	case FLOAT1:
		glUniform1f(m_Handle,((float*)m_Data)[0]);
		return;
	case FLOAT4:
		glUniform4f(m_Handle,((float*)m_Data)[0],((float*)m_Data)[1],((float*)m_Data)[2],((float*)m_Data)[3]);
		return;
	case INT1:
		glUniform1i(m_Handle,((int*)m_Data)[0]);
		return;
	case INT4:
		glUniform4i(m_Handle,((int*)m_Data)[0],((int*)m_Data)[1],((int*)m_Data)[2],((int*)m_Data)[3]);
		return;
	case FLOAT1_ARRAY:
		glUniform1fv(m_Handle,m_Count,(float*)m_Data);
		return;
	case FLOAT4_ARRAY:
		glUniform4fv(m_Handle,m_Count,(float*)m_Data);
		return;
	case INT1_ARRAY:
		glUniform1iv(m_Handle,m_Count,(int*)m_Data);
		return;
	case INT4_ARRAY:
		glUniform4iv(m_Handle,m_Count,(int*)m_Data);
		return;
	case MATRIX:
		glUniformMatrix4fv(m_Handle,1,false,(float*)m_Data);
		return;
	case MATRIX_ARRAY:
		glUniformMatrix4fv(m_Handle,m_Count,false,(float*)m_Data);
		return;
	}
}

#if defined(NCK_GRAPH_RES_PROXY)
ResourceProxy<Program_GL2> * Program_GL2::GetProxyManager(){
	return m_Proxy;
}
#endif // #if defined(NCK_GRAPH_RES_PROXY)

#endif // #if defined(NCK_OPENGL2)

_GRAPHICS_END
