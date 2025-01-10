#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "lab_work_5.hpp"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork5::_shaderFolder = "src/lab_works/lab_work_4/shaders/";

	LabWork5::~LabWork5() {}

	bool LabWork5::init()
	{
		std::cout << "Initializing lab work 5..." << std::endl;

		const std::string vertexShaderStr	= readFile( _shaderFolder + "lw5.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw5.frag" );
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

	void LabWork5::animate( const float p_deltaTime ) {}

	void LabWork5::render() {}

	void LabWork5::handleEvents( const SDL_Event & p_event ) {}

	void LabWork5::displayUI()
	{
		ImGui::Begin( "Settings lab work 5" );
		ImGui::Text( "No setting available!" );

		ImGui::End();
	}

} // namespace M3D_ISICG
