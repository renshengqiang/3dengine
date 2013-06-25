#include <Octree.h>
#include <OctreeNode.h>

//-----------------------------------------------------------------------
Octree::Octree( Octree * parent ) :
      m_halfSize( 0, 0, 0 )
{
	//initialize all children to null.
	for ( int i = 0; i < 2; i++ )
	{
	    for ( int j = 0; j < 2; j++ )
	    {
	        for ( int k = 0; k < 2; k++ )
	        {
	            m_children[ i ][ j ][ k ] = 0;
	        }
	    }
	}

	mp_parent = parent;
	m_numNodes = 0;
}
//-----------------------------------------------------------------------
Octree::~Octree()
{
	//initialize all children to null.
	for ( int i = 0; i < 2; i++ )
	{
	    for ( int j = 0; j < 2; j++ )
	    {
	        for ( int k = 0; k < 2; k++ )
	        {
	            if ( m_children[ i ][ j ][ k ] != 0 )
	                delete m_children[ i ][ j ][ k ];
	        }
	    }
	}

	mp_parent = 0;
}
//-----------------------------------------------------------------------
/** Returns true is the box will fit in a child.
*/
bool Octree::_IsTwiceSize( const AxisAlignedBox &box ) const
{
	// infinite boxes never fit in a child - always root node
	if (box.isInfinite())
		return false;

	Vector3f halfMBoxSize = m_box.getHalfSize();
	Vector3f boxSize = box.getSize();
	return ((boxSize.x <= halfMBoxSize.x) && (boxSize.y <= halfMBoxSize.y) && (boxSize.z <= halfMBoxSize.z));

}
//-----------------------------------------------------------------------
/** It's assumed the the given box has already been proven to fit into
* a child.  Since it's a loose octree, only the centers need to be
* compared to find the appropriate node.
*/
void Octree::_GetChildIndexes( const AxisAlignedBox &box, int *x, int *y, int *z ) const
{
	Vector3f center = m_box.getMaximum().midPoint( m_box.getMinimum() );

	Vector3f ncenter = box.getMaximum().midPoint( box.getMinimum() );

	if ( ncenter.x > center.x )
	    * x = 1;
	else
	    *x = 0;

	if ( ncenter.y > center.y )
	    * y = 1;
	else
	    *y = 0;

	if ( ncenter.z > center.z )
	    * z = 1;
	else
	    *z = 0;

}
//-----------------------------------------------------------------------
void Octree::_AddNode( OctreeNode * n )
{
	mNodes.push_back( n );
	n->SetOctant( this );

	//update total counts.
	_ref();

}
//-----------------------------------------------------------------------
void Octree::_RemoveNode( OctreeNode * n )
{
	mNodes.erase( std::find( mNodes.begin(), mNodes.end(), n ) );
	n -> SetOctant( 0 );

	//update total counts.
	_unref();
}
//-----------------------------------------------------------------------
void Octree::_GetCullBounds( AxisAlignedBox *b ) const
{
	b->setExtents( m_box.getMinimum() - m_halfSize, m_box.getMaximum() + m_halfSize );
}
