#include "imgui.h"
#include "lab_work_3.hpp"
#include "utils/read_file.hpp"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"
#include "utils/random.hpp"

namespace M3D_ISICG
{
	const std::string LabWork3::_shaderFolder = "src/lab_works/lab_work_3/shaders/";


	LabWork3::~LabWork3()
	{
		
	}

	bool LabWork3::init() 
	{
		std::cout << "Initializing lab work 3..." << std::endl;

		const std::string vertexShaderStr	= readFile( _shaderFolder + "lw3.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw3.frag" );
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

		_cube = _createCube();
		// on va creer les points de notre cube
		Vec3f pointA = Vec3f( -0.5, 0.5, -0.5 );
		Vec3f pointB = Vec3f( 0.5, 0.5, -0.5 );
		Vec3f pointC = Vec3f( 0.5, -0.5, -0.5 );
		Vec3f pointD = Vec3f( -0.5, -0.5, -0.5 );
		Vec3f pointE = Vec3f( -0.5, 0.5, 0.5f );
		Vec3f pointF = Vec3f( 0.5, 0.5, 0.5f );
		Vec3f pointG = Vec3f( 0.5, -0.5, 0.5f );
		Vec3f pointH = Vec3f( -0.5, -0.5, 0.5f );

		// on range les points de notre cube
		_cube.pSommets = { pointA, pointB, pointC, pointD, pointE, pointF, pointG, pointH };

		// on va avoir des couleurs random pour notre cube
		for ( int i = 0; i < this->_cube.pSommets.size(); i++ )
		{
			_cube.cSommets.push_back( getRandomVec3f() );
		}

		// Trie du tableau en fonction de l'ebo
		_cube.iSommets = { 0, 1, 3, 1, 2, 3, 
						   1, 5, 6, 1, 2, 6, 
						   4, 5, 6, 4, 7, 6, 
						   0, 4, 7, 0, 3, 7, 4, 5, 1, 4, 0, 1, 2, 6, 7, 7, 3, 2 };



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

		_initBuffers();
		
		// utilisation du programme
		glUseProgram( program );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork3::_initBuffers() {
		// Initialisation du vbo position et vbo colors
		glCreateBuffers( 1, &_cube._vboPositions );
		glNamedBufferData(
						  _cube._vboPositions, 
						  _cube.pSommets.size() * sizeof( glm::vec3 ), 
					      _cube.pSommets.data(), 
						  GL_STATIC_DRAW );
		glCreateBuffers( 1, &_cube._vboColors );
		glNamedBufferData(
						  _cube._vboColors, 
						  _cube.pSommets.size() * sizeof( glm::vec3 ), 
						  _cube.pSommets.data(), 
						  GL_STATIC_DRAW );

		//Creation du vao 
		glCreateVertexArrays( 1, &_cube.vao );
		glEnableVertexArrayAttrib( _cube.vao, 0 );
		glEnableVertexArrayAttrib( _cube.vao, 1 );
		//Definition du format de l'attribut
		glVertexArrayAttribFormat( _cube.vao, 0, 3, GL_FLOAT,GL_FALSE, 0 );
		glVertexArrayAttribFormat( _cube.vao, 1, 3, GL_FLOAT, GL_FALSE, 0 );
		//Specifier le vbo à lire
		glVertexArrayVertexBuffer( _cube.vao, 1, _cube._vboColors, 0, sizeof( glm::vec3 ) );
		glVertexArrayVertexBuffer( _cube.vao, 0, _cube._vboPositions, 0, sizeof( glm::vec3 ) );
		//Connecter le vao et VertexShader
		glVertexArrayAttribBinding( _cube.vao, 0, 0 );
		glVertexArrayAttribBinding( _cube.vao, 1, 1 );

	}

	// creation de notre cube, on va lui faire subir une transformation de sclaing, et envoyer la model matrix
	LabWork3::Mesh LabWork3::_createCube()
	{
		Mesh	  cube		 = Mesh();
		glm::mat4 transformation = glm::mat4( 1.f );
		/*transformation			 = glm::scale( transformation, glm::vec3( 0.8f ) );
		returned._transformation = transformation;
		_uModelMatrixLoc		 = glGetUniformLocation( _program, "modelMatrix" );
		glProgramUniformMatrix4fv( _program, _uModelMatrixLoc, 1, GL_FALSE, &transformation[ 0 ][ 0 ] );*/

		return cube;
	}

	void LabWork3::animate( const float p_deltaTime ) {
		
	}

	void LabWork3::render()
	{
		// netoyage du framebuffer
		//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		// Lier le Vao au programme
		//glBindVertexArray( _cube.vao );
		// GL_TRIANGLE en paramètre pour spécifier de dessiner un triangle avec les 3 sommets
		//glDrawElements( GL_TRIANGLES, _cube.pSommets.size(), GL_UNSIGNED_INT, 0 );
		//glBindVertexArray( 0 );
	}

	void LabWork3::handleEvents( const SDL_Event & p_event ) {}

	void LabWork3::displayUI()
	{
		ImGui::Begin( "Settings lab work 3" );
		ImGui::Text( "No setting available!" );


		ImGui::End();
	}

} // namespace M3D_ISICG
