#include "imgui.h"
#include "lab_work_4.hpp"
#include "utils/read_file.hpp"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"
#include "utils/random.hpp"

namespace M3D_ISICG
{
	const std::string LabWork4::_shaderFolder = "src/lab_works/lab_work_4/shaders/";


	LabWork4::~LabWork4()
	{
		
	}

	bool LabWork4::init() 
	{
		std::cout << "Initializing lab work 4..." << std::endl;

		const std::string vertexShaderStr	= readFile( _shaderFolder + "lw4.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw4.frag" );
		const GLchar *	  vSrc				= vertexShaderStr.c_str();
		const GLchar *	  fSrc				= fragmentShaderStr.c_str();

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// Creation de l'objet shader pour le vertex 'vshader'
		GLuint vshader = glCreateShader( GL_VERTEX_SHADER );
		glShaderSource( vshader, 1, &vSrc, NULL );
		glCompileShader( vshader );

		// Creation de l'objet shader pour le fragment 'fshader'
		GLuint fshader = glCreateShader( GL_FRAGMENT_SHADER );
		glShaderSource( fshader, 1, &fSrc, NULL );
		glCompileShader( fshader );

		// Check if compilation is ok.
		GLint compiled;
		glGetShaderiv( vshader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vshader, sizeof( log ), NULL, log );
			glDeleteShader( vshader );
			glDeleteShader( fshader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		// Création de notre program
		GLuint program = glCreateProgram();

		

		// On attache nos shaders à notre programme
		glAttachShader( program, vshader );
		glAttachShader( program, fshader );
		glLinkProgram( program );

		// Check if link is ok.
		GLint linked;
		glGetProgramiv( program, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( program, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		// Destruction des shader
		glDeleteShader( vshader );
		glDeleteShader( fshader );

		
		// utilisation du programme
		glUseProgram( program );

		std::cout << "Done!" << std::endl;
		return true;
	}

	
	void LabWork4::animate( const float p_deltaTime ) {
		
	}

	void LabWork4::render()
	{
		
	}

	void LabWork4::handleEvents( const SDL_Event & p_event ) {}

	void LabWork4::displayUI()
	{
		ImGui::Begin( "Settings lab work 4" );
		ImGui::Text( "No setting available!" );


		ImGui::End();
	}

} // namespace M3D_ISICG
