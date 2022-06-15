//----------------------------------------------------------------------------------
// File : asdxNode.cpp
// Desc : Node Module.
// Copyright(c) Project Asura. All right reserved.
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <asdxNode.h>


namespace asdx {

////////////////////////////////////////////////////////////////////////////////////
// Node class
////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------
//      コンストラクタです.
//----------------------------------------------------------------------------------
Node::Node( const u32 nodeID, Node* pParent )
: m_NodeID  ( nodeID )
, m_pParent ( pParent )
, m_Children()
{ /* DO_NOTHING */ }

//----------------------------------------------------------------------------------
//      デストラクタです.
//----------------------------------------------------------------------------------
Node::~Node()
{
    Release();
}

//----------------------------------------------------------------------------------
//      解放時の処理です.
//----------------------------------------------------------------------------------
void Node::OnRelease()
{
    if ( !m_Children.empty() )
    {
        for( u32 i=0; i<m_Children.size(); ++i )
        { ASDX_DELETE( m_Children[ i ] ); }
    }

    m_Children.clear();
    m_pParent = nullptr;
}

//----------------------------------------------------------------------------------
//      解放処理を行います.
//----------------------------------------------------------------------------------
void Node::Release()
{
    if ( !m_Children.empty() )
    {
        for( u32 i=0; i<m_Children.size(); ++i )
        {
            if ( m_Children[i] != nullptr )
            { m_Children[i]->Release(); }
        }
    }

    OnRelease();
}

//----------------------------------------------------------------------------------
//      更新時の処理です.
//----------------------------------------------------------------------------------
void Node::OnUpdate( NodeUpdateParam& param, void* pUser )
{
    ASDX_UNUSED_VAR( param );
    ASDX_UNUSED_VAR( pUser );
}

//----------------------------------------------------------------------------------
//      更新処理です.
//----------------------------------------------------------------------------------
void Node::Update( NodeUpdateParam& param, void* pUser )
{
    OnUpdate( param, pUser );

    if ( !m_Children.empty() )
    {
        for( u32 i=0; i<m_Children.size(); ++i )
        {
            if ( m_Children[i] != nullptr )
            { m_Children[i]->Update( param, pUser ); }
        }
    }
}

//----------------------------------------------------------------------------------
//      子供を持つかどうか.
//----------------------------------------------------------------------------------
bool Node::HasChild() const
{ return ( !m_Children.empty() && m_Children.size() >= 1 ); }

//----------------------------------------------------------------------------------
//      親を持つかどうか.
//----------------------------------------------------------------------------------
bool Node::HasParent() const
{ return ( m_pParent != nullptr ); }

//----------------------------------------------------------------------------------
//      親を取得する.
//----------------------------------------------------------------------------------
Node* Node::GetParent() const
{ return m_pParent; }

//----------------------------------------------------------------------------------
//      子供を取得する.
//----------------------------------------------------------------------------------
Node* Node::GetChild( u32 index ) const
{
    if ( HasChild() )
    {
        if ( index < m_Children.size() )
        { return m_Children[ index ]; }
    }

    return nullptr;
}

//----------------------------------------------------------------------------------
//      ノード識別番号を取得します.
//----------------------------------------------------------------------------------
const u32 Node::GetID() const
{ return m_NodeID; }

//----------------------------------------------------------------------------------
//      子供の数を取得する.
//----------------------------------------------------------------------------------
u32 Node::GetChildCount( bool isRecursive ) const
{
    u32 result = m_Children.size();

    if ( isRecursive )
    {
        for( u32 i=0; i<m_Children.size(); ++i )
        { result += m_Children[i]->GetChildCount(); }
    }

    return result;
}

//----------------------------------------------------------------------------------
//      親を設定する.
//----------------------------------------------------------------------------------
void Node::SetParent( Node* pParent )
{
    if ( m_pParent != nullptr )
    { m_pParent->DelChild( this ); }

    m_pParent = pParent;
}

//----------------------------------------------------------------------------------
//      子供を追加する.
//----------------------------------------------------------------------------------
bool Node::AddChild( Node* pChild )
{
    if ( pChild != nullptr )
    {
        if ( !pChild->HasParent() )
        { pChild->SetParent( this ); }

        m_Children.push_back( pChild );
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------------
//      子供を削除図する.
//----------------------------------------------------------------------------------
bool Node::DelChild( Node* pChild, bool isRecursive )
{
    if ( HasChild() && pChild != nullptr )
    {
        for( u32 i=0; i<m_Children.size(); ++i )
        {
            if ( m_Children[i] == pChild )
            {
                m_Children.erase( m_Children.begin() + i );
                return true;
            }
            else if ( isRecursive )
            {
                if ( m_Children[i]->DelChild( pChild, isRecursive ) )
                { return true; }
            }
        }
    }

    return false;
}

//----------------------------------------------------------------------------------
//      子供を探す.
//----------------------------------------------------------------------------------
Node* Node::FindChild( const u32 nodeID, bool isRecursive )
{
    Node* pResult = nullptr;

    if ( HasChild() )
    {
        for( u32 i=0; i<m_Children.size(); ++i )
        {
            if (  m_Children[i]->m_NodeID == nodeID )
            {
                pResult = m_Children[i];
                break;
            }
            else if ( isRecursive )
            {
                pResult = m_Children[i]->FindChild( nodeID, isRecursive );
                if ( pResult != nullptr )
                { break; }
            }
        }
    }

    return pResult;
}

//----------------------------------------------------------------------------------
//      親を探す.
//----------------------------------------------------------------------------------
Node* Node::FindParent( const u32 nodeID )
{
    if ( HasParent() )
    {
        if ( m_pParent->m_NodeID == nodeID )
        { return m_pParent; }
        else
        { return m_pParent->FindParent( nodeID ); }
    }

    return nullptr;
}


} // namespace asdx
