#include "lab_work_7.hpp"
#include "common/shader_utils.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork7::_shaderFolder = "src/lab_works/lab_work_7/shaders/";

	bool LabWork7::init()
	{
		std::cout << "Initializing LabWork7 - Projet " << std::endl;

		// Load model
		_model.load( "vokselia", "data/models/vokselia_spawn/vokselia_spawn.obj" );
		_camera.setPosition( Vec3f( 0.f, 1.f, 3.f ) );
		_camera.setLookAt( Vec3f( 0.f, 1.f, 0.f ) );

		// Setup
		_initCamera();
		_initFBO();
		_initFullScreenQuad();
		_initGeometryPass();
		_initFXAA();
		_initShadowMap();

		glEnable( GL_DEPTH_TEST );
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		return true;
	}

	void LabWork7::_initCamera() { _camera.setScreenSize( _windowWidth, _windowHeight ); }

	void LabWork7::_initFullScreenQuad()
	{
		float quadVertices[] = {
			// positions    // texCoords
			-1.f, -1.f, 0.f, 0.f, 1.f, -1.f, 1.f, 0.f, -1.f, 1.f, 0.f, 1.f, 1.f, 1.f, 1.f, 1.f,
		};

		glCreateVertexArrays( 1, &_quadVAO );
		glCreateBuffers( 1, &_quadVBO );
		glNamedBufferStorage( _quadVBO, sizeof( quadVertices ), quadVertices, 0 );

		glVertexArrayVertexBuffer( _quadVAO, 0, _quadVBO, 0, 4 * sizeof( float ) );

		glEnableVertexArrayAttrib( _quadVAO, 0 );
		glVertexArrayAttribFormat( _quadVAO, 0, 2, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayAttribBinding( _quadVAO, 0, 0 );

		glEnableVertexArrayAttrib( _quadVAO, 1 );
		glVertexArrayAttribFormat( _quadVAO, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof( float ) );
		glVertexArrayAttribBinding( _quadVAO, 1, 0 );
	}

	void LabWork7::_initFBO()
	{
		glCreateTextures( GL_TEXTURE_2D, 1, &_sceneColorTex );
		glTextureStorage2D( _sceneColorTex, 1, GL_RGBA8, _windowWidth, _windowHeight );

		glCreateRenderbuffers( 1, &_sceneDepthRBO );
		glNamedRenderbufferStorage( _sceneDepthRBO, GL_DEPTH24_STENCIL8, _windowWidth, _windowHeight );

		glCreateFramebuffers( 1, &_sceneFBO );
		glNamedFramebufferTexture( _sceneFBO, GL_COLOR_ATTACHMENT0, _sceneColorTex, 0 );
		glNamedFramebufferRenderbuffer( _sceneFBO, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _sceneDepthRBO );

		if ( glCheckNamedFramebufferStatus( _sceneFBO, GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
			std::cerr << "Framebuffer incomplete!" << std::endl;
	}

	void LabWork7::_initFXAA()
	{
		_fxaaProgram = createProgramFromFiles( _shaderFolder + "fxaa_pass.vert", _shaderFolder + "fxaa_pass.frag" );
	}

	void LabWork7::_initGeometryPass()
	{
		_geometryProgram
			= createProgramFromFiles( _shaderFolder + "geometry_pass.vert", _shaderFolder + "geometry_pass.frag" );
	}

	void LabWork7::_updateGeometryUniforms()
	{
		if ( _geometryProgram == 0 )
			return;

		Mat4f mv		= _camera.getViewMatrix() * _model._transformation;
		Mat4f mvp		= _camera.getProjectionMatrix() * mv;
		Mat3f normalMat = glm::transpose( glm::inverse( Mat3f( mv ) ) );

		glUseProgram( _geometryProgram );
		glProgramUniformMatrix4fv( _geometryProgram,
								   glGetUniformLocation( _geometryProgram, "uMVMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( mv ) );
		glProgramUniformMatrix4fv( _geometryProgram,
								   glGetUniformLocation( _geometryProgram, "uMVPMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( mvp ) );
		glProgramUniformMatrix3fv( _geometryProgram,
								   glGetUniformLocation( _geometryProgram, "uNormalMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( normalMat ) );
	}

	void LabWork7::_geometryPass()
	{
		glBindFramebuffer( GL_FRAMEBUFFER, _sceneFBO );
		glViewport( 0, 0, _windowWidth, _windowHeight );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// === Calcul des matrices ===
		_updateGeometryUniforms(); // envoie uMVMatrix, uMVPMatrix, uNormalMatrix

		// === Activation du shader geometry ===
		glUseProgram( _geometryProgram );

		// === Uniformes caméra & lumière ===
		glProgramUniform3fv( _geometryProgram,
							 glGetUniformLocation( _geometryProgram, "cameraPosition" ),
							 1,
							 glm::value_ptr( _camera.getPos() ) );

		glProgramUniform3fv( _geometryProgram,
							 glGetUniformLocation( _geometryProgram, "uLightPosition" ),
							 1,
							 glm::value_ptr( _lightPosition ) );

		// === Matériaux ===
		glProgramUniform3fv( _geometryProgram,
							 glGetUniformLocation( _geometryProgram, "ambientColor" ),
							 1,
							 glm::value_ptr( _ambientColor ) );
		glProgramUniform3fv( _geometryProgram,
							 glGetUniformLocation( _geometryProgram, "diffuseColor" ),
							 1,
							 glm::value_ptr( _diffuseColor ) );
		glProgramUniform3fv( _geometryProgram,
							 glGetUniformLocation( _geometryProgram, "specularColor" ),
							 1,
							 glm::value_ptr( _specularColor ) );
		glProgramUniform1f( _geometryProgram, glGetUniformLocation( _geometryProgram, "shininess" ), _shininess );
		glProgramUniform1f(
			_geometryProgram, glGetUniformLocation( _geometryProgram, "uSpecularIntensity" ), _specularIntensity );
		glProgramUniform1f( _geometryProgram, glGetUniformLocation( _geometryProgram, "uAOStrength" ), _aoStrength );

		// === Shadow Mapping ===
		glProgramUniform1f( _geometryProgram, glGetUniformLocation( _geometryProgram, "uShadowBias" ), _shadowBias );
		glProgramUniform1i(
			_geometryProgram, glGetUniformLocation( _geometryProgram, "uEnableShadow" ), _enableShadow );
		glProgramUniformMatrix4fv( _geometryProgram,
								   glGetUniformLocation( _geometryProgram, "uLightViewProj" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( _lightViewProj ) );

		// === Options textures ===
		glProgramUniform1i( _geometryProgram, glGetUniformLocation( _geometryProgram, "uUseBlinn" ), _useBlinnPhong );
		glProgramUniform1i(
			_geometryProgram, glGetUniformLocation( _geometryProgram, "uEnableDiffuseMap" ), _enableDiffuseMap );
		glProgramUniform1i(
			_geometryProgram, glGetUniformLocation( _geometryProgram, "uEnableSpecularMap" ), _enableSpecularMap );
		glProgramUniform1i(
			_geometryProgram, glGetUniformLocation( _geometryProgram, "uEnableNormalMap" ), _enableNormalMap );
		glProgramUniform1i( _geometryProgram, glGetUniformLocation( _geometryProgram, "uEnableAOMap" ), _enableAOMap );

		// === Textures ===
		const auto & material = _model._meshes[ 0 ]._material;
		glBindTextureUnit( 1, material._diffuseMap._id );
		glBindTextureUnit( 2, material._specularMap._id );
		glBindTextureUnit( 3, material._normalMap._id );
		glBindTextureUnit( 4, material._ambientOcclusionMap._id );
		glBindTextureUnit( 5, _shadowDepthMap );

		// === Dessin du modèle ===
		_model.render( _geometryProgram );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void LabWork7::_fxaaPass()
	{
		if ( !_useFXAA )
			return;
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glUseProgram( _fxaaProgram );
		glBindTextureUnit( 0, _sceneColorTex );
		glBindVertexArray( _quadVAO );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	}

	void LabWork7::_initShadowMap()
	{
		// Créer texture depth
		glCreateTextures( GL_TEXTURE_2D, 1, &_shadowDepthMap );
		glTextureStorage2D( _shadowDepthMap, 1, GL_DEPTH_COMPONENT32F, SHADOW_RES, SHADOW_RES );
		glTextureParameteri( _shadowDepthMap, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( _shadowDepthMap, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTextureParameteri( _shadowDepthMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
		glTextureParameteri( _shadowDepthMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
		const float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
		glTextureParameterfv( _shadowDepthMap, GL_TEXTURE_BORDER_COLOR, borderColor );

		// FBO
		glCreateFramebuffers( 1, &_shadowFBO );
		glNamedFramebufferTexture( _shadowFBO, GL_DEPTH_ATTACHMENT, _shadowDepthMap, 0 );
		glNamedFramebufferDrawBuffer( _shadowFBO, GL_NONE );
		glNamedFramebufferReadBuffer( _shadowFBO, GL_NONE );

		if ( glCheckNamedFramebufferStatus( _shadowFBO, GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
			std::cerr << "[Shadow] Framebuffer incomplete!" << std::endl;

		// Compile le programme
		_shadowProgram
			= createProgramFromFiles( _shaderFolder + "shadow_pass.vert", _shaderFolder + "shadow_pass.frag" );
	}

	void LabWork7::_shadowPass()
	{
		glUseProgram( _shadowProgram );
		glBindFramebuffer( GL_FRAMEBUFFER, _shadowFBO );
		glViewport( 0, 0, SHADOW_RES, SHADOW_RES );
		glClear( GL_DEPTH_BUFFER_BIT );

		// Calcul de la matrice VP lumière (vue orthographique pour une lumière directionnelle)
		Vec3f lightDir	  = glm::normalize( _lightPosition );
		Vec3f lightTarget = Vec3f( 0.f ); // regarde le centre
		Vec3f up		  = Vec3f( 0.f, 1.f, 0.f );

		Mat4f lightView = glm::lookAt( _lightPosition, lightTarget, up );
		float s			= _shadowOrthoSize;
		Mat4f lightProj = glm::ortho( -s, s, -s, s, 0.1f, 30.f );
		_lightViewProj	= lightProj * lightView;

		// Uniforms
		glProgramUniformMatrix4fv( _shadowProgram,
								   glGetUniformLocation( _shadowProgram, "uLightViewProj" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( _lightViewProj ) );
		glProgramUniformMatrix4fv( _shadowProgram,
								   glGetUniformLocation( _shadowProgram, "uModelMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( _model._transformation ) );

		_model.render( _shadowProgram );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void LabWork7::render()
	{
		_shadowPass();
		_geometryPass();
		_fxaaPass();
	}

	void LabWork7::resize( const int p_width, const int p_height )
	{
		_windowWidth  = p_width;
		_windowHeight = p_height;
		_camera.setScreenSize( _windowWidth, _windowHeight );
		_initFBO();
	}

	void LabWork7::handleEvents( const SDL_Event & p_event )
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

	void LabWork7::_updateViewMatrix() { _updateGeometryUniforms(); }

	void LabWork7::displayUI()
	{
		ImGui::Begin( "LabWork 7 - Projet" );
		ImGui::Checkbox( "Enable FXAA", &_useFXAA );
		ImGui::SliderFloat( "FOV Y", &_fovy, 10.f, 160.f );
		_camera.setFovy( _fovy );
		ImGui::Separator();
		ImGui::Text( "Material" );
		ImGui::Separator();
		ImGui::ColorEdit3( "Ambient", glm::value_ptr( _ambientColor ) );
		ImGui::ColorEdit3( "Diffuse", glm::value_ptr( _diffuseColor ) );
		ImGui::ColorEdit3( "Specular", glm::value_ptr( _specularColor ) );
		ImGui::SliderFloat( "Shininess", &_shininess, 1.f, 256.f );
		ImGui::SliderFloat( "Specular Intensity", &_specularIntensity, 0.f, 2.f );
		ImGui::Checkbox( "Use Blinn-Phong", &_useBlinnPhong );
		ImGui::Separator();
		ImGui::Text( "Textures" );
		ImGui::Separator();
		ImGui::Checkbox( "Use Diffuse Map", &_enableDiffuseMap );
		ImGui::Checkbox( "Use Specular Map", &_enableSpecularMap );
		ImGui::Checkbox( "Use Normal Map", &_enableNormalMap );
		ImGui::Separator();
		ImGui::Text( "Shadow Mapping" );
		ImGui::Checkbox( "Enable Shadows", &_enableShadow );
		ImGui::SliderFloat( "Shadow Bias", &_shadowBias, 0.005f, 1.f, "%.5f" );
		ImGui::SliderFloat( "Ortho Size", &_shadowOrthoSize, 2.f, 50.f );
		ImGui::SliderFloat3( "Light Position", glm::value_ptr( _lightPosition ), -20.f, 20.f );

		ImGui::End();
	}
} // namespace M3D_ISICG
