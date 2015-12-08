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

		res = subprocess.Popen(linkerArgs,stdout=subprocess.PIPE,stderr=subprocess.PIPE);res.wait()
		stderr_msg = res.stderr.read()

		if(res.returncode==1):
			print (stderr_msg)
			return False
		elif (stderr_msg):
			print (stderr_msg)
	return True	


def CompileSource(SRC_FOLDER, OUT_FOLDER):
	retvalue = True
	"""SRC_FILES =[
	"../bxporter_cli/appExporter.cpp",
	"bXAnimationGraph.cpp",
	"bXArmature.cpp",
	"bXCamera.cpp",
	"bXCurve.cpp",
	"bXExportSupport.cpp",
	"bXFile.cpp",
	"bXLamp.cpp",
	"bXMaterial.cpp",
	"bXMesh.cpp",
	"bXMeshMaterialMixer.cpp",
	"bXTextureMapping.cpp",
	"bXMeshTools.cpp",
	"bXObject.cpp",
	"bXProperties.cpp",
	"bXScene.cpp",
	"bXTexture.cpp",
	"bXVectorMath.cpp",
	"tinystr.cpp",
	"../xml/tinyxml.cpp",
	"../xml/tinyxmlerror.cpp",
	"../xml/tinyxmlparser.cpp"]
	for src in SRC_FILES:
		if(not CompileToOBJ(SRC_FOLDER+src,OUT_FOLDER + src.split('.')[0] + ".o")):
			retvalue = False

	return retvalue
	"""
	SRC_FILES = [
	#xml
	["xml",["tinystr.cpp","tinyxml.cpp","tinyxmlerror.cpp","tinyxmlparser.cpp"],None],
	["bxporter",["bXAnimationGraph.cpp","bXArmature.cpp","bXCamera.cpp","bXCurve.cpp","bXExportSupport.cpp",
	"bXFile.cpp","bXLamp.cpp","bXMaterial.cpp","bXMesh.cpp","bXMeshMaterialMixer.cpp","bXTextureMapping.cpp","bXMeshTools.cpp",
	"bXObject.cpp","bXProperties.cpp","bXScene.cpp","bXTexture.cpp","bXVectorMath.cpp"],["xml"]],
	#core objects
	["bxporter_cli",["appExporter.cpp"],["xml","bxporter"]]
	]

	previous = None
	for entry in SRC_FILES:
		src_fld = entry[0]+"/";
		inc_fld = None;
		if not entry[2] == None: 
			for inc_entry in entry[2]:
				if inc_fld == None:
					inc_fld = []
				inc_fld.append(SRC_FOLDER+inc_entry) 

		for src in entry[1]:
			current = PrepareCompiteToObject(inc_fld,SRC_FOLDER+src_fld+src,OUT_FOLDER + os.path.basename(src.split('.')[0] + ".o"))
			if previous!=None:
				if(not WaitForCompile(previous)):
					retvalue = False
			previous = current

		
	if previous!=None:
		if(not WaitForCompile(previous)):
			retvalue = False
	
	return retvalue

def LinkExporter(OBJ_FOLDER,OUTPUT_BINARY):
	retvalue = True
	LIB_OBJ_FILES =[
	"appExporter.o",
	"bXAnimationGraph.o",
	"bXArmature.o",
	"bXCamera.o",
	"bXCurve.o",
	"bXExportSupport.o",
	"bXFile.o",
	"bXLamp.o",
	"bXMaterial.o",
	"bXMesh.o",
	"bXMeshMaterialMixer.o",
	"bXMeshTools.o",
	"bXObject.o",
	"bXProperties.o",
	"bXScene.o",
	"bXTexture.o",
	"bXVectorMath.o",
	"tinystr.o",
	"tinyxml.o",
	"tinyxmlerror.o",
	"tinyxmlparser.o",
	"bXTextureMapping.o"]
		
	LinkFromOBJ(OBJ_FOLDER,LIB_OBJ_FILES,OUTPUT_BINARY,[])

	return retvalue

source_folder = "../../source/"
output_folder = "obj/"
output_binary = "bxporter"

if not os.path.isdir(source_folder):
	print "Source folder not found !"
	exit(0);

if not os.path.isdir(output_folder):
	os.mkdir(output_folder)
	print("Output folder created !");

if CompileSource(source_folder,output_folder) and LinkExporter(output_folder,output_binary):
	print "bXporter compiled & linked, done !"

	
