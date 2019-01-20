#include"OctreeNode.h"
#include"JGRCObject.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;



void OctreeNode::BuildOctree(UINT vCenter, UINT vHalfWidth, UINT Level)
{

}

void OctreeNode::DistributeObject(std::vector<class JGRCObject*>& ObjArr)
{
	
}
bool OctreeNode::IsIncludeNode(Camera* MainCamera)
{
	return false;
}
std::vector<JGRCObject*>& OctreeNode::GetArray()
{
	return m_StaticObjects;
}