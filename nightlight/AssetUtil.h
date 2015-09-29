#pragma once
#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include "WICTextureLoader\WICTextureLoader.h"

using namespace DirectX;

namespace AssetUtility 
{
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
		XMFLOAT3 normal;
	};

	struct Model 
	{
		~Model ( ) 
		{
			vertexBuffer->Release ( );
		}

		std::string name;
		ID3D11Buffer* vertexBuffer;
		int vertexBufferSize;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;

	};

	struct RenderObject 
	{
		Model* model;
		ID3D11ShaderResourceView* diffuseTexture = nullptr;
		ID3D11ShaderResourceView* specularTexture = nullptr;
	};

	static void SplitStringToVector ( std::string input, std::vector<std::string> &output, std::string delimiter ) 
	{
		size_t pos = 0;
		std::string token;
		while ( ( pos = input.find ( delimiter ) ) != std::string::npos ) 
		{
			token = input.substr ( 0, pos );
			if (!token.empty()) 
			{
				output.push_back ( token );
			}
			input.erase ( 0, pos + delimiter.length ( ) );
		}
		output.push_back ( input );
	};

	//turns a text file into a vector of strings line-by-line
	static void FileToStrings ( std::string file_path, std::vector<std::string> &output ) 
	{
		std::ifstream file ( file_path );
		if ( !file.is_open ( ) ) 
		{
			printf ( "Could not open " );
			printf ( file_path.c_str() );
			printf ( "\n" );
			return;
		}
		std::string s ( ( std::istreambuf_iterator<char> ( file ) ), std::istreambuf_iterator<char> ( ) );

		SplitStringToVector ( s, output, "\n" );
	};

	static std::vector<int> StringToIntArray ( std::string input )
	{
		std::vector<int> output;
		int from = 0;

		for ( int to = 0; to < ( signed )input.size ( ); to++ ) 
		{
			if ( input[ to ] == ',' ) 
			{
				output.push_back ( std::stoi ( input.substr ( from, to - from ) ) );
				from = to + 1;
			}
		}
		output.push_back ( std::stoi ( input.substr ( from, input.size ( ) - from ) ) );
		return output;
	}
}
