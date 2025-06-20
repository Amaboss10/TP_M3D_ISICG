// === lab_work_7.hpp ===
#ifndef __LAB_WORK_7_HPP__
#define __LAB_WORK_7_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "define.hpp"
#include <string>

namespace M3D_ISICG
{
	class LabWork7 : public BaseLabWork
	{
	  public:
		LabWork7()	= default;
		~LabWork7() = default;

		bool init() override;
		void animate( const float ) override {}
		void render() override;
		void handleEvents( const SDL_Event & ) override;
		void _updateViewMatrix();
		void displayUI() override;
		void resize( const int, const int ) override;

	  private:
		void _initCamera();
		void _initFullScreenQuad();
		void _initFBO();
		void _initFXAA();
		void _updateGeometryUniforms();

		void _geometryPass();
		void _fxaaPass();
		void _initGeometryPass();

		// === Scene ===
		Camera			  _camera;
		TriangleMeshModel _model;



		// === Geometry pass ===
		GLuint _sceneFBO		= 0;
		GLuint _sceneColorTex	= 0;
		GLuint _sceneDepthRBO	= 0;
		GLuint _geometryProgram = 0;

		// === FXAA ===
		GLuint _fxaaProgram = 0;
		GLuint _quadVAO		= 0;
		GLuint _quadVBO		= 0;
		bool   _useFXAA		= true;

		// === Material & Lighting ===
		Vec3f _ambientColor		 = Vec3f( 0.3f, 0.3f, 0.3f );
		Vec3f _diffuseColor		 = Vec3f( 1.f, 1.f, 1.f );
		Vec3f _specularColor	 = Vec3f( 1.f, 1.f, 1.f );
		Vec3f _lightPosition	 = Vec3f( 5.f, 10.f, 5.f );
		float _shininess		 = 32.f;
		float _specularIntensity = 1.f;
		float _aoStrength		 = 1.0f;
		bool  _useBlinnPhong	 = true;

		// === Texture toggles ===
		bool _enableDiffuseMap	= true;
		bool _enableSpecularMap = true;
		bool _enableNormalMap	= true;
		bool _enableAOMap		= true;

		// === UI / Config ===
		Vec4f _bgColor			 = Vec4f( 0.1f, 0.1f, 0.1f, 1.f );
		float _fovy				 = 60.f;
		float _cameraSpeed		 = 0.1f;
		float _cameraSensitivity = 0.1f;

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_7_HPP__
