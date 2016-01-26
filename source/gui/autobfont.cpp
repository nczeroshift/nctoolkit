
#include "autobfont.h"
#include "nckImage.h"
#include "nckColor4.h"
#include "nckUtils.h"

CellDatai::CellDatai()
{
	redline = 0x7FFFFFFF;
	left = 0x7FFFFFFF;
	right = 0xFFFFFFFF;
	top = 0x7FFFFFFF;
	bottom = 0xFFFFFFFF;
	xi = 0;
	yi = 0;
}

void ParseConversionMap(std::string filename,unsigned char **cv)
{
	FILE *f = fopen(filename.c_str(),"r");
	unsigned char ch;
	
	if(!f)
		throw std::string("Error in \"ParseConversionTable\", the file \"" + 
		filename + "\" doesn't exist.");

	std::string line_buffer = "";

	while(!feof(f))
	{
		fscanf(f,"%c",&ch);

		if(ch == '\n')
		{
			if(line_buffer.size()>1)
			{
				// Comment, ignore.
				if(line_buffer.substr(0,2) == "//")
				{
					line_buffer = "";
					continue;
				}

				std::string id = line_buffer.substr(0,line_buffer.find(","));
				std::string value = line_buffer.substr(line_buffer.find(",")+1,
					line_buffer.size()-line_buffer.find(",")+1);

				if(value.size()== 1 && id.size()>0)
				{
					unsigned char char_id = value.c_str()[0];
					int cell_id = atoi(id.c_str());

					if(!cv[cell_id])
						cv[cell_id] = new unsigned char;
					else{
						throw std::string("Error in \"ParseConversionTable\"") + 
							"while parsing the file \"" + filename  + 
							"\", the element \"" + value + "\" already exists.";
 					}

					*cv[cell_id] = char_id;
				}

				line_buffer = "";
			}	
		}
		else{
			// Remove tabs or spaces
			if(ch!= '\t' && ch != ' ' && ch != '\r')
				line_buffer+= ch;
		}
	}
	fclose(f);
}

/// Application entry point.
int main(int argc, char *argv[])
{
	// Conversion characters ID vector filename.
	std::string input_filename = "";

	// Input image characters map filename. 
	std::string map_filename = "";

	// Output configuration file filename.
	std::string output_filename = ""; 

	// Use the lower half of the image as bold characters.
	bool use_Bold = false;

	// Number of cell columns.
	int x_divisions = 16;

	// Number of cell lines.
	int y_divisions = 16;

	// Characters map image data.
	Core:: Image * imageMap = NULL;

	// Alpha threshold for font limits detection.
	unsigned char alpha_threshold = 50;

	// Line height.
	int line_height = 0;

	// Parse commands.
	for(int i = 1; i < argc; i++)
	{
		if(std::string(argv[i]) == "-i" && i+1 < argc)
		{
			char temp[128] = "";
			sprintf(temp,"%s",argv[i+1]);
			input_filename = temp;
		}
		else if(std::string(argv[i]) == "-m" && i+1 < argc)
		{
			char temp[128] = "";
			sprintf(temp,"%s",argv[i+1]);
			map_filename = temp;
		}
		else if(std::string(argv[i]) == "-o" && i+1 < argc)
		{
			char temp[128] = "";
			sprintf(temp,"%s",argv[i+1]);
			output_filename = temp;
		}
		else if(std::string(argv[i]) == "-b")
		{
			use_Bold = true;
		}
		else if(std::string(argv[i]) == "-t" && i+1 < argc)
		{
			alpha_threshold = atof(argv[i+1]);
		}
		else if(std::string(argv[i]) == "-h" && i+1 < argc)
		{
			line_height = atoi(argv[i+1]);
		}
		else if (std::string(argv[i]) == "-xd" && i + 1 < argc)
		{
			x_divisions = atoi(argv[i + 1]);
		}
		else if (std::string(argv[i]) == "-yd" && i + 1 < argc)
		{
			y_divisions = atoi(argv[i + 1]);
		}
		i++;
	}

	// Create character conversion vector.
	unsigned char **conversion_vector = new unsigned char*[x_divisions*y_divisions];
	for(int i = 0;i<x_divisions*y_divisions;i++)
		conversion_vector[i] = NULL;

	// Load and check the basic stuff
	try{
		if(output_filename == "" || input_filename == "" || map_filename == "")
			throw std::string("Error in \"main\", output,input & map filenames must be specified.");

		ParseConversionMap(input_filename,conversion_vector);
		imageMap = Core::Image::Load(map_filename);

		if(imageMap->GetFormat()!=Core::PIXEL_FORMAT_RGBA_8B)
			throw std::string("Error in \"main\", TGA image must be RGBA.");

		//if(imageMap->GetWidth() != imageMap->GetHeight())
		//	throw std::string("Error in \"main\", image dimensions must be equal.");
	}
	catch(std::string str)
	{
		printf("%s\n",str.c_str());
		SafeDelete(imageMap);
		return 0;
	}
	
	if(line_height == 0){
		line_height = (3*(imageMap->GetHeight()/y_divisions))/4;
	}

	// Start image processing.
	CellDatai ** cells = new CellDatai*[x_divisions * y_divisions];

	int cx_size = imageMap->GetHeight()/x_divisions;
	int cy_size = imageMap->GetWidth()/y_divisions;
	int dsize = (imageMap->GetHeight() * imageMap->GetWidth()-1) * 4;

	Math::Color4ub * imageData = (Math::Color4ub*)imageMap->GetData();

	// Lines
	for(int yi = 0; yi < y_divisions; yi++)
	{
		int ayi = yi * cy_size;

		// Columns
		for(int xi= 0;xi < x_divisions; xi++)
		{
			int axi = xi*cx_size;

			CellDatai **temp = cells + yi * x_divisions+xi;
			*temp = NULL;

			for(int x = 0;x < cx_size; x++)
			{
				for(int y = 0;y < cy_size; y++)
				{			
					Math::Color4ub * col = (Math::Color4ub*)imageData+((ayi + y)*imageMap->GetWidth() + (axi  + x));
			
					if(*temp)
						(*temp)->redline = line_height;

					//if(*temp && col->r == 0xFF && col->g == 0x00 && col->b == 0x00)
					//{
					//	(*temp)->redline = y;
					//}

					if(col->GetA() > alpha_threshold )
					{
						if(!*temp)
						{
							*temp = new CellDatai();
							(*temp)->xi = axi;
							(*temp)->yi = ayi;
						}

						if(x <=  (*temp)->left)
							(*temp)->left = x;

						if( x >= (*temp)->right)
							(*temp)->right = x;

						if(y<= (*temp)->top)
							(*temp)->top = y;

						if(y>= (*temp)->bottom)
							(*temp)->bottom = y;
					}
				}
			}
		}
	}
	
	FILE * fd = fopen(output_filename.c_str(),"w");

	if(!fd){
		printf("Error in \"main\", unable to create output file.");

		for (int yi = 0; yi < y_divisions; yi++)
			for (int xi = 0; xi < x_divisions; xi++){
				CellDatai **temp = cells + yi * x_divisions + xi;
				SafeDelete(*temp);
			}

		SafeArrayDelete(cells);
		SafeDelete(imageData);
		return 0;
	}

	fprintf(fd,"Parameter,Dimension,%d\n",imageMap->GetWidth());

	if(use_Bold)
		fprintf(fd,"Parameter,Bold,True\n");
	else
		fprintf(fd,"Parameter,Bold,False\n");

	fprintf(fd,"\n");

	for(int i = 0;i<x_divisions*y_divisions;i++)
	{
		if(i == 128 && use_Bold)
			fprintf(fd,"\n");

		if(cells[i])
		{
			if(conversion_vector[i])
				fprintf(fd,"Map,%d,%c,%d,%d,%d,%d,%d\n",i,*(conversion_vector[i]),cells[i]->redline,
					cells[i]->left,
					cells[i]->right,
					cells[i]->top,
					cells[i]->bottom);
			else
			{
				if(use_Bold && i>=128 && conversion_vector[i-128] )
				{
					fprintf(fd,"Map,%d,%c,%d,%d,%d,%d,%d\n",i,*(conversion_vector[i-128]),
						cells[i]->redline,
						cells[i]->left,
						cells[i]->right,
						cells[i]->top,
						cells[i]->bottom);
				}
			}
		}
	}

	fclose(fd);
	for (int yi = 0; yi < y_divisions; yi++)
		for (int xi = 0; xi < x_divisions; xi++) {
			CellDatai **temp = cells + yi * x_divisions + xi;
			SafeDelete(*temp);
		}
	SafeDelete(imageMap);
	SafeArrayDelete(cells);
	printf("Done\n");
}
