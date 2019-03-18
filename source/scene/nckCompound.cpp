
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckCompound.h"
#include "nckUtils.h"
#include "nckException.h"
#include "bxonDataIO.h"
#include <algorithm>

_SCENE_BEGIN

static bool objNameSort(Scene::Object* a, Scene::Object* b) { return a->GetName() < b->GetName(); }

RenderControl::RenderControl() { 
	m_Frustum = NULL; 
	m_Material = NULL; 
	m_Layers = LAYER_ALL; 
	m_Pass = MATERIAL_PASS_DEFAULT;
};

RenderControl RenderControl::SetFrustum(Math::Frustum * f) { 
	m_Frustum = f; return *this; 
};

RenderControl RenderControl::SetMaterial(Scene::Material * mat) {
	m_Material = mat;  return *this; 
};

RenderControl RenderControl::SetLayers(LayerMask layers) { 
	m_Layers = layers; return *this; 
};

RenderControl RenderControl::SetPass(MaterialPass pass) { 
	m_Pass = pass;  return *this;
};

Math::Frustum * RenderControl::GetFrustum() const {
	return m_Frustum;
};

Scene::Material * RenderControl::GetMaterial() const {
	return m_Material;
}

LayerMask RenderControl::GetLayers() const{
	return m_Layers;
}

MaterialPass RenderControl::GetPass() const {
	return m_Pass;
}

Compound::~Compound()
{
    ListFor(Camera*,m_Cameras,i)
    delete (*i);
    m_Cameras.clear();
    
    ListFor(Object*,m_Objects,i)
    delete (*i);
    m_Objects.clear();
    
    ListFor(Model*,m_Models,i)
    delete (*i);
    m_Models.clear();
    
    ListFor(Material*,m_Materials,i)
    delete (*i);
    m_Materials.clear();
    
    ListFor(Lamp*,m_Lamps,i)
    delete (*i);
    m_Lamps.clear();
    
    ListFor(Armature*,m_Armatures,i)
    delete (*i);
    m_Armatures.clear();
    
    ListFor(Texture*,m_Textures,i)
    delete (*i);
    m_Textures.clear();
}

DatablockType Compound::GetType(){
    return DATABLOCK_COMPOUND;
}

#ifdef NCK_BXON

bool Compound::ReadBXON(BXON::Map * entry, Processor * processor){
    
    std::map<std::string,Datablock *> tex_map;
    std::map<std::string,Datablock *> mat_map;
    std::map<std::string,Datablock *> mod_map;
    std::map<std::string,Datablock*> cam_map;
    std::map<std::string,Datablock*> arm_map;
    std::map<std::string,Object*> obj_map;
    std::map<std::string,Datablock*> lmp_map;
    std::map<std::string,Datablock*> cur_map;

    if (entry->GetArray("curve")) {
        BXON::Array * curves = entry->GetArray("curve");

        for (unsigned int i = 0; i < curves->GetSize(); i++) {
            Curve * c = new Curve(m_Device);
            BXON::Map * entry = curves->GetMap(i);

            try {
                c->Read(entry);
                cur_map.insert(std::pair<std::string, Datablock *>(c->GetName(), c));
            }
            catch (Core::Exception & ex) {
                delete c;
                THROW_EXCEPTION_STACK("Unable load curve", ex);
            }

            m_Curves.push_back(c);
        }
    }

    if(entry->HasKey("texture")){
        BXON::Array * textures  = entry->GetArray("texture");
        
        for(unsigned int i = 0;i<textures->GetSize() ;i++){
            Texture * tex = new Texture(m_Device);
            
            BXON::Map * entry = textures->GetMap(i);
            
            try{
                tex->Read(entry);
                tex_map.insert(std::pair<std::string, Datablock *>(tex->GetName(), tex));
            }
            catch(Core::Exception & ex){
                const std::string name = tex->GetName();
                delete tex;
                THROW_EXCEPTION_STACK("Unable to load compound texture resource \""+ name +"\"",ex);
            }
            
            if (processor) tex = processor->HandleTexture(tex);

            m_Textures.push_back(tex);
        }
    }
    
    if(entry->HasKey("material")){
        BXON::Array * materials  = entry->GetArray("material");
        
        for(unsigned int i = 0;i<materials->GetSize() ;i++){
            Material * mat = new Material(m_Device);
            
            BXON::Map * entry = materials->GetMap(i);
            
            try{
                mat->Read(entry, tex_map);
                mat_map.insert(std::pair<std::string, Datablock *>(mat->GetName(), mat));
            }
            catch(Core::Exception & ex){
                delete mat;
                THROW_EXCEPTION_STACK("Unable to load material material resource",ex);
            }
            
            if (processor) mat = processor->HandleMaterial(mat);

            m_Materials.push_back(mat);
        }
    }
    
    if(entry->HasKey("armature")){
        BXON::Array * armatures  = entry->GetArray("armature");
        
        for(unsigned int i = 0;i<armatures->GetSize() ;i++){
            Armature * arm = new Armature(m_Device);
            
            BXON::Map * entry = armatures->GetMap(i);
            
            try{
                arm->Read(entry);
                arm_map.insert(std::pair<std::string, Datablock *>(arm->GetName(), arm));
            }
            catch(Core::Exception & ex){
                delete arm;
                THROW_EXCEPTION_STACK("Unable to load compound armature resource",ex);
            }
            
            if (processor) arm = processor->HandleArmature(arm);

            m_Armatures.push_back(arm);
        }
    }
    
    
    if(entry->HasKey("mesh")){
        BXON::Array * models = entry->GetArray("mesh");
        
        for(unsigned int i = 0;i<models->GetSize() ;i++){
            Model * mod = new Model(m_Device);
            BXON::Map * entry = models->GetMap(i);
            
            try{
                mod->Read(entry, mat_map, arm_map, processor);
                mod_map.insert(std::pair<std::string, Datablock *>(mod->GetName(), mod));
            }
            catch(Core::Exception & ex){
                delete mod;
                THROW_EXCEPTION_STACK("Unable to load compound model resource",ex);
            }
            
            m_Models.push_back(mod);
        }
        
    }
    
    if(entry->HasKey("camera")){
        BXON::Array * cameras = entry->GetArray("camera");
        
        for(unsigned int i = 0;i<cameras->GetSize() ;i++){
            Camera *cam = new Camera(m_Device);
            BXON::Map * entry = cameras->GetMap(i);
            
            try{
                cam->Read(entry);
                cam_map.insert(std::pair<std::string, Datablock *>(cam->GetName(), cam));
            }
            catch(Core::Exception & ex){
                delete cam;
                THROW_EXCEPTION_STACK("Unable to load compound camera resource",ex);
            }
            
            if (processor) cam = processor->HandleCamera(cam);

            m_Cameras.push_back(cam);
        }
    }
    
    if(entry->HasKey("lamp")){
        BXON::Array * lamps = entry->GetArray("lamp");
        
        for(unsigned int i = 0;i<lamps->GetSize() ;i++){
            Lamp * lamp = new Lamp(m_Device);
            BXON::Map * entry = lamps->GetMap(i);
            
            try{
                lamp->Read(entry);
                lmp_map.insert(std::pair<std::string, Datablock *>(lamp->GetName(), lamp));
            }
            catch(Core::Exception & ex){
                delete lamp;
                THROW_EXCEPTION_STACK("Unable to load compound lamp resource",ex);
            }
            
            if (processor) lamp = processor->HandleLamp(lamp);

            m_Lamps.push_back(lamp);
        }
    }
    
    if(entry->HasKey("object")){
        BXON::Array * objects = entry->GetArray("object");
        
        for(unsigned int i = 0;i<objects->GetSize() ;i++){
            Object * obj = new Object(m_Device);
            
            BXON::Map * entry = objects->GetMap(i);
            std::string name = entry->GetString("name");
            
            obj_map.insert(std::pair<std::string, Object*>(name,obj));
            
            m_Objects.push_back(obj);
        }
        
        int idx = 0;
        ListFor(Object *, m_Objects, i){
            Object * obj = (*i);
            BXON::Map * entry = objects->GetMap(idx++);
            
            try{
                obj->Parse(entry, obj_map, mod_map, cam_map, lmp_map, arm_map, cur_map);
            }
            catch(Core::Exception & ex){
                THROW_EXCEPTION_STACK("Unable to load compound object resource",ex);
            }
            
            if(obj->GetData())
            {
                switch(obj->GetData()->GetType())
                {
                    case DATABLOCK_CAMERA:
                        ((Camera*)obj->GetData())->SetObject(obj);
                        break;
                    case DATABLOCK_LAMP:
                        ((Lamp*)obj->GetData())->SetObject(obj);
                        break;
                    default:
                        break;
                };
                
            }

            if (processor) 
                processor->HandleObject(obj);
        }
        
    }
    
    // Order objects to move the transparent ones to last in render onder.
    std::list<Object*> tObjects;
    ListFor(Object*, m_Objects, i) {
        if ((*i)->GetData() != NULL && (*i)->GetData()->GetType() == DATABLOCK_MODEL) {
            Model * m = dynamic_cast<Scene::Model*>((*i)->GetData());
            std::vector<Scene::Material*> mats = m->GetMaterials();
            for (int j = 0; j < mats.size(); j++)
                if (mats[j]->GetAlpha() < 1.0 || mats[j]->GetFlag(MATERIAL_ALPHA_BLENDING)){
                    tObjects.push_back((*i));
                    break;
                }
        }
    }

    if (tObjects.size() > 0) {
        tObjects.sort(objNameSort);

        ListFor(Object*, tObjects, i) {
            m_Objects.remove((*i));
            m_Objects.push_back((*i));
        }
    }

    return true;
}

#endif

bool Compound::Read(Core::DataReader *f)
{
    // Read file header.
    char FileID[4] = "";
    f->Read(FileID,sizeof(char)*4);
    
    if(!(FileID[0] == 'N'&& FileID[1] == 'C' && FileID[2] == 'C' && FileID[3] == 'F')){
        THROW_EXCEPTION("Unable to open compound file due to an incorrect file tag");
    }
    
    // Read file version.
    short FileVersion = 0;
    f->Read(&FileVersion,sizeof(short));
    
    if(FileVersion != 170){
        THROW_EXCEPTION("Unable to open compound file due to an incorrect file version");
    }
    
    unsigned int objects=0;
    unsigned int models=0;
    unsigned int materials=0;
    unsigned int textures=0;
    unsigned int cameras=0;
    unsigned int curves=0;
    unsigned int lamps=0;
    unsigned int armatures=0;
    
    f->Read(&textures,sizeof(unsigned int));
    f->Read(&materials,sizeof(unsigned int));
    f->Read(&models,sizeof(unsigned int));
    f->Read(&cameras,sizeof(unsigned int));
    f->Read(&lamps,sizeof(unsigned int));
    f->Read(&armatures,sizeof(unsigned int));
    f->Read(&curves,sizeof(unsigned int));
    f->Read(&objects,sizeof(unsigned int));
       
    // Load textures.
    std::vector<Texture *> tex_vec;
    tex_vec.reserve(textures);
    
    for(unsigned int i = 0;i<textures ;i++){
        Texture *tex = new Texture(m_Device);
        
        try{
            tex->Read(f);
        }
        catch(Core::Exception & ex){
            delete tex;
            THROW_EXCEPTION_STACK("Unable to load compound texture resource",ex);
        }
        
        m_Textures.push_back(tex);
        tex_vec.push_back(tex);
    }
        
    // Load materials.
    std::vector<Material *> mat_vec;
    mat_vec.reserve(materials);
    
    for(unsigned int i = 0;i<materials ;i++){
        Material *mat = new Material(m_Device);
        
        try{
            mat->Read(f,&tex_vec);
        }
        catch(Core::Exception & ex){
            delete mat;
            THROW_EXCEPTION_STACK("Unable to load material texture resource",ex);
        }
        
        m_Materials.push_back(mat);
        mat_vec.push_back(mat);
    }
        
    // Load models.
    std::vector<Model *> mod_vec;
    mod_vec.reserve(models);
    
    for(unsigned int i = 0;i<models ;i++){
        Model *mod = new Model(m_Device);
        
        try{
            mod->Read(f,&mat_vec);
        }
        catch(Core::Exception & ex){
            delete mod;
            THROW_EXCEPTION_STACK("Unable to load compound model resource",ex);
        }
        
        m_Models.push_back(mod);
        mod_vec.push_back(mod);
    }
        
    // Load cameras.
    std::vector<Camera*> cam_vec;
    cam_vec.reserve(cameras);
    
    for(unsigned int i = 0;i<cameras ;i++){
        Camera *cam = new Camera(m_Device);
        
        try{
            cam->Read(f);
        }
        catch(Core::Exception & ex){
            delete cam;
            THROW_EXCEPTION_STACK("Unable to load compound camera resource",ex);
        }
        
        m_Cameras.push_back(cam);
        cam_vec.push_back(cam);
    }
        
    // Load lamps.
    std::vector<Lamp*> lmp_vec;
    lmp_vec.reserve(lamps);
    
    for(unsigned int i = 0;i<lamps ;i++){
        Lamp *lmp = new Lamp(m_Device);
        
        try{
            lmp->Read(f);
        }
        catch(Core::Exception & ex){
            delete lmp;
            THROW_EXCEPTION_STACK("Unable to load compound lamp resource",ex);
        }
        
        m_Lamps.push_back(lmp);
        lmp_vec.push_back(lmp);
    }
    
    // Load armature
    std::vector<Armature*> arm_vec;
    arm_vec.reserve(armatures);
    
    for(unsigned int i = 0;i<armatures;i++)	{
        Armature *arm = new Armature(m_Device);
        
        try{
            arm->Read(f);
        }
        catch(Core::Exception & ex){
            delete arm;
            THROW_EXCEPTION_STACK("Unable to load compound armature resource",ex);
        }
        
        m_Armatures.push_back(arm);
        arm_vec.push_back(arm);
    }
    
    // Load objects.
    std::vector<Object*> obj_vec;
    obj_vec.reserve(objects);
    
    for(unsigned int i = 0;i < objects;i++){
        Object *obj = new Object(m_Device);
        obj_vec.push_back(obj);
        m_Objects.push_back(obj);
    }
    
    for(unsigned int i = 0;i < objects;i++){
        
        try{
            obj_vec[i]->Read(f,&obj_vec,
                             (std::vector<Datablock*>*)&mod_vec,
                             (std::vector<Datablock*>*)&cam_vec,
                             (std::vector<Datablock*>*)&lmp_vec,
                             (std::vector<Datablock*>*)&arm_vec);
        }
        catch(Core::Exception & ex){
            THROW_EXCEPTION_STACK("Unable to load compound object resource",ex);
        }
        
        if(obj_vec[i]->GetData())
        {
            switch(obj_vec[i]->GetData()->GetType())
            {
                case DATABLOCK_CAMERA:
                    ((Camera*)obj_vec[i]->GetData())->SetObject(obj_vec[i]);
                    break;
                case DATABLOCK_LAMP:
                    ((Lamp*)obj_vec[i]->GetData())->SetObject(obj_vec[i]);
                    break;
                default:
                    break;
            };
            
        }
        
    }
    
    return true;
}


Datablock *Compound::GetDatablock(DatablockType type, std::string name)
{
    switch(type)
    {
        case DATABLOCK_CURVE:
            ListFor(Curve *, m_Curves, i)
                if ((*i)->GetName() == name)
                    return (*i);
            return NULL;
        case DATABLOCK_CAMERA:
            ListFor(Camera *,m_Cameras,i)
            if((*i)->GetName() == name)
                return (*i);
            return NULL;
        case DATABLOCK_OBJECT:
            ListFor(Object *,m_Objects,i)
            if((*i)->GetName() == name)
                return (*i);
            return NULL;
        case DATABLOCK_MATERIAL:
            ListFor(Material *,m_Materials,i)
            if((*i)->GetName() == name)
                return (*i);
            return NULL;
        case DATABLOCK_MODEL:
            ListFor(Model *,m_Models,i)
            if((*i)->GetName() == name)
                return (*i);
            return NULL;
        case DATABLOCK_TEXTURE:
            ListFor(Texture *,m_Textures,i)
            if((*i)->GetName() == name)
                return (*i);
            return NULL;
        case DATABLOCK_LAMP:
            ListFor(Lamp *,m_Lamps,i)
            if((*i)->GetName() == name)
                return (*i);
            return NULL;
        case DATABLOCK_ARMATURE:
            ListFor(Armature *,m_Armatures,i)
            if((*i)->GetName() == name)
                return (*i);
            return NULL;
        default:
            break;
    }
    
    return NULL;
}




Compound_Base::Compound_Base(Graph::Device *dev) : Compound(dev){
    
}

Compound_Base::~Compound_Base()
{
    m_MObjects.clear();
}

void Compound_Base::Load(const std::string & filename, Processor * processor)
{
    Core::DataReader * f = Core::DataReader::Open(filename);
    
    if(!f){
        THROW_EXCEPTION("Unable to open file \"" + filename + "\"");
    }
    
    if (Core::FindExtension(filename) == "bxon") {
        BXON::ReaderContext * frCtx = NULL; 
        BXON::Object * obj = NULL;
        BXON::Map * map = NULL;
        try {
            frCtx = new BXON::ReaderContext(f);
            obj = BXON::Object::Parse(dynamic_cast<BXON::Context*>(frCtx));
            if (processor) obj = processor->HandleData(obj);
            map = dynamic_cast<BXON::Map*>(obj);
            ReadBXON(map,processor);
            if (processor) processor->HandleFinish(map, this);
        }
        catch (const Core::Exception & e) {
            SafeDelete(obj);
            SafeDelete(frCtx);
            delete f;
            throw e;
        }

        SafeDelete(obj);
        SafeDelete(frCtx);
    }
    else {
        try {
            Read(f);
        }
        catch (Core::Exception & ex)
        {
            delete f;
            throw ex;
        }
    }

    delete f;
    
    Math::BoundBox tmpBB;
    ListFor(Object*,m_Objects,i){
        if( (*i)->GetData() && (*i)->GetData()->GetType() == DATABLOCK_MODEL){
            Model * m = dynamic_cast<Model*>((*i)->GetData());
            Math::BoundBox bb = m->GetBoundBox() * (*i)->GetMatrix();
            tmpBB.Insert(bb.GetMax());
            tmpBB.Insert(bb.GetMin());
            m_MObjects.push_back(*i);
        }
    }
    m_Boundbox = tmpBB;
}

Math::BoundBox Compound_Base::GetBoundBox(){
    return m_Boundbox;
}

void Compound_Base::Load(BXON::Map * entry, Processor * processor){
    ReadBXON(entry);
    
    Math::BoundBox tmpBB;
    ListFor(Object*,m_Objects,i){
        if( (*i)->GetData() && (*i)->GetData()->GetType() == DATABLOCK_MODEL){
            Model * m = dynamic_cast<Model*>((*i)->GetData());
            Math::BoundBox bb = m->GetBoundBox() * (*i)->GetMatrix();
            tmpBB.Insert(bb.GetMax());
            tmpBB.Insert(bb.GetMin());
            m_MObjects.push_back(*i);
        }
    }

    m_Boundbox = tmpBB;

    m_CamerasWithKeyframes = fetchCamerasWithKeyframes(entry, this);
}

RenderStatistics Compound_Base::Render(const RenderControl & control) {
	int rendered = 0;

	Math::Frustum * fr = control.GetFrustum();
	Material *overlap = control.GetMaterial();
	int layer_mask = control.GetLayers();
	MaterialPass pass = control.GetPass();

	ListFor(Object*, m_MObjects, i)
	{
		Object * o = (*i);
		Model * m = (Model*)o->GetData();

		if (layer_mask != LAYER_ALL && ((1 << (*i)->GetLayer()) & layer_mask) == 0)
			continue;

		if (!o->GetVisible())
			continue;

		if (fr) {
			m_Device->PushMatrix();

			Math::BoundBox bb = m->GetBoundBox()* o->GetMatrix();

			if (fr->CheckCube(bb.GetMax(), bb.GetMin())) {
				o->Bind();
				m->Render(pass,overlap);
				rendered++;
			}

			m_Device->PopMatrix();

		}
		else {
			m_Device->PushMatrix();
			o->Bind();
			m->Render(pass,overlap);
			m_Device->PopMatrix();
			rendered++;
		}
	}

	return RenderStatistics(m_MObjects.size(), rendered);
}

RenderStatistics Compound_Base::Render(Math::Frustum * fr, Material *overlap, int layer_mask)
{
    int rendered = 0;

    ListFor(Object*,m_MObjects,i)
    {
        Object * o = (*i);
        Model * m = (Model*)o->GetData();

        if (layer_mask != LAYER_ALL && ((1 << (*i)->GetLayer()) & layer_mask) == 0)
            continue;

        if (!o->GetVisible())
            continue;

        if(fr){
            m_Device->PushMatrix();
            
            Math::BoundBox bb = m->GetBoundBox()* o->GetMatrix() ;
            
            if(fr->CheckCube(bb.GetMax(),bb.GetMin()) ){
                o->Bind();
                m->Render(MATERIAL_PASS_DEFAULT,overlap);
                rendered++;
            }
            
            m_Device->PopMatrix();
          
        }
        else{	
            m_Device->PushMatrix();
            o->Bind();
            m->Render(MATERIAL_PASS_DEFAULT,overlap);
            m_Device->PopMatrix();
            rendered++;
        }
    }

    return RenderStatistics(m_MObjects.size() ,rendered);
}


Object * Compound_Base::GetObject(const std::string & name) {
    ListFor(Object*, m_Objects, i) 
        if ((*i)->GetName() == name)
            return (*i);
    return NULL;
}

int Compound_Base::GetObjectsWithLayer(std::vector<Object*> * datablocks, int layer_mask) {
    datablocks->reserve(m_Objects.size());
    ListFor(Object*, m_Objects, i)
    {
        if (((1 << (*i)->GetLayer()) & layer_mask) != 0)
            datablocks->push_back((*i));
    }
    return datablocks->size();
}

int Compound_Base::GetObjectsWithLayer(std::vector<Object*> * datablocks, DatablockType type, int layer_mask) {
    datablocks->reserve(m_Objects.size());
    ListFor(Object*, m_Objects, i)
    {
        if ((*i)->GetData() != NULL && (*i)->GetData()->GetType() == type && ((1 << (*i)->GetLayer()) & layer_mask) != 0)
            datablocks->push_back((*i));
    }
    return datablocks->size();
}

int Compound_Base::GetEmptyObjects(std::vector<Object*> * objects, int layer_mask) {
    objects->reserve(m_Objects.size());
    ListFor(Object*, m_Objects, i)
    {
        if ((*i)->GetData() == NULL)
            objects->push_back((*i));
    }
    return objects->size();
}

Material * Compound_Base::GetMaterial(const std::string & name) {
    ListFor(Material*, m_Materials, i)
        if ((*i)->GetName() == name)
            return (*i);
    return NULL;
}

Model * Compound_Base::GetModel(const std::string & name) {
    ListFor(Model*, m_Models, i)
        if ((*i)->GetName() == name)
            return (*i);
    return NULL;
}


Texture * Compound_Base::GetTexture(const std::string & name) {
    ListFor(Texture*, m_Textures, i)
        if ((*i)->GetName() == name)
            return (*i);
    return NULL;
}

Armature * Compound_Base::GetArmature(const std::string & name) {
    ListFor(Armature*, m_Armatures, i)
        if ((*i)->GetName() == name)
            return (*i);
    return NULL;
}

Curve * Compound_Base::GetCurve(const std::string & name) {
    ListFor(Curve*, m_Curves, i)
        if ((*i)->GetName() == name)
            return (*i);
    return NULL;
}

Camera * Compound_Base::GetCamera(const std::string & name) {
    ListFor(Camera*, m_Cameras, i)
        if ((*i)->GetName() == name)
            return (*i);
    return NULL;
}


std::vector<std ::pair<float, Scene::Object*> > Compound_Base::fetchCamerasWithKeyframes(BXON::Map * map, Compound_Base * compound){
    std::vector< std::pair<float, Object*> > ret;
    if (map->HasKey("tl_markers")) {
        BXON::Array * markers = map->GetArray("tl_markers");

        std::vector<int> orderedFrames;
        orderedFrames.reserve(markers->GetSize());

        std::map<int, std::string> camKeyMap;

        for (size_t i = 0; i < markers->GetSize(); i++) {
            BXON::Map * entry = markers->GetMap(i);
            int frameValue = entry->GetInteger("frame");
            if (entry->HasKey("camera")) {
                std::string camName = entry->GetString("camera");
                orderedFrames.push_back(frameValue);
                camKeyMap.insert(std::pair<int, std::string>(frameValue, camName));
            }
        }

        std::sort(orderedFrames.begin(), orderedFrames.end());

        ret.reserve(orderedFrames.size());

        for (int k = 0; k < orderedFrames.size(); k++) {
            int key = orderedFrames[k];
            if (camKeyMap.find(key) != camKeyMap.end()) {
                std::string name = camKeyMap.find(key)->second;
                Scene::Object * camObj = compound->GetObject(name);
                if (camObj != NULL)
                    ret.push_back(std::pair<float, Object*>((float)key, camObj));
            }
        }
    }

    return ret;
}

Scene::Camera * Compound_Base::GetCameraForKeyframe(float time) {
    std::vector< std::pair<float, Scene::Object*> > vec = m_CamerasWithKeyframes;

    if (vec.size() == 0)
        return NULL;

    if (time <= vec[0].first)
        return dynamic_cast<Camera*>(vec[0].second->GetData());
    else if (time >= vec[vec.size() - 1].first)
        return dynamic_cast<Camera*>(vec[vec.size() - 1].second);

    int left = 0;
    int right = vec.size() - 1;
    int half;

    while (left <= right) {
        half = (left + right) / 2;
        if (vec[half].first == time)
            return dynamic_cast<Camera*>(vec[half].second);
        else if (time < vec[half].first)
            right = half - 1;
        else
            left = half + 1;
    }

    if (half >= 0 && half <= vec.size() - 1)
        return dynamic_cast<Camera*>(vec[half].second);

    return NULL;
}

int Compound_Base::GetAllMaterials(std::vector<Material*> * materials) {
    materials->reserve(m_Materials.size());
    ListFor(Material*, m_Materials, i) {
        materials->push_back((*i));
    }
    return materials->size();
}

int Compound_Base::GetAllTextures(std::vector<Texture*> * textures) {
    textures->reserve(m_Textures.size());
    ListFor(Texture*, m_Textures, i) {
        textures->push_back((*i));
    }
    return textures->size();
}

_SCENE_END
