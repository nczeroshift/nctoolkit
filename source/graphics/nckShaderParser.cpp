
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckShaderParser.h"
#include <set>
#include <algorithm>
#include "nckDataIO.h"
#include "nckException.h"
#include "nckUtils.h"
#include <string.h>

_GRAPHICS_BEGIN

class ShaderToken{
public:
	enum Type{
		NONE,
		STRING,
		SPACE,
		NEWLINE
	};

	ShaderToken(){
		mType=NONE;
		mText="";
	}

	ShaderToken(Type type){
		mType=type;
		mText="";
	}

	ShaderToken(Type type, std::string text){
		mType = type;
		mText = text;
	}

	ShaderToken(Type type, char ch){
		mType = type;
		mText += ch;
	}

	~ShaderToken(){}

	static Type GetCharType(char ch){
		if(isSpacer(ch))
			return SPACE;
		else if(ch == '\n')
			return NEWLINE;
		else
			return STRING;
		return NONE;
	}

	inline Type GetType(){return mType;};
	inline std::string GetString(){return mText;}
	void AddChar(char ch){mText+=ch;}

private:
	static bool isSpacer(char ch){
		if(ch==' ' || ch == '\t')
			return true;

		return false;
	}

	Type mType;
	std::string mText;
};


std::map<std::string,std::list<std::string>> ShaderParser::Map(const std::string & srcText)
{
	ShaderToken ltoken;
	int state = 0;
	std::string * curstr = NULL;

	std::string currEntryName = "";
	std::map<std::string,std::list<std::string>> retMap;
	std::set<std::string> supportedShaders;

	supportedShaders.insert("vertex_shader_gl2");
	supportedShaders.insert("fragment_shader_gl2");
	supportedShaders.insert("vertex_shader_glx2");
	supportedShaders.insert("fragment_shader_glx2");
	supportedShaders.insert("vertex_shader_dx9");
	supportedShaders.insert("pixel_shader_dx9");
    supportedShaders.insert("vertex_shader_dx11");
	supportedShaders.insert("pixel_shader_dx11");
    supportedShaders.insert("geometry_shader_dx11");
	supportedShaders.insert("vertex_shader_gles2");
	supportedShaders.insert("pixel_shader_gles2");
 	supportedShaders.insert("vertex_shader_gl3");
	supportedShaders.insert("pixel_shader_gl3");
	supportedShaders.insert("geometry_shader_gl3");
   	supportedShaders.insert("vertex_shader_gl4");
	supportedShaders.insert("pixel_shader_gl4");
	supportedShaders.insert("geometry_shader_gl4");
    
	std::map<std::string,std::list<std::string>>::iterator currentIt = retMap.end();

	for(size_t i = 0;i<srcText.length();i++)
	{
		char ch = srcText[i];

		if(ch=='\r') continue; // Ignore the carriage return

		ShaderToken::Type type = ShaderToken::GetCharType(ch);

		if(ltoken.GetType()!=type)
		{
			if(ltoken.GetType()!=ShaderToken::NONE)
			{
				if(state==0 || state == 3)
				{
					if(ltoken.GetType()==ShaderToken::STRING && ltoken.GetString()=="#pragma")
						state = 1;
                    else if (ltoken.GetType() == ShaderToken::STRING && ltoken.GetString() == "#include")
                        state = 4;
				}
				else if(state == 1)
				{
					if(ltoken.GetType()==ShaderToken::SPACE){
						// do nothing
					}
					else if(ltoken.GetType()==ShaderToken::STRING && supportedShaders.find(ltoken.GetString()) != supportedShaders.end())
					{
                        std::list<std::string> vec = std::list<std::string>();
                        vec.push_back("");
						retMap.insert(std::pair<std::string, std::list<std::string>>(ltoken.GetString(), vec));
						currentIt = retMap.find(ltoken.GetString());
						state = 2; 
					}
					else{

						if(curstr){
							*curstr+="#pragma ";
							state = 3;
						}
						else
							state = 0;
					}
				}
				else if(state == 2)
				{
					if(ltoken.GetType()==ShaderToken::SPACE){
						// do nothing
					}
					else if(ltoken.GetType()==ShaderToken::NEWLINE)
						state = 3;
					else // This sentence is invalid
						state = 0;
                }
			}

            if (currentIt != retMap.end()) {
                if (state == 3)
                {
                    if (ltoken.GetType() == ShaderToken::SPACE)
                        currentIt->second.back() += " ";
                    else if (ltoken.GetType() == ShaderToken::NEWLINE)
                        currentIt->second.back() += "\n";
                    else
                        currentIt->second.back() += ltoken.GetString();
                }
                else if (state == 4)
                {
                    std::string tStr = ltoken.GetString();
                    if (tStr.find_first_of("\"") == 0 &&
                        tStr.find_last_of("\"") == tStr.length()-1)
                    {
                        std::string includeName = ltoken.GetString().substr(1, tStr.length() - 2);
                        //int64_t fSize = Core::FileReader::Size("shader://include/" + includeName);
                        Core::DataReader * fReader = Core::DataReader::Open("shader://include/" + includeName);
                        if (fReader == NULL)
                            THROW_EXCEPTION("Include \"" + includeName + "\" not found");
                        int64_t fSize = fReader->Length();

                        if (fSize > 0) {
                            char * text = new char[fSize + 1];
                            memset(text, '\0', fSize + 1);

                            fReader->Read(text, fSize);
                            std::string tmp = std::string(text);
                            SafeArrayDelete(text);
                            SafeDelete(fReader);

                            //tmp.erase(std::remove(tmp.begin(), tmp.end(), '\n'), tmp.end());
                            //tmp.erase(std::remove(tmp.begin(), tmp.end(), '\r'), tmp.end());

                            std::list<std::string>::iterator it = currentIt->second.end();
                            it--;
                            currentIt->second.insert(it, tmp + "\n");

                            currentIt->second.back() += "//"+ includeName+"\n";
                        }
                        state = 3;
                    }
                }
            }
			ltoken=ShaderToken(type);
		}

		ltoken.AddChar(ch);
	}

	if(ltoken.GetType() == ShaderToken::STRING && 
		ltoken.GetString().length()>0 && 
			currentIt != retMap.end()){
		currentIt->second.back() += ltoken.GetString();
	}

	return retMap;
}

_GRAPHICS_END