// ProcessAudio.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <cstdint>
#include <vector>

#include <Windows.h>
#include <io.h>
bool connectAudio(const char *headerFile, const char *dataFile);
char* wcharToChar(TCHAR *s);
void listFiles(std::vector<std::string> &vec, const char * dir);
std::string getCurrentExePath();

using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	std::string currentFolder = getCurrentExePath();

	std::vector<std::string> vecFiles;
	listFiles(vecFiles, currentFolder.c_str());


	for (int i = 0; i < vecFiles.size(); i++){
		std::string header = currentFolder;
		header.append("\\0.mp4");

		std::string srcFile = currentFolder;
		srcFile.append("\\");
		srcFile.append(vecFiles[i]);
		connectAudio(header.c_str(), srcFile.c_str());
	}
	
	return 0;
}


bool connectAudio(const char *headerFile,const char *dataFile)
{
	if (!headerFile || !dataFile){
		return false;
	}

	std::string srcData(dataFile);
	std::string fileName = srcData.substr(0, srcData.find(".amp4"));
	fileName.append(".mp4");
	FILE *dest = fopen(fileName.c_str(), "wb+");

	int length = 0;
	FILE *fileHeader = fopen(headerFile, "rb");
	if (fileHeader != NULL){
		uint8_t szData[1024*2]= { 0 };
		int length = fread(szData, 1, sizeof(szData), fileHeader);

		if (length > 0){
			fwrite(szData, 1, length, dest);
		}
		fclose(fileHeader);
	} else {
		return false;
	}

	FILE *fileData = fopen(dataFile, "rb");
	if (fileData != NULL){
		while(!feof(fileData))
		{
			uint8_t szData[1024*2] = { 0 };
			length = fread(szData, 1, sizeof(szData), fileData);
			fwrite(szData, 1, length, dest);
		}

		fclose(fileData);
	}

	fclose(dest);
	
	std::cout << "merge file success!" << std::endl;
	return true;
}


char* wcharToChar(TCHAR *s){
	int w_nlen=WideCharToMultiByte(CP_ACP,0,s,-1,NULL,0,NULL,false);
	char *ret=new char[w_nlen];
	memset(ret,0,w_nlen);
	WideCharToMultiByte(CP_ACP,0,s,-1,ret,w_nlen,NULL,false);
	return ret;
}

void listFiles(std::vector<std::string> &vec,  const char * dir)
{
	char dirNew[200];
	strcpy(dirNew, dir);
	strcat(dirNew, "\\*.*");    // ��Ŀ¼�������"\\*.*"���е�һ������

	intptr_t handle;
	_finddata_t findData;

	handle = _findfirst(dirNew, &findData);
	if (handle == -1)        // ����Ƿ�ɹ�
		return;

	do
	{
		if (findData.attrib & _A_SUBDIR)
		{
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;

			cout << findData.name << "\t<dir>\n";

			// ��Ŀ¼�������"\\"����������Ŀ¼��������һ������
			strcpy(dirNew, dir);
			strcat(dirNew, "\\");
			strcat(dirNew, findData.name);

			listFiles(vec, dirNew);
		}
		else{
			std::string path = findData.name;
			size_t index = path.find(".amp4");
			if (index != std::string::npos){
				//std::string fileName = path.substr(0, index);
				vec.push_back(path);
			}

			//cout << findData.name << "\t" << findData.size << " bytes.\n";
		}
	} while (_findnext(handle, &findData) == 0);

	_findclose(handle);    // �ر��������
}

std::string getCurrentExePath(){
	std::string currentFolder;
	HMODULE module = GetModuleHandle(0); 
	char currentPath[MAX_PATH]; 
	GetModuleFileNameA(module, currentPath, sizeof(currentPath)); 

	std::string path(currentPath);

	size_t pos = path.find_last_of("\\");
	if (pos != std::string::npos){
		currentFolder = path.substr(0, pos);
	}


	return currentFolder;
}