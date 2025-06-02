#include "imgui.h"
#include "lab_work_4.hpp"
#include "utils/read_file.hpp"
#include "common/Shader_utils.hpp"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"
#include "utils/random.hpp"

namespace M3D_ISICG
{
	const std::string LabWork4::_shaderFolder = "src/lab_works/lab_work_4/shaders/";

	LabWork4::~LabWork4() {}

	bool LabWork4::init()
	{
		std::cout << "Initializing LabWork 4..." << std::endl;

		_program = createProgramFromFiles( _shaderFolder + "mesh.vert", _shaderFolder + "mesh.frag" );
		if ( _program == 0 )
			return false;

		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		glEnable( GL_DEPTH_TEST );

		// Chargement du modèle
		_model.load( "bunny", "data/models/bunny/bunny.obj" );

		_initCamera();
		glUseProgram( _program );

		_uMVPMatrixLoc		= glGetUniformLocation( _program, "uMVPMatrix" );
		_uNormalMatrixLoc	= glGetUniformLocation( _program, "uNormalMatrix" );
		_uMVMatrixLoc		= glGetUniformLocation( _program, "uMVMatrix" );
		_uCameraPositionLoc = glGetUniformLocation( _program, "cameraPosition" );
		_uViewMatrixLoc		= glGetUniformLocation( _program, "uViewMatrix" );
		_uLightPositionLoc	= glGetUniformLocation( _program, "uLightPosition" );

		return true;
	}

	void LabWork4::animate( const float ) {}

	void LabWork4::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glUseProgram( _program );

		Mat4f mvp		   = _camera.getProjectionMatrix() * _camera.getViewMatrix() * _model._transformation;
		Mat4f MVMatrix	   = _camera.getViewMatrix() * _model._transformation;
		Mat3f normalMatrix = Mat3f( glm::transpose( glm::inverse( MVMatrix ) ) );

		glProgramUniformMatrix4fv( _program, _uMVPMatrixLoc, 1, GL_FALSE, glm::value_ptr( mvp ) );
		glProgramUniformMatrix3fv( _program, _uNormalMatrixLoc, 1, GL_FALSE, glm::value_ptr( normalMatrix ) );
		glProgramUniformMatrix4fv( _program, _uViewMatrixLoc, 1, GL_FALSE, glm::value_ptr( _camera.getViewMatrix() ) );
		glProgramUniformMatrix4fv( _program, _uMVMatrixLoc, 1, GL_FALSE, glm::value_ptr( MVMatrix ) );
		glProgramUniform3fv( _program, _uCameraPositionLoc, 1, glm::value_ptr( _camera.getPos() ) );

		glProgramUniform3fv( _program, _uLightPositionLoc, 1, glm::value_ptr( _lightPosition ) );

		glProgramUniform3fv(
			_program, glGetUniformLocation( _program, "ambientColor" ), 1, glm::value_ptr( _ambientColor ) );
		glProgramUniform3fv(
		 	_program, glGetUniformLocation( _program, "difusColor" ), 1, glm::value_ptr( _diffuseColor ) );
		glProgramUniform3fv(
		 	_program, glGetUniformLocation( _program, "speculaireColor" ), 1, glm::value_ptr( _specularColor ) );
		glProgramUniform1f( _program, glGetUniformLocation( _program, "shininess" ), _shininess );

		_model.render( _program );
	}

	void LabWork4::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera.moveFront( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_S: // Back
				_camera.moveFront( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_A: // Left
				_camera.moveRight( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_D: // Right
				_camera.moveRight( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_R: // Up
				_camera.moveUp( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera.moveUp( -_cameraSpeed );
				_updateViewMatrix();
				break;
			default: break;
			}
		}

		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
			_updateViewMatrix();
		}
	}

	void LabWork4::_updateViewMatrix()
	{
		_uViewMatrixLoc = glGetUniformLocation( _program, "viewMatrix" );
		glProgramUniformMatrix4fv( _program, _uViewMatrixLoc, 1, GL_FALSE, &_camera.getViewMatrix()[ 0 ][ 0 ] );
	}

	void LabWork4::resize( const int p_width, const int p_height )
	{
		BaseLabWork::resize( p_width, p_height );
		_camera.setScreenSize( p_width, p_height );
	}

	void LabWork4::_initCamera()
	{
		_camera.setScreenSize( _windowWidth, _windowHeight );
		_camera.setPosition( Vec3f( 0.f, 0.f, 2.f ) ); // caméra en recul
		_camera.setLookAt( Vec3f( 0.f, 0.f, 0.f ) );   // regarde vers l'origine
		_camera.setFovy( _fovy );					   // 60° par défaut
	}

	void LabWork4::_resetLightingToDefault()
	{
		_ambientColor  = Vec3f( 0.2f, 0.2f, 0.2f ); 
		_diffuseColor  = Vec3f( 0.2f, 0.4f, 1.0f ); 
		_specularColor = Vec3f( 1.0f, 1.0f, 1.0f ); 
		_shininess	   = 32.0f;
	}


	void LabWork4::displayUI()
	{
		ImGui::Begin( "Settings lab work 4" );

		//  Modifier la couleur de fond de la scène
		if ( ImGui::ColorEdit3( "Background", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}

		//  Lumière ambiante
		ImGui::ColorEdit3( "Ambient", glm::value_ptr( _ambientColor ) );

		//  Couleur diffuse principale (ex: bleu)
		ImGui::ColorEdit3( "Diffuse", glm::value_ptr( _diffuseColor ) );

		// Couleur des reflets spéculaires
		ImGui::ColorEdit3( "Specular", glm::value_ptr( _specularColor ) );

		// Contrôle de la netteté du reflet
		ImGui::SliderFloat( "Shininess", &_shininess, 1.0f, 128.0f, "%.1f" );

		// Réinitialise toutes les valeurs lumineuses aux valeurs TP
		if ( ImGui::Button( "Reset Lighting (TP Default)" ) )
			_resetLightingToDefault();

		ImGui::SliderFloat3( "Light Position", glm::value_ptr( _lightPosition ), -5.0f, 5.0f, "%.1f" );

		// Contrôle de l’angle de vue de la caméra (fovy)
		if ( ImGui::SliderFloat( "fovy", &_fovy, 10.f, 160.f, "%01.f" ) )
			_camera.setFovy( _fovy );

		ImGui::End();
	}

} // namespace M3D_ISICG
