
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo selector frontend
*/

#include "nckDemo_Selector.h"
#include "nckVec4.h"

class DemoDetails{
public:
	enum Platform{
		ALL,
		LINUX,
		WINDOWS,
		MACOS,
		ANDROID,
		IOS,
		RASPBERRYPI,		
	};

	DemoDetails();
	~DemoDetails();

	void SetKeywords(const std::vector<std::string> & keywords);
	void AddKeyword(const std::string & keyword);
	void AddPlatform(Platform p);
	void SetDescription(const std::string & description);
	bool SupportsCurrentPlatform();
	void SetScreenshotFilename(const std::string & filename);

	std::vector<Platform> GetPlatforms() const;
	std::string GetDescription() const;
	std::vector<std::string> GetKeywords() const;
	std::string GetScreenshotFilename() const;
private:
	std::vector<std::string> m_Keywords;
	std::vector<Platform> m_Platforms;
	std::string m_Description;
	std::string m_ScreenshotFilename;
};

DemoDetails::DemoDetails(){

}

DemoDetails::~DemoDetails(){

}

std::string DemoDetails::GetDescription() const{
	return m_Description;
}

std::vector<std::string> DemoDetails::GetKeywords() const{
	return m_Keywords;
}

std::vector<DemoDetails::Platform> DemoDetails::GetPlatforms() const{
	return m_Platforms;
}

void DemoDetails::SetKeywords(const std::vector<std::string> & keywords){
	m_Keywords = keywords;
}

void DemoDetails::SetDescription(const std::string & description){
	m_Description = description;
}

void DemoDetails::AddKeyword(const std::string & keyword){
	m_Keywords.push_back(keyword);
}

void DemoDetails::SetScreenshotFilename(const std::string & filename){
	m_ScreenshotFilename = filename;
}

std::string DemoDetails::GetScreenshotFilename() const{
	return m_ScreenshotFilename;
}


bool DemoDetails::SupportsCurrentPlatform(){
	if(m_Platforms.size() == 0)
		return true;

	for(uint32_t i = 0;i<m_Platforms.size();i++){
		Platform p = m_Platforms[i];
		if(p == ALL)
			return true;
#if defined(NCK_WINDOWS)
		if(p == WINDOWS)
			return true;
#elif defined(NCK_LINUX)
		if(p == LINUX)
			return true;
#elif defined(NCK_MACOSX)
		if(p == MACOS)
			return true;
#endif
	}
	return false;
}




DemoSelector::DemoSelector(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;

	m_FontTexture = NULL;
	m_FontMap = NULL;
	m_Callback = NULL;
	m_BlurryBg = NULL;
	m_IconNck = NULL;

	m_ScrollPosition = 0;
	m_ScrollVelocity = 0;
	m_ScrollOffset = 0;
	m_ActiveDemoId = 0;
	m_TotalDeslocation = 0;

	DemoDetails * d1 = new DemoDetails();
	d1->SetDescription("Textured 3D model loading and rendering with a shader");
	d1->AddKeyword("Basic");
	d1->AddKeyword("Model");
	d1->AddKeyword("Shader");

	DemoDetails * d2 = new DemoDetails();
	d2->SetDescription("Graphical user interface creation and rendering");
	d2->AddKeyword("Basic");
	d2->AddKeyword("GUI");

	DemoDetails * d3 = new DemoDetails();
	d3->SetDescription("Http server creation and interaction");
	d3->AddKeyword("Basic");
	d3->AddKeyword("HTTP");
	d3->AddKeyword("Server");
	
	DemoDetails * d4 = new DemoDetails();
	d4->SetDescription("OGG audio playback");
	d4->AddKeyword("Basic");
	d4->AddKeyword("Audio");
	d4->AddKeyword("Playback");
	d4->AddKeyword("OGG");

	DemoDetails * d5 = new DemoDetails();
	d5->SetDescription("Marching Cubes Rendering");
	d5->AddKeyword("Basic");
	d5->AddKeyword("Marching Cubes");

	DemoDetails * d6 = new DemoDetails();
	d6->SetDescription("3D Texture usage");
	d6->AddKeyword("Basic");
	d6->AddKeyword("3D Texture");

	DemoDetails * d7 = new DemoDetails();
	d7->SetDescription("Webcam video capture");
	d7->AddKeyword("Advanced");
	d7->AddKeyword("Webcam");
	d7->AddKeyword("v4l");
	d7->AddKeyword("Linux");

	DemoDetails * d8 = new DemoDetails();
	d8->SetDescription("Rendering graphics to Http server");
	d8->AddKeyword("Advanced");
	d8->AddKeyword("Rendering");
	d8->AddKeyword("HTTP");
	d8->AddKeyword("Server");

	DemoDetails * d9 = new DemoDetails();
	d9->SetDescription("Arduino serial port communication");
	d9->AddKeyword("Advanced");
	d9->AddKeyword("Serial Port");
	d9->AddKeyword("IO");
	d9->AddKeyword("Arduino");

	DemoDetails * d10 = new DemoDetails();
	d10->SetDescription("Simple 2D physics simulation");
	d10->AddKeyword("Advanced");
	d10->AddKeyword("Physics");
	d10->AddKeyword("Simulation");
	d10->AddKeyword("WIP");

    DemoDetails * d11 = new DemoDetails();
    d11->SetDescription("BXON Models & Animation");
    d11->AddKeyword("Model");
    d11->AddKeyword("Animation");
    d11->AddKeyword("BXON");

    DemoDetails * d12 = new DemoDetails();
    d12->SetDescription("Multi Texture");
    d12->AddKeyword("Textures");
    
    DemoDetails * d13 = new DemoDetails();
    d13->SetDescription("BumpMapping");
    d13->AddKeyword("Model");
    d13->AddKeyword("Textures");
    d13->AddKeyword("Shaders");
    
    DemoDetails * d14 = new DemoDetails();
    d14->SetDescription("Shadows");
    d14->AddKeyword("Model");
    d14->AddKeyword("Textures");
    d14->AddKeyword("Shaders");

	m_DemoDetails.push_back(d1);
	m_DemoDetails.push_back(d2);
	m_DemoDetails.push_back(d3);
	m_DemoDetails.push_back(d4);
	m_DemoDetails.push_back(d5);
	m_DemoDetails.push_back(d6);
	m_DemoDetails.push_back(d7);
	m_DemoDetails.push_back(d8);
	m_DemoDetails.push_back(d9);
    m_DemoDetails.push_back(d10);  
	m_DemoDetails.push_back(d11);
    m_DemoDetails.push_back(d12);
    m_DemoDetails.push_back(d13);
    m_DemoDetails.push_back(d14);
}

DemoSelector::~DemoSelector(){
	while(m_DemoDetails.size()>0){
		delete m_DemoDetails.back();
		m_DemoDetails.pop_back();
	}
	SafeDelete(m_FontTexture);
	SafeDelete(m_FontMap);
	SafeDelete(m_BlurryBg);
	SafeDelete(m_IconNck);
}

void DemoSelector::Load(){
	dev->Enable(Graph::STATE_BLEND);
	dev->Enable(Graph::STATE_DEPTH_TEST);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA,Graph::BLEND_INV_SRC_ALPHA);
	dev->ClearColor(0.5,0.5,0.5,1.0);

	dev->Enable(Graph::STATE_BLEND);
	dev->Disable(Graph::STATE_DEPTH_TEST);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA,Graph::BLEND_INV_SRC_ALPHA);
	
	m_FontTexture = dynamic_cast<Graph::Texture2D*>(dev->LoadTexture("texture://tex2d_font_ubuntu.png"));
	m_FontTexture->SetFilter(Graph::FILTER_MAGNIFICATION,Graph::FILTER_LINEAR);
	m_FontTexture->SetFilter(Graph::FILTER_MINIFICATION,Graph::FILTER_LINEAR);
	m_FontTexture->SetFilter(Graph::FILTER_MIPMAPPING,Graph::FILTER_LINEAR);

	m_FontMap = new Gui::FontMap(dev);
	m_FontMap->Load("script://font_ubuntu.txt");

	m_BlurryBg = dynamic_cast<Graph::Texture2D*>(dev->LoadTexture("texture://tex2d_blurrybg.png"));
	m_IconNck = dynamic_cast<Graph::Texture2D*>(dev->LoadTexture("texture://tex2d_icon_nck.png"));
}

void DemoSelector::SetCallback(DemoSelector_Callback * callback){
	m_Callback = callback;
}

void DemoSelector::RenderSquare(float x,float y,float w, float h, const Math::Color4ub & col1){
	dev->Color(col1.GetR(),col1.GetG(),col1.GetB(),col1.GetA());
	dev->Begin(Graph::PRIMITIVE_QUADS);
	dev->TexCoord(0,0);
	dev->Vertex(x,y);
	dev->TexCoord(0,1);
	dev->Vertex(x,y+h);
	dev->TexCoord(1,1);
	dev->Vertex(x+w,y+h);
	dev->TexCoord(1,0);
	dev->Vertex(x+w,y);
	dev->End();
}

void DemoSelector::Render(float dt){
    dev->ClearColor(0.5,0.5,0.5,1);
	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());
    dev->Clear();
    
	dev->Disable(Graph::STATE_DEPTH_TEST);

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
	dev->Ortho2D((float)wnd->GetWidth(),(float)wnd->GetHeight());

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();

	m_BlurryBg->Enable();
	RenderSquare(0,0,(float)wnd->GetWidth(),(float)wnd->GetHeight(),Math::Color4ub());
	m_BlurryBg->Disable();

	Core::Point p = wnd->GetMousePosition();

	m_ScrollPosition += m_ScrollOffset + m_ScrollVelocity;

	m_ScrollVelocity *= 0.95f;

	const float cellHeight = 50.0f;
	const float cellSplitHeight = 1.0f;
	const float margin = 10.0f;
	const float demoSectionWidth = 110.0f;

	if(m_ScrollPosition < -(cellHeight+cellSplitHeight)*m_DemoDetails.size() + (float)wnd->GetHeight()-margin*2){
		m_ScrollPosition = -(cellHeight+cellSplitHeight)*m_DemoDetails.size() + (float)wnd->GetHeight()-margin*2;
	}

	if(m_ScrollPosition > 0){
		m_ScrollPosition = 0;
	}

	for(uint32_t i = 0;i<m_DemoDetails.size();i++)
	{
		dev->PushMatrix();
		dev->Translate(margin,m_ScrollPosition+i*(cellHeight+cellSplitHeight)+margin,0);

		Math::Color4ub bgColor = Math::Color4ub(20,20,20,150);

		if(p.GetY() > (m_ScrollPosition+i*(cellHeight+cellSplitHeight)+margin) && 
			p.GetY() < (m_ScrollPosition+i*(cellHeight+cellSplitHeight)+margin+cellHeight) && 
			p.GetX() > margin && 
			p.GetX() < wnd->GetWidth() - margin)
		{
			m_ActiveDemoId = i;
			bgColor = Math::Color4ub(50,50,50,150);
		}
		
		float demoTitleX = wnd->GetWidth()-margin*2-demoSectionWidth;
		RenderSquare(0,0,cellHeight,cellHeight,bgColor);
		RenderSquare(demoTitleX,0,demoSectionWidth,cellHeight,bgColor);
		RenderSquare(cellHeight+cellSplitHeight,0,wnd->GetWidth()-margin*2-(demoSectionWidth+cellSplitHeight*2+cellHeight),cellHeight,bgColor);

		std::string text = m_DemoDetails[i]->GetDescription();

		m_FontTexture->Enable();

		dev->Color(125,125,125);
		m_FontMap->Draw(cellHeight+margin,18,16,text);

		dev->Color(64,119,47);
		m_FontMap->Draw(demoTitleX+10,25,32,"Demo "+Math::IntToString(i+1),true,Gui::FONT_ALIGNMENT_LEFT);

		std::vector<std::string> keywords = m_DemoDetails[i]->GetKeywords();
		for(int j = 0,x = 0;j<(int)keywords.size();j++){
			dev->Color(149,53,19);
			m_FontMap->Draw(cellHeight+margin+x,cellHeight/2+16/2+3,16,keywords[j],true);
			x += (int)m_FontMap->GetLength(16,keywords[j],true)+10;
		}
		m_FontTexture->Disable();

		m_IconNck->Enable();
		RenderSquare(9,9,32,32,Math::Color4ub());
		m_IconNck->Disable();

		dev->PopMatrix();
	}

	dev->PresentAll();
}

void DemoSelector::UpdateWndEvents()
{
    static bool didFirstPress = true;
    
	if(wnd->GetMouseButtonStatus(Core::BUTTON_MOUSE_LEFT) == Core::BUTTON_STATUS_DOWN)
	{
        if(didFirstPress){
            m_LastCursorPosition = wnd->GetMousePosition();
            didFirstPress = false;
        }
	}
	else
    {
        didFirstPress = true;
        if(m_LastCursorPosition.GetY() > 0)
        {
            m_LastCursorPosition = Core::Point(-1,-1);
            m_ScrollOffset = 0;
            m_ScrollVelocity = 0;

            float weight = 1.0;
        
            while(m_ScrollVelocitySamples.size() > 0)
            {
                m_ScrollVelocity += m_ScrollVelocitySamples.back() * weight;
                weight *= 0.5;
                m_ScrollVelocitySamples.pop_back();
            }
            
            m_ScrollVelocitySamples.clear();
            
            if(m_TotalDeslocation == 0){
                if(m_Callback != NULL){
					wnd->SetTitle("Demo #" + Math::IntToString(m_ActiveDemoId) + " - " + m_DemoDetails[m_ActiveDemoId]->GetDescription());
                    m_Callback->SelectDemo(m_ActiveDemoId+1);
				}
            }

            m_TotalDeslocation = 0;
        }
    }
    
	if(m_LastCursorPosition.GetY() > 0)
	{
		Core::Point pos = wnd->GetMousePosition();
        
		m_ScrollOffset = pos.GetY() - m_LastCursorPosition.GetY();
		m_TotalDeslocation += ABS(m_ScrollOffset);
		m_ScrollVelocitySamples.push_back(m_ScrollOffset);

		if(m_ScrollVelocitySamples.size()>5)
			m_ScrollVelocitySamples.pop_back();

		m_LastCursorPosition = pos;
	}
	else
	{
		m_ScrollOffset = 0;
	}
	
}


Demo * CreateDemo_Selector(Core::Window * wnd, Graph::Device * dev){
	return new DemoSelector(wnd,dev);
}
