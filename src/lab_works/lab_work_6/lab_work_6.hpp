#ifndef __LAB_WORK_6_HPP__
#define __LAB_WORK_6_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "common/Shader_utils.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork6 : public BaseLabWork
	{
	  public:
		LabWork6()	= default;
		~LabWork6() = default;

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;
		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

	  private:
		// === Étapes du rendu différé ===
		void _initGBuffer();
		void _initGeometryPassProgram();
		void _initShadingPassProgram();
		void _initFullScreenQuad();
		void _geometryPass();
		void _shadingPass();
		void _initCamera();
		void _updateViewMatrix(); 
		void resize( const int p_width, const int p_height );

		Camera			  _camera; 
		TriangleMeshModel _model;

		//=== Camera ===
		float  _fovy			  = 60.f;
		float  _cameraSpeed		  = 0.1f;
		float  _cameraSensitivity = 0.1f;

		Vec3f  _lightPosition	  = Vec3f( 0.f, 2.f, 2.f );

		bool _displayDebugTexture = true; // true = mode debug actif

		// === FBO et textures ===
		GLuint _gBufferFBO			 = GL_INVALID_INDEX;
		GLuint _gBufferTextures[ 6 ] = { 0 }; // 5 couleurs + 1 profondeur

		// === Programmes shaders ===
		GLuint _geometryPassProgram = GL_INVALID_INDEX;
		GLuint _shadingPassProgram	= GL_INVALID_INDEX;

		// === Quad plein écran ===
		GLuint _quadVAO = GL_INVALID_INDEX;
		GLuint _quadVBO = GL_INVALID_INDEX;
		GLuint _quadEBO = GL_INVALID_INDEX;

		// === Sélection de texture à afficher ===
		int _selectedTexture = 0;

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_6_HPP__
