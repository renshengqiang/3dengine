#ifndef _SKELETON_BONE_H
#define _SKELETON_BONE_H
#include "math_3d.h"
#include "Node.h"
#include <string>
#include <vector>

class SkeletonBone : public Node{
public:
	SkeletonBone(const std::string &name, const Matrix4f &transform);
	SkeletonBone(const std::string &name);
	~SkeletonBone();
	const Matrix4f& GetTransform(void);
	void SetTransform(const Matrix4f &transform);
	SkeletonBone *CreateChildBone(const std::string &name, const Matrix4f &transform);
	
protected:
	virtual SkeletonBone* CreateChildImpl(const std::string &name);
};

#endif
