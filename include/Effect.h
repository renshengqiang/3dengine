#ifndef _EFFECT_H
#define _EFFECT_H
#include <list>
#include <GL/glew.h>

#define INVALID_LOCATION 0xFFFFFFFF

class Effect
{
public:
	typedef std::list<GLuint> ShaderObjList;
	enum SHADER_TYPE{	VERTEX_SHADER, FRAGMENT_SHADER};
	Effect();
	~Effect();
	virtual bool Init(void);
	void Enable(void);
	static void Disable(void);
protected:
	bool AddShader(SHADER_TYPE shaderType, const char *fileName);
	bool Finalize(void);
	GLint GetUniformLocation(const char *pUniformName);
	GLint GetAttribLocation(const char *pAttribName);
private:
	GLuint m_shaderProgram;
	ShaderObjList m_shaderObjList;
};

#endif
