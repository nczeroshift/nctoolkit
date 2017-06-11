"""
* NCtoolKit (C) 2007-2017 Luis F.Loureiro, under zlib software license.
* code.google.com/p/nctoolkit
* 
* Toolkit python buildscript
"""

import subprocess
import os,sys
from stat import *
import datetime

compileOptimization = '-O0' # '-O3'

def PrepareCompiteToObject(incfolders,srcfile,outfile):
	fileFlag = os.path.exists(outfile)
	compileFlag = False
	res = None
	if(fileFlag):
		time_src = os.stat(srcfile)[ST_MTIME];time_obj = os.stat(outfile)[ST_MTIME]
		if(time_src>time_obj):
			compileFlag = True
	else:
		compileFlag = True
	if(compileFlag):
		print("Compiling : " + srcfile)
		commands = ['g++',srcfile,'-c','-msse3','-mfpmath=sse','-g','-ggdb',compileOptimization,'-o',outfile];
		if incfolders != None:
			for ipath in incfolders:
				commands.append('-I')
				commands.append(ipath);

		res = subprocess.Popen(commands,stdout=subprocess.PIPE,stderr=subprocess.PIPE);
	return res;

def WaitForCompile(process):
	process.wait()
	stderr_msg = process.stderr.read()
	if(process.returncode==1):
		print (stderr_msg)
		return False
	elif (stderr_msg):
		print (stderr_msg)
	return True


def CompileToOBJ(incfolders,srcfile,outfile):
	proc = PrepareCompiteToObject(incfolders,srcfile,outfile)
	return WaitForCompile(proc);


def LinkFromOBJ(objfolder,objfiles,appfile,libraries):
	fileFlag = os.path.exists(appfile)
	linkFlag = False

	if(fileFlag):
		time_app = os.stat(appfile)[ST_MTIME]
		for o in objfiles:
			if(not os.path.exists(objfolder+o) or os.stat(objfolder+o)[ST_MTIME]>time_app):
				linkFlag=True
				break	
	else:
		linkFlag=True
	if(linkFlag):
		print("Linking : " + appfile)
		linkerArgs = ['g++']
		for o in objfiles:
			linkerArgs.append(objfolder+o)
		
		if libraries != None:
			for lib in libraries:
				linkerArgs.append("-"+lib)

		linkerArgs.append('-o');linkerArgs.append(appfile)
		#print(linkerArgs)
		res = subprocess.Popen(linkerArgs,stdout=subprocess.PIPE,stderr=subprocess.PIPE);res.wait()
		stderr_msg = res.stderr.read()

		if(res.returncode==1):
			print (stderr_msg)
			return False
		elif (stderr_msg):
			print (stderr_msg)
	return True	

def CompileSource(ROOT_SRC_FOLDER, OUT_FOLDER):
	retvalue = True
	SRC_FILES = [
	#xml
	["xml",["tinystr.cpp","tinyxml.cpp","tinyxmlerror.cpp","tinyxmlparser.cpp"],None],
	#core objects
	["core",["nckChronometer.cpp","nckPoint.cpp","nckQueueBuffer.cpp","nckDate.cpp",
    "nckDataIO.cpp","nckImage.cpp","nckThread.cpp","nckException.cpp","nckUtils.cpp","nckWindow.cpp",
	"linux/nckWindow_linux.cpp"],None],
	#bxon
	["bxon",["bxon.cpp","bxonDataIO.cpp"],["core","math"]],
    #math objects
	["math",["nckBoundBox.cpp","nckColor4.cpp","nckFrustum.cpp","nckLine.cpp","nckMat44.cpp","nckMathUtils.cpp",
	"nckPlane.cpp","nckQuadtree.cpp","nckQuat.cpp","nckTransform.cpp","nckTriangle.cpp","nckVec2.cpp","nckVec3.cpp","nckVec4.cpp"],None],
	#graphics objects
	["graphics",["nckGraphics.cpp","nckShaderParser.cpp","nckTextureCache.cpp","gl2/nckExtensions_gl2.cpp","gl2/nckGraphics_gl2.cpp",
	"gl2/nckMesh_gl2.cpp","gl2/nckShader_gl2.cpp","gl2/nckTexture_gl2.cpp","gl2/nckProxy_gl2.cpp"],["math","core"]],
	#audio objects
	["audio",["nckAudioDevice.cpp","alsa/nckDeviceAlsa.cpp","nckOggStream.cpp"],["core"]],
	#scene objects
	["scene",["nckAnimation.cpp","nckArmature.cpp","nckCamera.cpp","nckCompound.cpp","nckDatablock.cpp",
	"nckLamp.cpp","nckMaterial.cpp","nckModel.cpp","nckObject.cpp","nckTexture.cpp","nckMarchingCubes.cpp",
    "nckCurve.cpp","nckGeometry.cpp"],["math","core","graphics","bxon"]],
	#gui objects
	["gui",["nckFont.cpp","nckShapeRenderer.cpp","nckWidget.cpp","nckWidgetRenderer.cpp"],["math","core","graphics"]],
	#io objects
	["io",["nckSerialPort.cpp"],["core"]],
	#network objects
	["network",["nckHttpServer.cpp"],["core"]],
	#video objects
	["video",["nckCameraDevice.cpp","nckColorSpace.cpp"],["core"]],
	#app
	["apps",["main.cpp","nckDemo.cpp","nckDemo_Selector.cpp","demos/nckDemo_Webcam.cpp","demos/nckDemo_Triangles.cpp",
    "demos/nckDemo_TextureWrapping.cpp","demos/nckDemo_TextureNPT.cpp","demos/nckDemo_TextureFiltering.cpp",
    "demos/nckDemo_TextureCubemap.cpp","demos/nckDemo_Texture3D.cpp","demos/nckDemo_Texture2D.cpp","demos/nckDemo_Shadows.cpp",
    "demos/nckDemo_ShaderProgram.cpp","demos/nckDemo_Serial.cpp","demos/nckDemo_Quadtree.cpp","demos/nckDemo_Particles.cpp",
    "demos/nckDemo_OcclusionQuery.cpp","demos/nckDemo_MultiCamera.cpp","demos/nckDemo_MotionBlur.cpp","demos/nckDemo_Model.cpp",
    "demos/nckDemo_Metaballs.cpp","demos/nckDemo_Material.cpp","demos/nckDemo_HttpStream.cpp","demos/nckDemo_HttpServer.cpp",
    "demos/nckDemo_GUI.cpp","demos/nckDemo_FBO.cpp","demos/nckDemo_Curves.cpp","demos/nckDemo_CubemapFBO.cpp",
    "demos/nckDemo_Compound.cpp","demos/nckDemo_Bumpmap.cpp","demos/nckDemo_AudioOut.cpp","demos/nckDemo_AudioOgg.cpp",
    "demos/nckDemo_AudioFFT.cpp","demos/nckDemo_Armature.cpp","demos/nckDemo_Animation.cpp"],["bxon","xml","math","core","graphics","gui","scene","network","audio","video","bxporter","io"]]
	]

	previous = None
	for entry in SRC_FILES:
		src_fld = entry[0]+"/";
		inc_fld = None;
		if not entry[2] == None: 
			for inc_entry in entry[2]:
				if inc_fld == None:
					inc_fld = []
				inc_fld.append(ROOT_SRC_FOLDER+inc_entry) 

		for src in entry[1]:
			current = PrepareCompiteToObject(inc_fld,ROOT_SRC_FOLDER+src_fld+src,OUT_FOLDER + os.path.basename(src.split('.')[0] + ".o"))
			if previous!=None:
				if(not WaitForCompile(previous)):
					retvalue = False
			previous = current

		
	if previous!=None:
		if(not WaitForCompile(previous)):
			retvalue = False
	
	return retvalue

def LinkExporter(OBJ_FOLDER,OUTPUT_BINARY,LIBS):
	retvalue = True
	LIB_OBJ_FILES =[
	# xml
	"tinystr.o","tinyxml.o","tinyxmlerror.o","tinyxmlparser.o",
    #bxon
	"bxon.o","bxonDataIO.o",
    # core objs
	"nckChronometer.o","nckPoint.o","nckQueueBuffer.o","nckDataIO.o","nckDate.o","nckImage.o","nckThread.o","nckUtils.o","nckWindow.o","nckWindow_linux.o","nckException.o",
	# math objs
	"nckBoundBox.o","nckColor4.o","nckFrustum.o","nckLine.o","nckMat44.o","nckMathUtils.o",
	"nckPlane.o","nckQuadtree.o","nckQuat.o","nckTransform.o","nckTriangle.o","nckVec2.o","nckVec3.o","nckVec4.o",
	# graphics objs
	"nckGraphics.o","nckTextureCache.o","nckShaderParser.o","nckExtensions_gl2.o","nckGraphics_gl2.o",
	"nckMesh_gl2.o","nckShader_gl2.o","nckTexture_gl2.o","nckProxy_gl2.o",
	# scene objs
	"nckAnimation.o","nckArmature.o","nckCamera.o","nckCompound.o","nckDatablock.o",
	"nckLamp.o","nckMaterial.o","nckModel.o","nckObject.o","nckTexture.o","nckMarchingCubes.o","nckGeometry.o","nckCurve.o",
	# gui objs
	"nckFont.o","nckShapeRenderer.o","nckWidget.o","nckWidgetRenderer.o",
	# network obs
	"nckHttpServer.o",
	# audio objs
	"nckAudioDevice.o","nckDeviceAlsa.o","nckOggStream.o",
	# video objs
	"nckCameraDevice.o","nckColorSpace.o",
	# IO
	"nckSerialPort.o",
	# main
	"main.o","nckDemo.o","nckDemo_Selector.o","nckDemo_Webcam.o","nckDemo_Triangles.o",
    "nckDemo_TextureWrapping.o","nckDemo_TextureNPT.o","nckDemo_TextureFiltering.o",
    "nckDemo_TextureCubemap.o","nckDemo_Texture3D.o","nckDemo_Texture2D.o","nckDemo_Shadows.o",
    "nckDemo_ShaderProgram.o","nckDemo_Serial.o","nckDemo_Quadtree.o","nckDemo_Particles.o",
    "nckDemo_OcclusionQuery.o","nckDemo_MultiCamera.o","nckDemo_MotionBlur.o","nckDemo_Model.o",
    "nckDemo_Metaballs.o","nckDemo_Material.o","nckDemo_HttpStream.o","nckDemo_HttpServer.o",
    "nckDemo_GUI.o","nckDemo_FBO.o","nckDemo_Curves.o","nckDemo_CubemapFBO.o",
    "nckDemo_Compound.o","nckDemo_Bumpmap.o","nckDemo_AudioOut.o","nckDemo_AudioOgg.o",
    "nckDemo_AudioFFT.o","nckDemo_Armature.o","nckDemo_Animation.o"]
		
	LinkFromOBJ(OBJ_FOLDER,LIB_OBJ_FILES,OUTPUT_BINARY,LIBS)

	return retvalue

source_folder = "../../source/"
output_folder = "obj/"
output_binary = "nctoolkit"
linker_libs = ["lGLU","lGL","lpng","ljpeg","lz","lX11","lasound","logg","lvorbisfile","lvorbis","lpthread"]

if not os.path.isdir(source_folder):
	print "Source folder not found !"
	exit(0);

if not os.path.isdir(output_folder):
	os.mkdir(output_folder)
	print("Output folder created !");

if CompileSource(source_folder,output_folder) and LinkExporter(output_folder,output_binary,linker_libs):
	print "nctoolkit compiled & linked, done !"

	
