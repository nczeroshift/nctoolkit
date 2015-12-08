
/**
 * NCtoolKit © 2007-2015 Lu’s F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckGraphics_gl2.h"
#include "nckException.h"
#include "nckUtils.h"

_GRAPHICS_BEGIN

#if defined(NCK_OPENGL2)

Mesh_GL2::Mesh_GL2(Device_GL2 *dev){
	m_Device = dev;
	m_VertexBuffer = 0;
	m_VerticeCount = 0;
}

void Mesh_GL2::Load(uint32_t vertices,VertexProfile profile, void * vertexdata,
		uint32_t face_groups,uint32_t *face_count, uint32_t **faces){
	m_VertexProfile = profile;

	glGenBuffers(1, &m_VertexBuffer);

	if(!m_VertexBuffer)
		THROW_EXCEPTION("Unable to create vertices buffer");

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices*profile.GetVertexSize(), vertexdata, GL_STATIC_DRAW);
	m_VerticeCount = vertices;

	int arraysize = 0;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &arraysize);

	if(!arraysize)
		THROW_EXCEPTION("Unable to copy vertices buffer");

	glBindBuffer( GL_ARRAY_BUFFER, GL_ZERO );

	m_IndexBuffer.reserve(face_groups);
	for(unsigned int i = 0;i<face_groups;i++)
	{
		FacesGroup group;
		group.count = face_count[i];


		glGenBuffers(1, &group.buffer);

		if(!group.buffer)
			THROW_EXCEPTION("Unable to create triangle index buffer");

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, group.buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,group.count*sizeof(uint32_t)*3, faces[i], GL_STATIC_DRAW_ARB);

		int arraysize = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &arraysize);

		if(!arraysize)
			THROW_EXCEPTION("Unable to copy triangle index buffer");

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_ZERO );

		m_IndexBuffer.push_back(group);

	}

	glBindBuffer( GL_ARRAY_BUFFER, GL_ZERO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_ZERO );
}

Mesh_GL2::~Mesh_GL2()
{
	glDeleteBuffers(1, &m_VertexBuffer);

	for(unsigned int i = 0;i<m_IndexBuffer.size();i++){
		glDeleteBuffers(1, &(m_IndexBuffer[i].buffer) );
	}
}

void Mesh_GL2::Enable(){
	m_Device->UpdateMatrix();

	glBindBuffer( GL_ARRAY_BUFFER, m_VertexBuffer );
	uint32_t vertexsize = m_VertexProfile.GetVertexSize();
	
	for(unsigned int i = 0;i<m_VertexProfile.Size();i++)
	{
		void *basepointer = NULL;

		switch (m_VertexProfile[i].type)
		{
		case VERTEX_POSITION:
			glVertexPointer( m_VertexProfile[i].components,GL_FLOAT,vertexsize ,(char*) basepointer + m_VertexProfile[i].offset);
			glEnableClientState(GL_VERTEX_ARRAY);
			break;
                
		case VERTEX_NORMAL:
			glNormalPointer(GL_FLOAT, vertexsize,(char*) basepointer + m_VertexProfile[i].offset);
			glEnableClientState(GL_NORMAL_ARRAY);
			break;

		case VERTEX_UV:
			glActiveTexture(GL_TEXTURE0_ARB+ m_VertexProfile[i].index);
			glClientActiveTexture(GL_TEXTURE0_ARB + m_VertexProfile[i].index);
			glTexCoordPointer(m_VertexProfile[i].components, GL_FLOAT, vertexsize,(char*) basepointer + m_VertexProfile[i].offset);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			break;

		case VERTEX_COLOR:
			glColorPointer(m_VertexProfile[i].components,GL_UNSIGNED_BYTE,vertexsize,(char*) basepointer + m_VertexProfile[i].offset);
			glEnableClientState(GL_COLOR_ARRAY);
			break;
		}
	}
}

void Mesh_GL2::Disable(){
	glBindBuffer( GL_ARRAY_BUFFER, GL_ZERO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_ZERO);
}

void Mesh_GL2::Render(unsigned int subset)
{
	m_Device->m_TextureCache.Check();
	
	if(m_Device->m_ActiveProgram){
		if(m_Device->m_ActiveProgram->m_ToRemove)
			m_Device->m_ActiveProgram->DisableInternal();
		else
			m_Device->m_ActiveProgram->EnableInternal();
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_IndexBuffer[subset].buffer);
	glDrawElements(GL_TRIANGLES, m_IndexBuffer[subset].count *3, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_ZERO);
}

void Mesh_GL2::Unlock(){
}

void* Mesh_GL2::Lock(){
	return NULL;
}

unsigned int Mesh_GL2::GetSubSetCount(){
	return (unsigned int)m_IndexBuffer.size();
}

unsigned int Mesh_GL2::GetVerticesCount(){
	return m_VerticeCount;
}

unsigned int Mesh_GL2::GetFacesCount()
{
	GLuint totalFaces = 0;
	for(uint32_t i=0;i<m_IndexBuffer.size();i++)
		totalFaces+=m_IndexBuffer[i].count;
	return totalFaces;
}

void Mesh_GL2::GetVertexProfile(VertexProfile *p){
	*p = m_VertexProfile;
}

DynamicMesh_GL2::DynamicMesh_GL2(Device_GL2 *dev){
	m_Device = dev;
	m_MaxVertices = 0;
	m_IndexBuffer = 0;
}

DynamicMesh_GL2::~DynamicMesh_GL2(){
	SafeArrayDelete(m_VertexBuffer);
	SafeArrayDelete(m_IndexBuffer);
}

void DynamicMesh_GL2::Create(int maxVertices, int maxTriangles){
	m_MaxVertices = maxVertices;
	m_MaxTriangles = maxTriangles;

	m_VertexBuffer = new float[maxVertices*10];
	m_IndexBuffer = new int[maxTriangles*3];
}

void DynamicMesh_GL2::Enable()
{
	if(m_Device->m_ActiveProgram){
		if(m_Device->m_ActiveProgram->m_ToRemove){
			m_Device->m_ActiveProgram->DisableInternal();
		}
		else
			m_Device->m_ActiveProgram->EnableInternal();
	}

	glEnableClientState (GL_TEXTURE_COORD_ARRAY);

	glActiveTexture(GL_TEXTURE0_ARB);
	glClientActiveTexture(GL_TEXTURE0_ARB);
	glTexCoordPointer(2,GL_FLOAT,40,(void*) (((float*)(m_VertexBuffer))+6));

	glEnableClientState (GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT,40, (void*) (((float*)(m_VertexBuffer))+3));

	glEnableClientState (GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,40,(void*)m_VertexBuffer);
}

void DynamicMesh_GL2::Disable(){

	glDisableClientState (GL_NORMAL_ARRAY);
	glDisableClientState (GL_VERTEX_ARRAY);
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
}

void DynamicMesh_GL2::Render(){
	glDrawElements(GL_TRIANGLES,m_TrianglesCount*3,GL_UNSIGNED_INT,(void*)m_IndexBuffer);
}

void DynamicMesh_GL2::LockVBO(void ** data){
	*data = (void*)m_VertexBuffer;
}

void DynamicMesh_GL2::UnlockVBO(){

}

void DynamicMesh_GL2::LockIBO(void ** data){
	*data = (void*)m_IndexBuffer;
}

void DynamicMesh_GL2::UnlockIBO(){

}

unsigned int DynamicMesh_GL2::GetMaxVertices(){
	return m_MaxVertices;
}

unsigned int DynamicMesh_GL2::GetMaxTriangles(){
	return m_MaxTriangles;
}

unsigned int DynamicMesh_GL2::GetVerticesCount(){
	return m_VerticesCount;
}

unsigned int DynamicMesh_GL2::GetTrianglesCount(){
	return m_TrianglesCount;
}

void DynamicMesh_GL2::SetVerticesCount(unsigned int count){
	m_VerticesCount = count;
}

void DynamicMesh_GL2::SetTrianglesCount(unsigned int count){
	m_TrianglesCount = count;
}

#endif // #if defined(NCK_OPENGL2)

_GRAPHICS_END
