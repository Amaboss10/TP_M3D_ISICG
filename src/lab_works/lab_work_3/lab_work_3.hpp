#ifndef __LAB_WORK_3_HPP__
#define __LAB_WORK_3_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork3 : public BaseLabWork
	{
		struct Mesh
		{
			~Mesh()
			{
				glDisableVertexArrayAttrib( vao, 0 );
				glDisableVertexArrayAttrib( vao, 1 );
				glDeleteVertexArrays( 1, &vao );
				glDeleteBuffers( 1, &_vboPositions );
				glDeleteBuffers( 1, &_vboColors );
				glDeleteBuffers( 1, &ebo );
			}
			// ================ GL data.

			std::vector<glm::vec3>	  pSommets; // Point des sommets
			std::vector<glm::vec3>	  cSommets; // couleur des sommets
			std::vector<unsigned int> iSommets; // indices de sommets
			Mat4f					  _transformation;

			// ================ GL data.
			GLuint vao = GL_INVALID_INDEX;
			GLuint ebo = GL_INVALID_INDEX;

			// ================ Vertex Buffer Objects.
			GLuint _vboPositions;
			GLuint _vboColors;
			// ================
		};

	  public:
		LabWork3() : BaseLabWork() {}
		~LabWork3();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

	  private:
		// ================ Scene data.
		Mesh _cube;
		// ================

		// ================ Create a mesh
		Mesh _createCube();

		// ================ GL data.
		// Ajout d'un attribut pour stocker l'identifiant du programme
		GLuint program;

		// ================ Functions
		void _initBuffers();

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_3_HPP__