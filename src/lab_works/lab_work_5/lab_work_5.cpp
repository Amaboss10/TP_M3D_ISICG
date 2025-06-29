#include "lab_work_5.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "common/Shader_utils.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork5::_shaderFolder = "src/lab_works/lab_work_5/shaders/";

	LabWork5::~LabWork5() {}

	bool LabWork5::init()
	{
		std::cout << "Initializing LabWork 5 (Textures)..." << std::endl;

		_program = createProgramFromFiles( _shaderFolder + "mesh_texture.vert", _shaderFolder + "mesh_texture.frag" );
		if ( _program == 0 )
			return false;

		glEnable( GL_DEPTH_TEST );
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// Chargement du modèle
		_loadScene( "sponza" );

		_initCamera();

		glUseProgram( _program );

		// Matrices
		_uMVPMatrixLoc	  = glGetUniformLocation( _program, "uMVPMatrix" );
		_uMVMatrixLoc	  = glGetUniformLocation( _program, "uMVMatrix" );
		_uViewMatrixLoc	  = glGetUniformLocation( _program, "uViewMatrix" );
		_uNormalMatrixLoc = glGetUniformLocation( _program, "uNormalMatrix" );

		// Caméra & lumière
		_uCameraPositionLoc = glGetUniformLocation( _program, "cameraPosition" );
		_uLightPositionLoc	= glGetUniformLocation( _program, "uLightPosition" );

		return true;
	}


	void LabWork5::animate( const float ) {}

	void LabWork5::render()
	{
		if ( _program == 0 )
			return;

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glUseProgram( _program );

		Mat4f modelMatrix  = _model._transformation;
		Mat4f viewMatrix   = _camera.getViewMatrix();
		Mat4f projMatrix   = _camera.getProjectionMatrix();
		Mat4f mvMatrix	   = viewMatrix * modelMatrix;
		Mat4f mvpMatrix	   = projMatrix * mvMatrix;
		Mat3f normalMatrix = Mat3f( glm::transpose( glm::inverse( mvMatrix ) ) );

		glProgramUniformMatrix4fv( _program, _uMVPMatrixLoc, 1, GL_FALSE, glm::value_ptr( mvpMatrix ) );
		glProgramUniformMatrix4fv( _program, _uMVMatrixLoc, 1, GL_FALSE, glm::value_ptr( mvMatrix ) );
		glProgramUniformMatrix4fv( _program, _uViewMatrixLoc, 1, GL_FALSE, glm::value_ptr( viewMatrix ) );
		glProgramUniformMatrix3fv( _program, _uNormalMatrixLoc, 1, GL_FALSE, glm::value_ptr( normalMatrix ) );

		glProgramUniform3fv( _program, _uCameraPositionLoc, 1, glm::value_ptr( _camera.getPos() ) );
		glProgramUniform3fv( _program, _uLightPositionLoc, 1, glm::value_ptr( _lightPosition ) );

		// Material
		glProgramUniform3fv(
			_program, glGetUniformLocation( _program, "ambientColor" ), 1, glm::value_ptr( _ambientColor ) );
		glProgramUniform3fv(
			_program, glGetUniformLocation( _program, "diffuseColor" ), 1, glm::value_ptr( _diffuseColor ) );
		glProgramUniform3fv(
			_program, glGetUniformLocation( _program, "specularColor" ), 1, glm::value_ptr( _specularColor ) );
		glProgramUniform1f( _program, glGetUniformLocation( _program, "shininess" ), _shininess );
		glProgramUniform1i( _program, glGetUniformLocation( _program, "useBlinn" ), _useBlinnPhong );

		glEnable( GL_BLEND ); 
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); 
		_model.render( _program );
		glDisable( GL_BLEND );

	}


	void LabWork5::handleEvents( const SDL_Event & p_event )
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
	void LabWork5::resize( const int p_width, const int p_height )
	{
		BaseLabWork::resize( p_width, p_height );
		_camera.setScreenSize( p_width, p_height );
	}

	void LabWork5::_initCamera()
	{
		_camera.setScreenSize( _windowWidth, _windowHeight ); 
		_camera.setPosition( Vec3f( 0.46f, 1.11f, 0.36f ) );
		_camera.setLookAt( Vec3f( 0.2f, 1.f, 0.2f ) );
		_lightPosition = Vec3f( 0.f, 3.f, 2.f );

	}
	void LabWork5::_updateViewMatrix()
	{
		glProgramUniformMatrix4fv( _program, _uViewMatrixLoc, 1, GL_FALSE, &_camera.getViewMatrix()[ 0 ][ 0 ] );
	}

	void LabWork5::_loadScene( const std::string & name )
	{
		_model.cleanGL();
		_model = TriangleMeshModel(); // Reset du modèle

		if ( name == "sponza" )
		{
			_model.load( "sponza", "data/models/sponza/sponza.obj" );
			_model._transformation = glm::scale( glm::mat4( 1.f ), glm::vec3( 0.003f ) );
			_camera.setPosition( Vec3f( -0.16f, 0.99f, 0.42f ) );
			_camera.setLookAt( Vec3f( 0.2f, 1.f, 0.2f ) );
			_lightPosition = Vec3f( 0.f, 3.f, 2.f );
		}
		else if ( name == "bunny" )
		{
			_model.load( "bunny", "data/models/bunny/bunny.obj" );
			_model._transformation = glm::mat4( 1.f );
			_camera.setPosition( Vec3f( 0.f, 0.f, 2.f ) );
			_camera.setLookAt( Vec3f( 0.f, 0.f, 0.f ) );
			_lightPosition = Vec3f( 0.f, 2.f, 1.f );
		}

		_updateViewMatrix();
	}




	void LabWork5::displayUI()
	{
		ImGui::Begin( "TP5 - Texture Settings" );

		// === Couleurs matériau ===
		if ( ImGui::ColorEdit3( "Ambient", glm::value_ptr( _ambientColor ) ) )
			glProgramUniform3fv(
				_program, glGetUniformLocation( _program, "ambientColor" ), 1, glm::value_ptr( _ambientColor ) );

		if ( ImGui::ColorEdit3( "Diffuse", glm::value_ptr( _diffuseColor ) ) )
			glProgramUniform3fv(
				_program, glGetUniformLocation( _program, "diffuseColor" ), 1, glm::value_ptr( _diffuseColor ) );

		if ( ImGui::ColorEdit3( "Specular", glm::value_ptr( _specularColor ) ) )
			glProgramUniform3fv(
				_program, glGetUniformLocation( _program, "specularColor" ), 1, glm::value_ptr( _specularColor ) );

		if ( ImGui::SliderFloat( "Shininess", &_shininess, 1.f, 256.f ) )
			glProgramUniform1f( _program, glGetUniformLocation( _program, "shininess" ), _shininess );

		// === Lumière ===
		ImGui::SliderFloat3( "Light Position", glm::value_ptr( _lightPosition ), -5.0f, 5.0f, "%.1f" );

		// === Caméra ===
		if ( ImGui::SliderFloat( "fovy", &_fovy, 10.f, 160.f, "%01.f" ) )
			_camera.setFovy( _fovy );

		const Vec3f & camPos = _camera.getPos();
		ImGui::Text( "Camera Position: X=%.2f  Y=%.2f  Z=%.2f", camPos.x, camPos.y, camPos.z );

		// === Modèle d'éclairage ===
		if ( ImGui::Checkbox( "Use Blinn-Phong", &_useBlinnPhong ) )
			glProgramUniform1i( _program, glGetUniformLocation( _program, "useBlinn" ), _useBlinnPhong );

		// === Sélection de la scène ===
		const char * sceneLabels[] = { "Bunny", "Sponza" };
		if ( ImGui::Combo( "Scene", &_currentSceneIndex, sceneLabels, IM_ARRAYSIZE( sceneLabels ) ) )
		{
			_sceneNames = { "bunny", "sponza" };
			_loadScene( _sceneNames[ _currentSceneIndex ] );
		}

		// === Reload shader ===
		if ( ImGui::Button( "Reload Shaders" ) )
		{
			GLuint newProgram
				= createProgramFromFiles( _shaderFolder + "mesh_texture.vert", _shaderFolder + "mesh_texture.frag" );
			if ( newProgram != 0 )
			{
				glDeleteProgram( _program );
				_program = newProgram;
				glUseProgram( _program );

				_uMVPMatrixLoc		= glGetUniformLocation( _program, "uMVPMatrix" );
				_uMVMatrixLoc		= glGetUniformLocation( _program, "uMVMatrix" );
				_uViewMatrixLoc		= glGetUniformLocation( _program, "uViewMatrix" );
				_uNormalMatrixLoc	= glGetUniformLocation( _program, "uNormalMatrix" );
				_uCameraPositionLoc = glGetUniformLocation( _program, "cameraPosition" );
				_uLightPositionLoc	= glGetUniformLocation( _program, "uLightPosition" );
			}
		}

		// === Contrôle des textures ===
		static bool	 enableDiffuseMap  = true;
		static bool	 enableSpecularMap = true;
		static bool	 enableNormalMap   = true;
		static float specularIntensity = 1.0f;

		ImGui::Checkbox( "Use Diffuse Map", &enableDiffuseMap );
		ImGui::Checkbox( "Use Specular Map", &enableSpecularMap );
		ImGui::Checkbox( "Use Normal Map", &enableNormalMap );
		ImGui::SliderFloat( "Specular Intensity", &specularIntensity, 0.0f, 2.0f );

		glProgramUniform1i( _program, glGetUniformLocation( _program, "uEnableDiffuseMap" ), enableDiffuseMap );
		glProgramUniform1i( _program, glGetUniformLocation( _program, "uEnableSpecularMap" ), enableSpecularMap );
		glProgramUniform1i( _program, glGetUniformLocation( _program, "uEnableNormalMap" ), enableNormalMap );
		glProgramUniform1f( _program, glGetUniformLocation( _program, "uSpecularIntensity" ), specularIntensity );

		ImGui::End();
	}



} // namespace M3D_ISICG
