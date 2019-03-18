
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Texture_Diff.h"

std::list<Math::Vec2> points;

#include <iostream>
#include <algorithm>

void Detect_Difference(Core::Image * m_Image_Normal, Core::Image * m_Image_Laser, std::list<Math::Vec2> * points);

Demo_TextureDiff::Demo_TextureDiff(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
	m_Normal = NULL;
	m_Laser = NULL;
}

Demo_TextureDiff::~Demo_TextureDiff(){
  
}

void Demo_TextureDiff::Load(){
    // Enable z test
    dev->Enable(Graph::STATE_DEPTH_TEST);

    // Set correct states for alpha blending.
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    m_Normal = dynamic_cast<Graph::Texture2D*>(dev->LoadTexture("texture://normal.png"));
    m_Laser = dynamic_cast<Graph::Texture2D*>(dev->LoadTexture("texture://laser.png"));
    
	dev->ClearColor(0, 0, 0);

	Core::Image * imgNormal = Core::Image::Create(m_Normal->GetWidth(), m_Normal->GetHeight(), Core::PIXEL_FORMAT_RGBA_8B);
	Core::Image * imgLaser = Core::Image::Create(m_Laser->GetWidth(), m_Laser->GetHeight(), Core::PIXEL_FORMAT_RGBA_8B);

	uint8_t * dNormal = (uint8_t*)m_Normal->Lock();
	uint8_t * dLaser = (uint8_t*)m_Laser->Lock();

	memcpy(imgNormal->GetData(), dNormal, imgNormal->GetWidth() * imgNormal->GetHeight() *3 );
	memcpy(imgLaser->GetData(), dLaser, imgLaser->GetWidth() * imgLaser->GetHeight() * 3);

	m_Normal->Unlock();
	m_Laser->Unlock();

	Detect_Difference(imgNormal, imgLaser, &points);
}

void Demo_TextureDiff::Render(float dt){
	// Clear graphics rendering buffer.
	dev->Clear();

	// Define the rendering area in window space.
	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

	// Set projection matrix.
	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
    dev->Ortho2D(wnd->GetWidth(),wnd->GetHeight());
    
    // Set the view (camera) matrix.
    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();

	// Set the model matrix.
	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();
   
    dev->Disable(Graph::STATE_DEPTH_TEST);
    dev->Disable(Graph::STATE_ZBUFFER_WRITE);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);
    dev->Color(255, 255, 255);

	float aspect = m_Normal->GetWidth() / (float)m_Normal->GetHeight();

	float size = 480;

	m_Normal->Enable();
    RenderSquare2D(0, 0, size, size / aspect);
	m_Normal->Disable();

    m_Laser->Enable();
    RenderSquare2D(size, 0, size, size / aspect);
	m_Laser->Disable();

	dev->PointSprite(Graph::PointSpriteProperty::POINT_SIZE, 4);

	float scale = m_Normal->GetWidth() / size;

	dev->Color(255, 0, 255);
	ListFor(Math::Vec2, points, i) {
		dev->Begin(Graph::PRIMITIVE_POINTS);
		dev->Vertex(i->GetX() / scale+ size, i->GetY() / scale);
		dev->End();
	}
	

	/* m_RGBTex->Enable();
    RenderSquare2D(512, 0, 256, 256);
    m_RGBTex->Disable();

    m_RGBATex->Enable();
    RenderSquare2D(512, 256, 256, 256);
    m_RGBATex->Disable();

    m_R32Tex->Enable();
    RenderSquare2D(512+256, 0, 256, 256);
    m_R32Tex->Disable();

    m_R16Tex->Enable();
    RenderSquare2D(512+256, 256, 256, 256);
    m_R16Tex->Disable();

    m_RGBA32Tex->Enable();
    RenderSquare2D(512, 512, 256, 256);
    m_RGBA32Tex->Disable();

    m_RGBA16Tex->Enable();
    RenderSquare2D(512+256, 512, 256, 256);
    m_RGBA16Tex->Disable();*/

    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_ZBUFFER_WRITE);


	// Finish rendering and present the graphics.
	dev->PresentAll();
}

void Demo_TextureDiff::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_TextureDiff::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Basic");
    ret.push_back("Rendering");
    ret.push_back("Textures");
    return ret;
}

std::string Demo_TextureDiff::GetDescription() {
    return "2D Textures";
}

Demo * CreateDemo_Texture_Diff(Core::Window * wnd, Graph::Device * dev){
	return new Demo_TextureDiff(wnd,dev);
}

#define BYTES_PER_PIXEL 3


Math::Color4ub SampleColor(Core::Image * img, int x, int y) {
	uint8_t * pointer = img->GetData();
	if (x < 0) x = 0;
	if (x >= img->GetWidth()) x = img->GetWidth() - 1;
	if (y < 0) y = 0;
	if (y >= img->GetHeight()) y = img->GetHeight() - 1;

	if (img->GetFormat() == Core::PIXEL_FORMAT_RGB_8B) {
		uint8_t * ref = pointer + (img->GetWidth() * y + x) * 3;
		return Math::Color4ub(ref[0], ref[1], ref[2]);
	}
	else if (img->GetFormat() == Core::PIXEL_FORMAT_RGBA_8B) {
		uint8_t * ref = pointer + (img->GetWidth() * y + x) * 4;
		return Math::Color4ub(ref[0], ref[1], ref[2], ref[4]);
	}
	return Math::Color4ub(0, 0, 0);
}

Math::Color4ub SampleColor3x3(Core::Image * img, int x, int y) {
	Math::Color4f ret;

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
		Math::Color4ub col = SampleColor(img, x, y);
		ret += Math::Color4f(col.GetR(), col.GetG(), col.GetB());
		}
	}

	ret = ret * (1.0 / 9.0);

	return Math::Color4ub(ret.GetR(), ret.GetG(), ret.GetB(), 1.0);
	return SampleColor(img, x, y);
}

class Point
{
private:
	int id_point, id_cluster;
	std::vector<double> values;
	int total_values;
	std::string name;

public:
	Point(int id_point, std::vector<double>& values, std::string name = "")
	{
		this->id_point = id_point;
		total_values = values.size();

		for (int i = 0; i < total_values; i++)
			this->values.push_back(values[i]);

		this->name = name;
		id_cluster = -1;
	}

	int getID()
	{
		return id_point;
	}

	void setCluster(int id_cluster)
	{
		this->id_cluster = id_cluster;
	}

	int getCluster()
	{
		return id_cluster;
	}

	double getValue(int index)
	{
		return values[index];
	}

	int getTotalValues()
	{
		return total_values;
	}

	void addValue(double value)
	{
		values.push_back(value);
	}

	std::string getName()
	{
		return name;
	}
};

class Cluster
{
private:
	int id_cluster;
	std::vector<double> central_values;
	std::vector<Point> points;

public:
	Cluster(int id_cluster, Point point)
	{
		this->id_cluster = id_cluster;

		int total_values = point.getTotalValues();

		for (int i = 0; i < total_values; i++)
			central_values.push_back(point.getValue(i));

		points.push_back(point);
	}

	void addPoint(Point point)
	{
		points.push_back(point);
	}

	bool removePoint(int id_point)
	{
		int total_points = points.size();

		for (int i = 0; i < total_points; i++)
		{
			if (points[i].getID() == id_point)
			{
				points.erase(points.begin() + i);
				return true;
			}
		}
		return false;
	}

	double getCentralValue(int index)
	{
		return central_values[index];
	}

	void setCentralValue(int index, double value)
	{
		central_values[index] = value;
	}

	Point getPoint(int index)
	{
		return points[index];
	}

	int getTotalPoints()
	{
		return points.size();
	}

	int getID()
	{
		return id_cluster;
	}
};

class KMeans
{
public:
	int K; // number of clusters
	int total_values, total_points, max_iterations;
	std::vector<Cluster> clusters;

	// return ID of nearest center (uses euclidean distance)
	int getIDNearestCenter(Point point)
	{
		double sum = 0.0, min_dist;
		int id_cluster_center = 0;

		for (int i = 0; i < total_values; i++)
		{
			sum += pow(clusters[0].getCentralValue(i) -
				point.getValue(i), 2.0);
		}

		min_dist = sqrt(sum);

		for (int i = 1; i < K; i++)
		{
			double dist;
			sum = 0.0;

			for (int j = 0; j < total_values; j++)
			{
				sum += pow(clusters[i].getCentralValue(j) -
					point.getValue(j), 2.0);
			}

			dist = sqrt(sum);

			if (dist < min_dist)
			{
				min_dist = dist;
				id_cluster_center = i;
			}
		}

		return id_cluster_center;
	}


	KMeans(int K, int total_points, int total_values, int max_iterations)
	{
		this->K = K;
		this->total_points = total_points;
		this->total_values = total_values;
		this->max_iterations = max_iterations;
	}

	void run(std::vector<Point> & points)
	{
		if (K > total_points)
			return;

		std::vector<int> prohibited_indexes;

		// choose K distinct values for the centers of the clusters
		for (int i = 0; i < K; i++)
		{
			while (true)
			{
				int index_point = rand() % total_points;

				if (find(prohibited_indexes.begin(), prohibited_indexes.end(),
					index_point) == prohibited_indexes.end())
				{
					prohibited_indexes.push_back(index_point);
					points[index_point].setCluster(i);
					Cluster cluster(i, points[index_point]);
					clusters.push_back(cluster);
					break;
				}
			}
		}

		int iter = 1;

		while (true)
		{
			bool done = true;

			// associates each point to the nearest center
			for (int i = 0; i < total_points; i++)
			{
				int id_old_cluster = points[i].getCluster();
				int id_nearest_center = getIDNearestCenter(points[i]);

				if (id_old_cluster != id_nearest_center)
				{
					if (id_old_cluster != -1)
						clusters[id_old_cluster].removePoint(points[i].getID());

					points[i].setCluster(id_nearest_center);
					clusters[id_nearest_center].addPoint(points[i]);
					done = false;
				}
			}

			// recalculating the center of each cluster
			for (int i = 0; i < K; i++)
			{
				for (int j = 0; j < total_values; j++)
				{
					int total_points_cluster = clusters[i].getTotalPoints();
					double sum = 0.0;

					if (total_points_cluster > 0)
					{
						for (int p = 0; p < total_points_cluster; p++)
							sum += clusters[i].getPoint(p).getValue(j);
						clusters[i].setCentralValue(j, sum / total_points_cluster);
					}
				}
			}

			if (done == true || iter >= max_iterations)
			{
				std::cout << "Break in iteration " << iter << "\n\n";
				break;
			}

			iter++;
		}

		// shows elements of clusters
		for (int i = 0; i < K; i++)
		{
			int total_points_cluster = clusters[i].getTotalPoints();

			std::cout << "Cluster " << clusters[i].getID() + 1 << std::endl;
			for (int j = 0; j < total_points_cluster; j++)
			{
				std::cout << "Point " << clusters[i].getPoint(j).getID() + 1 << ": ";
				for (int p = 0; p < total_values; p++)
					std::cout << clusters[i].getPoint(j).getValue(p) << " ";

				std::string point_name = clusters[i].getPoint(j).getName();

				if (point_name != "")
					std::cout << "- " << point_name;

				std::cout << std::endl;
			}

			std::cout << "Cluster values: ";

			for (int j = 0; j < total_values; j++)
				std::cout << clusters[i].getCentralValue(j) << " ";

			std::cout << "\n\n";
		}
	}
};


void Detect_Difference(Core::Image * m_Image_Normal, Core::Image * m_Image_Laser, std::list<Math::Vec2> * points) {
	
	int width = m_Image_Normal->GetWidth();
	int height = m_Image_Normal->GetHeight();

	float threshold = 70.0;
	
	int K = 1;
	int max_iterations = 20;

	//KMeans kmeans(K, total_points, total_values, max_iterations);
	//kmeans.run(points);

	

	for (int i = 0; i < height; i++)
	{
		std::vector<Math::Vec2> pointsRowSample;
		std::vector<Math::Color4ub> colorRowSample;

		std::vector<Point> kmeans_points;
		//std::vector<float> kmeans_differences;

		for (int j = 0; j < width; j++)
		{
			Math::Color4ub nColor = SampleColor3x3(m_Image_Normal, j, i);
			Math::Color4ub lColor = SampleColor3x3(m_Image_Laser, j, i);

			Math::Vec3 normalVec(nColor.GetR(), nColor.GetG(), nColor.GetB());
			Math::Vec3 laserVec(lColor.GetR(), lColor.GetG(), lColor.GetB());

			bool diffColor = (normalVec - laserVec).Length() > threshold;

			if (diffColor) {
				//std::vector<double> v;
				//v.push_back(j);
				//kmeans_points.push_back(Point(kmeans_points.size(), v));

				//points->push_back(Math::Vec2(j, i));
				pointsRowSample.push_back(Math::Vec2(j, i));
				colorRowSample.push_back(nColor);
			}
		}

		/*if (kmeans_points.size() > 0) {
			KMeans kmeans(K, kmeans_points.size(), 1, max_iterations);
			kmeans.run(kmeans_points);

			for (int jj = 0; jj < kmeans.clusters.size(); jj++) {
				points->push_back(Math::Vec2(kmeans.clusters[jj].getCentralValue(0), i));
				
			}
		}*/

		if (pointsRowSample.size() > 1) {
			Math::Vec2 sample = pointsRowSample[pointsRowSample.size() / 2];

			int xMin = pointsRowSample.size()  * 0.05;
			int xMax = pointsRowSample.size()  * 0.95;

			/*int minColorId = 0;
			int minColor = 0;

			for (int i = xMin; i <= xMax; i++) {
				Math::Color4ub c = colorRowSample[i];
				int grey = (c.GetR() + c.GetG() + c.GetB()) / 3;
				if (grey > minColor) {
					minColorId = i;
					minColor = grey;
				}
			}*/

			sample = pointsRowSample[(xMax+xMin)/2];

			int lineOff = width * ((int)sample.GetY()) * BYTES_PER_PIXEL;
			int pixelOff = ((int)sample.GetX()) * BYTES_PER_PIXEL;

			uint8_t * normalData = (uint8_t*)m_Image_Normal->GetData();

			uint8_t nR = normalData[lineOff + pixelOff];
			uint8_t nG = normalData[lineOff + pixelOff + 1];
			uint8_t nB = normalData[lineOff + pixelOff + 2];

			points->push_back(sample);
		}
	}
}


