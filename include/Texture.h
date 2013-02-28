#ifndef TEXTURE_H
#define	TEXTURE_H
#include "Export.h"
#include <string>
#include <GL/glew.h>
#include <ImageMagick/Magick++.h>

class ENGINE_EXPORT Texture
{
public:
	Texture(GLenum TextureTarget, const std::string& FileName);
	GLuint GetTextureObj(){return m_textureObj;};

	bool Load();

	void Bind(GLenum TextureUnit);

private:
	std::string m_fileName;
	GLenum m_textureTarget;
	GLuint m_textureObj;
	Magick::Image* m_pImage;
	Magick::Blob m_blob;
};

#endif	/* TEXTURE_H */

