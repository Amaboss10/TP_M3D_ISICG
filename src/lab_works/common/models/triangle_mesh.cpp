#include "triangle_mesh.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

namespace M3D_ISICG
{
	TriangleMesh::TriangleMesh( const std::string &				  p_name,
								const std::vector<Vertex> &		  p_vertices,
								const std::vector<unsigned int> & p_indices,
								const Material &				  p_material ) :
		_name( p_name ),
		_vertices( p_vertices ), _indices( p_indices ), _material( p_material )
	{
		_vertices.shrink_to_fit();
		_indices.shrink_to_fit();
		_setupGL();
	}

	void TriangleMesh::render( const GLuint p_glProgram ) const
	{
		glUseProgram( p_glProgram );

		Vec3f ambientColor	   = this->_material._ambient;
		Vec3f diffuseColor = Vec3f( this->_material._diffuse );
		Vec3f specularColor	   = this->_material._specular;
		bool  uHasSpecularMap  = this->_material._hasSpecularMap;
		bool  uHasShininessMap = this->_material._hasShininessMap;
		bool  uHasNormalMap	   = this->_material._hasNormalMap;

		bool  uHasAmbientMap   = this->_material._hasAmbientMap;
		bool uHasDiffuseMap	 = this->_material._hasDiffuseMap;

	
	
		if ( uHasDiffuseMap || uHasAmbientMap || uHasSpecularMap )
		{	
			GLuint textureAmbientId	  = this->_material._ambientMap._id;
			GLuint textureDiffuseId = this->_material._diffuseMap._id;
			GLuint textureSpecularId  = this->_material._specularMap._id;
			GLuint textureShininessId = this->_material._shininessMap._id;
			GLuint textureNormalId	  = this->_material._normalMap._id;
			if ( uHasAmbientMap )
			{
				glBindTextureUnit( 2, textureAmbientId );
			}
			if ( uHasDiffuseMap )
			{
				glBindTextureUnit( 1, textureDiffuseId );
			}
			if ( uHasSpecularMap )
			{
				glBindTextureUnit( 3, textureSpecularId );
			}
			if ( uHasShininessMap )
			{
				glBindTextureUnit( 4, textureShininessId );
			}
			if ( uHasNormalMap )
			{
				glBindTextureUnit( 5, textureNormalId );
			}
		}
		// Uniform values
		GLfloat aColor		  = glGetUniformLocation( p_glProgram, "ambientColor" );
		GLfloat dColor = glGetUniformLocation( p_glProgram, "diffuseColor" );
		GLfloat sColor		  = glGetUniformLocation( p_glProgram, "specularColor" );
		GLfloat shininess	  = glGetUniformLocation( p_glProgram, "shininessVal" );
		GLuint	mDiffuseMap	  = glGetUniformLocation( p_glProgram, "uHasDiffuseMap" );
		GLuint	mAmbientMap	  = glGetUniformLocation( p_glProgram, "uHasAmbientMap" );
		GLuint	mSpecularMap  = glGetUniformLocation( p_glProgram, "uHasSpecularMap" );
		GLuint	mShininessMap = glGetUniformLocation( p_glProgram, "uHasShininessMap" );
		GLuint	mNormalMap	  = glGetUniformLocation( p_glProgram, "uHasNormalMap" );

		glUniform3f( aColor, ambientColor.x, ambientColor.y, ambientColor.z );
		glUniform3f( dColor, diffuseColor.x, diffuseColor.y, diffuseColor.z );
		glUniform3f( sColor, specularColor.x, specularColor.y, specularColor.z );
		glUniform1f( shininess, this->_material._shininess );
		glUniform1f( mDiffuseMap, this->_material._hasDiffuseMap );
		glUniform1f( mAmbientMap, this->_material._hasAmbientMap );
		glUniform1f( mSpecularMap, this->_material._hasSpecularMap );
		glUniform1f( mShininessMap, this->_material._hasShininessMap );
		glUniform1f( mNormalMap, this->_material._hasNormalMap );

		glBindVertexArray( _vao );
		glDrawElements( GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0 );
		glBindVertexArray( 0 );
		glUseProgram( 0 );
		
		if ( uHasDiffuseMap || uHasAmbientMap || uHasSpecularMap )
		{
			if ( uHasAmbientMap )
			{
				glBindTextureUnit( 0, this->_material._ambientMap._id );
			}
			if ( uHasDiffuseMap )
			{
				glBindTextureUnit( 0, this->_material._diffuseMap._id );
			}
			if ( uHasSpecularMap )
			{
				glBindTextureUnit( 0, this->_material._specularMap._id );
			}
			if ( uHasShininessMap )
			{
				glBindTextureUnit( 0, this->_material._shininessMap._id );
			}
			if ( uHasNormalMap )
			{
				glBindTextureUnit( 0, this->_material._normalMap._id );
			}
		}
	}

	void TriangleMesh::cleanGL()
	{
		glDisableVertexArrayAttrib( _vao, 0 );
		glDisableVertexArrayAttrib( _vao, 1 );
		glDisableVertexArrayAttrib( _vao, 2 );
		glDisableVertexArrayAttrib( _vao, 3 );
		glDisableVertexArrayAttrib( _vao, 4 );
		glDeleteVertexArrays( 1, &_vao );
		glDeleteBuffers( 1, &_vbo );
		glDeleteBuffers( 1, &_ebo );
	}

	void TriangleMesh::_setupGL()
	{		
		// creation vbo
		glCreateBuffers( 1, &_vbo );
		glNamedBufferData( _vbo, _vertices.size() * sizeof( Vertex ), _vertices.data(), GL_STATIC_DRAW );
		// creation ebo
		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _ebo, _indices.size() * sizeof( unsigned int ), _indices.data(), GL_STATIC_DRAW );

		// creation vao
		glCreateVertexArrays( 1, &_vao );
		// lie vao et vbo
		glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vertex ) );

		// chaque id pour un atribut diffenrents
		// 0: Cela va etre pour la position
		glEnableVertexArrayAttrib( _vao, 0 );
		glVertexArrayAttribFormat( _vao,
								   0,
								   3, // 3 pour le nombre de valeurs vec3

								   GL_FLOAT, // GL_Float car on traite des flottant vec3F
								   GL_FALSE,
								   offsetof( Vertex, _position ) ); // offset à utiliser
		glVertexArrayAttribBinding( _vao, 0, 0 );

		// 1: Cela va etre pour la position
		glEnableVertexArrayAttrib( _vao, 1 );
		glVertexArrayAttribFormat( _vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _normal ) );
		glVertexArrayAttribBinding( _vao, 1, 0 );

		// 2: Cela va etre pour la texCoor
		glEnableVertexArrayAttrib( _vao, 2 );
		glVertexArrayAttribFormat( _vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof( Vertex, _texCoords ) );
		glVertexArrayAttribBinding( _vao, 2, 0 );

		// 3: Cela va etre pour la tangente
		glEnableVertexArrayAttrib( _vao, 3 );
		glVertexArrayAttribFormat( _vao, 3, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _tangent ) );
		glVertexArrayAttribBinding( _vao, 3, 0 );

		// 4: Cela va etre pour la bitangente
		glEnableVertexArrayAttrib( _vao, 4 );
		glVertexArrayAttribFormat( _vao, 4, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _bitangent ) );
		glVertexArrayAttribBinding( _vao, 4, 0 );

		// on lie avec le vao
		glVertexArrayElementBuffer( _vao, _ebo );

	}
} // namespace M3D_ISICG
