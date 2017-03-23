
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckGraphics_dx9.h"
#include "nckException.h"
#include "nckUtils.h"

_GRAPHICS_BEGIN

#if defined _WIN32 || defined _WIN64
#if defined NCK_DIRECTX9

Mesh_DX9::Mesh_DX9(Device_DX9 *dev){
	m_Device = dev; 
	m_Mesh = NULL;
	m_VertexDeclaration = NULL;
}
Mesh_DX9::~Mesh_DX9(){
	SafeRelease(m_Mesh);
	SafeRelease(m_VertexDeclaration);
}
void Mesh_DX9::Render(unsigned int subset)
{
	m_Device->m_D3Ddevice->SetVertexDeclaration(m_VertexDeclaration);
	m_Device->m_TextureCache.Check();
	m_Device->UpdateClipPlanes();

	if(m_Device->m_ActiveProgram)((Program_DX9*)m_Device->m_ActiveProgram)->Enable_Internal();

	m_Mesh->DrawSubset(subset);
}

void Mesh_DX9::Unlock(){
	m_Mesh->UnlockVertexBuffer();
}

void* Mesh_DX9::Lock(){
	void* buffer = NULL;
	m_Mesh->LockVertexBuffer( 0, (void**)&buffer );
	return buffer;
}

unsigned int Mesh_DX9::GetSubSetCount(){
	return 0;
}

unsigned int Mesh_DX9::GetVerticesCount(){
	return m_Mesh->GetNumVertices();
}

unsigned int Mesh_DX9::GetFacesCount()
{
	return m_Mesh->GetNumFaces();
}

void Mesh_DX9::GetVertexProfile(VertexProfile *p){
	p = NULL;
}

void Mesh_DX9::Enable(){
}

void Mesh_DX9::Disable(){
}

DynamicMesh_DX9::DynamicMesh_DX9(Device_DX9 *dev){
	m_Device = dev;
	m_MaxTriangles = 0;
	m_TrianglesCount = 0;
	m_MaxVertices = 0;
	m_VerticesCount = 0;
}

DynamicMesh_DX9::~DynamicMesh_DX9(){
	SafeRelease(m_VertexBuffer);
	SafeRelease(m_IndexBuffer);
}

void DynamicMesh_DX9::Create(int maxVertices, int maxTriangles)
{
	m_MaxVertices = maxVertices;
	m_MaxTriangles = maxTriangles;

	if(!SUCCEEDED(m_Device->m_D3Ddevice->CreateVertexBuffer(maxVertices*sizeof(float)*8,
		D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
		D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1,D3DPOOL_DEFAULT,&m_VertexBuffer,NULL))){
		THROW_EXCEPTION("Unable to create vertex buffer");
	}

	if(!SUCCEEDED(m_Device->m_D3Ddevice->CreateIndexBuffer(maxTriangles*sizeof(int), 
		                           D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
								   D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_IndexBuffer,NULL))){
		THROW_EXCEPTION("Unable to create index buffer");
	}
}

void DynamicMesh_DX9::Enable(){
	m_Device->m_D3Ddevice->SetStreamSource(0,m_VertexBuffer,0,sizeof(float)*8);
	m_Device->m_D3Ddevice->SetIndices(m_IndexBuffer);
}

void DynamicMesh_DX9::Disable(){
	m_Device->m_D3Ddevice->SetFVF(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1);
	m_Device->m_D3Ddevice->SetStreamSource(0,NULL,0,sizeof(float)*8);
	m_Device->m_D3Ddevice->SetIndices(NULL);
}

void DynamicMesh_DX9::Render(){
	m_Device->m_D3Ddevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,0, m_VerticesCount, 
		                                           0, m_TrianglesCount);
}

void DynamicMesh_DX9::LockVBO(void ** data){
	m_VertexBuffer->Lock(0,m_MaxVertices,data,D3DLOCK_DISCARD);
}

void DynamicMesh_DX9::UnlockVBO(){
	m_VertexBuffer->Unlock();
}

void DynamicMesh_DX9::LockIBO(void ** data){
	m_IndexBuffer->Lock(0,m_MaxTriangles,data,D3DLOCK_DISCARD);
}

void DynamicMesh_DX9::UnlockIBO(){
	m_IndexBuffer->Unlock();
}

unsigned int DynamicMesh_DX9::GetMaxVertices(){
	return m_MaxVertices;
}

unsigned int DynamicMesh_DX9::GetMaxTriangles(){
	return m_MaxTriangles;
}

unsigned int DynamicMesh_DX9::GetVerticesCount(){
	return m_VerticesCount;
}

unsigned int DynamicMesh_DX9::GetTriangleCount(){
	return m_TrianglesCount;
}

void DynamicMesh_DX9::SetVerticeCount(unsigned int count){
	m_VerticesCount = count;
}

void DynamicMesh_DX9::SetTrianglesCount(unsigned int count){
	m_TrianglesCount = count;
}

#endif // #if defined NCK_DIRECTX9
#endif // #if defined _WIN32 || defined _WIN64

_GRAPHICS_END


