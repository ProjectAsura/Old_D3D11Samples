﻿//-------------------------------------------------------------------------------------------------
// File : asdxApp.h
// Desc : Application Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

#ifndef __ASDX_APP_H__
#define __ASDX_APP_H__

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <d3d11.h>
#include <dxgi.h>
#include <Windows.h>
#include <asdxTypedef.h>
#include <asdxRef.h>
#include <asdxTarget.h>
#include <asdxTimer.h>
#include <asdxHid.h>
#if defined(DEBUG) || defined(_DEBUG)
#include <DXGIDebug.h>
#endif

//-------------------------------------------------------------------------------------------------
// Linker
//-------------------------------------------------------------------------------------------------
#ifdef ASDX_AUTO_LINK
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "comctl32.lib" )
#endif//ASDX_AUTO_LINK


namespace asdx {

///////////////////////////////////////////////////////////////////////////////////////////////////
// MouseEventArgs structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct MouseEventArgs
{
    s32     X;                  //!< カーソルのX座標です.
    s32     Y;                  //!< カーソルのY座標です.
    s32     WheelDelta;         //!< マウスホイールの移動方向です.
    bool    IsLeftButtonDown;   //!< 左ボタンが押されたどうかを示すフラグです.
    bool    IsRightButtonDown;  //!< 右ボタンが押されたどうかを示すフラグです.
    bool    IsMiddleButtonDown; //!< 中ボタンが押されたかどうかを示すフラグです.
    bool    IsSideButton1Down;  //!< X1ボタンが押されたかどうかを示すフラグです.
    bool    IsSideButton2Down;  //!< X2ボタンが押されたかどうかを示すフラグです.

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    MouseEventArgs()
    : X                 ( 0 )
    , Y                 ( 0 )
    , WheelDelta        ( 0 )
    , IsLeftButtonDown  ( false )
    , IsRightButtonDown ( false)
    , IsMiddleButtonDown( false )
    , IsSideButton1Down ( false )
    , IsSideButton2Down ( false )
    { /* DO_NOTHING */ }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// KeyEventArgs structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct KeyEventArgs
{
    u32     KeyCode;        //!< キーコードです.
    bool    IsKeyDown;      //!< キーが押されたかどうかを示すフラグです.
    bool    IsAltDown;      //!< ALTキーが押されたかどうかを示すフラグです.

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    KeyEventArgs()
    : KeyCode   ( 0 )
    , IsKeyDown ( false )
    , IsAltDown ( false )
    { /* DO_NOTHING */ }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// ResizeEventArgs structure
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ResizeEventArgs
{
    u32    Width;          //!< 画面の横幅です.
    u32    Height;         //!< 画面の縦幅です.
    f32    AspectRatio;    //!< 画面のアスペクト比です.

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    ResizeEventArgs()
    : Width      ( 0 )
    , Height     ( 0 )
    , AspectRatio( 0.0f )
    { /* DO_NOTHING */ }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// FrameEventArgs struture
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FrameEventArgs
{
    ID3D11DeviceContext*    pDeviceContext; //!< デバイスコンテキストです.
    f64                     Time;           //!< アプリケーション開始からの相対時間です.
    f64                     ElapsedTime;    //!< 前のフレームからの経過時間(秒)です.
    f32                     FPS;            //!< １秒当たりフレーム更新回数です.
    bool                    IsStopDraw;     //!< 描画停止フラグです.

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    FrameEventArgs()
    : pDeviceContext( nullptr )
    , Time          ( 0 )
    , ElapsedTime   ( 0 )
    , FPS           ( 0.0f )
    , IsStopDraw    ( false )
    { /* DO_NOTHING */ }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Application class
///////////////////////////////////////////////////////////////////////////////////////////////////
class Application : private NonCopyable
{
    //=============================================================================================
    // list of friend classes and methods.
    //=============================================================================================
    /* NOTHING */

public:
    //=============================================================================================
    // public variables.
    //=============================================================================================
    /* NOTHING */

    //=============================================================================================
    // public methods.
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------------
    Application();

    //---------------------------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //!
    //! @param [in]     title       タイトル名.
    //! @param [in]     width       画面の横幅.
    //! @param [in]     height      画面の縦幅.
    //! @param [in]     hIcon       アイコンハンドル.
    //! @param [in]     hMenu       メニューハンドル.
    //! @param [in]     hAccel      アクセレレータハンドル.
    //---------------------------------------------------------------------------------------------
    Application( LPWSTR title, u32 width, u32 height, HICON hIcon, HMENU hMenu, HACCEL hAccel );

    //---------------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------------
    virtual ~Application();

    //---------------------------------------------------------------------------------------------
    //! @brief      アプリケーションを実行します.
    //---------------------------------------------------------------------------------------------
    void    Run();

    //---------------------------------------------------------------------------------------------
    //! @brief      フォーカスを持つかどうか判定します.
    //!
    //! @retval true    フォーカスを持ちます.
    //! @retval false   フォーカスを持ちません.
    //---------------------------------------------------------------------------------------------
    bool HasFocus() const;

protected:
    //=============================================================================================
    // protected variables
    //=============================================================================================
    HINSTANCE                       m_hInst;                //!< インスタンスハンドルです.
    HWND                            m_hWnd;                 //!< ウィンドウハンドルです.
    D3D_DRIVER_TYPE                 m_DriverType;           //!< ドライバータイプです.
    D3D_FEATURE_LEVEL               m_FeatureLevel;         //!< 機能レベルです.
    u32                             m_MultiSampleCount;     //!< マルチサンプリングのカウント数です.
    u32                             m_MultiSampleQuality;   //!< マルチサンプリングの品質値です.
    u32                             m_SwapChainCount;       //!< スワップチェインのバッファ数です.
    DXGI_FORMAT                     m_SwapChainFormat;      //!< スワップチェインのバッファフォーマットです.
    DXGI_FORMAT                     m_DepthStencilFormat;   //!< 深度ステンシルバッファのフォーマットです.
    RefPtr<ID3D11Device>            m_pDevice;              //!< デバイスです.
    RefPtr<ID3D11DeviceContext>     m_pDeviceContext;       //!< デバイスコンテキスト(イミディエイトコンテキスト)です.
    RefPtr<IDXGISwapChain>          m_pSwapChain;           //!< スワップチェインです.
    ColorTarget2D                   m_ColorTarget2D;        //!< 描画ターゲットです.
    DepthTarget2D                   m_DepthTarget2D;        //!< 深度ステンシルバッファです.
    RefPtr<ID3D11RasterizerState>   m_pRS;                  //!< ラスタライザーステートです.
    RefPtr<ID3D11DepthStencilState> m_pDSS;                 //!< 深度ステンシルステートです.
    RefPtr<ID3D11BlendState>        m_pBS;                  //!< ブレンドステートです.
    f32                             m_BlendFactor[ 4 ];     //!< ブレンドファクターです.
    u32                             m_SampleMask;           //!< サンプルマスクです.
    u32                             m_StencilRef;           //!< ステンシル参照です.
    f32                             m_ClearColor[ 4 ];      //!< 背景のクリアカラーです.
    u32                             m_Width;                //!< 画面の横幅です.
    u32                             m_Height;               //!< 画面の縦幅です.
    f32                             m_AspectRatio;          //!< 画面のアスペクト比です.
    LPWSTR                          m_Title;                //!< アプリケーションのタイトル名です.
    Timer                           m_Timer;                //!< タイマーです.
    D3D11_VIEWPORT                  m_Viewport;             //!< ビューポートです.
    D3D11_RECT                      m_ScissorRect;          //!< シザー矩形です.
    HICON                           m_hIcon;                //!< アイコンハンドルです.
    HMENU                           m_hMenu;                //!< メニューハンドルです.
    HACCEL                          m_hAccel;               //!< アクセレレータハンドルです.

#if ASDX_IS_DEBUG
    RefPtr<ID3D11Debug>             m_pD3D11Debug;          //!< デバッグオブジェクトです.
#endif//ASDX_IS_DEBUG

    //=============================================================================================
    // protected methods.
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    //! @brief      初期化時に実行する処理です.
    //!
    //! @note       派生クラスにて実装を行います.
    //---------------------------------------------------------------------------------------------
    virtual bool OnInit       ();

    //---------------------------------------------------------------------------------------------
    //! @brief      終了時に実行する処理です.
    //!
    //! @note       派生クラスにて実装を行います.
    //---------------------------------------------------------------------------------------------
    virtual void OnTerm       ();

    //---------------------------------------------------------------------------------------------
    //! @brief      フレーム遷移時に実行する処理です.
    //!
    //! @param [in]     param       フレームインベントパラメータです.
    //! @note       派生クラスにて実装を行います.
    //---------------------------------------------------------------------------------------------
    virtual void OnFrameMove  ( FrameEventArgs& param );

    //---------------------------------------------------------------------------------------------
    //! @brief      フレーム描画時に実行する処理です.
    //!
    //! @param [in]     param       フレームインベントパラメータです.
    //! @note       派生クラスにて実装を行います.
    //---------------------------------------------------------------------------------------------
    virtual void OnFrameRender( FrameEventArgs& param );

    //---------------------------------------------------------------------------------------------
    //! @brief      リサイズ時に実行する処理です.
    //!
    //! @param [in]     param       リサイズイベントパラメータです.
    //! @note       派生クラスにて実装を行います.
    //---------------------------------------------------------------------------------------------
    virtual void OnResize     ( const ResizeEventArgs& param );

    //---------------------------------------------------------------------------------------------
    //! @brief      キーイベント通知時に実行する処理です.
    //!
    //! @param [in]     param       キーイベントパラメータです.
    //! @note       派生クラスにて実装を行います.
    //---------------------------------------------------------------------------------------------
    virtual void OnKey        ( const KeyEventArgs&    param );

    //---------------------------------------------------------------------------------------------
    //! @brief      マウスイベント通知時に実行する処理です.
    //!
    //! @param [in]     param       マウスイベントパラメータです.
    //! @note       派生クラスにて実装を行います.
    //---------------------------------------------------------------------------------------------
    virtual void OnMouse      ( const MouseEventArgs&  param );

    //---------------------------------------------------------------------------------------------
    //! @brief      ウィンドウへのドラッグアンドドロップされたと時に実行する処理です.
    //!
    //! @param[in]      dropFiles     ドラッグアンドドロップされたファイル名です.
    //---------------------------------------------------------------------------------------------
    virtual void OnDrop( const char16* dropFiles );

    //---------------------------------------------------------------------------------------------
    //! @brief      メッセージプロシージャの処理です.
    //!
    //! @param[in]      hWnd        ウィンドウハンドル.
    //! @param[in]      msg         メッセージです.
    //! @param[in]      wp          メッセージの追加情報.
    //! @param[in]      lp          メッセージの追加情報.
    //---------------------------------------------------------------------------------------------
    virtual void OnMsgProc( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp );

    //---------------------------------------------------------------------------------------------
    //! @brief      描画停止フラグを設定します.
    //!
    //! @param [in]     isStopRendering     描画を停止するかどうか.停止する場合はtrueを指定します.
    //---------------------------------------------------------------------------------------------
    void SetStopRendering( bool isStopRendering );

    //---------------------------------------------------------------------------------------------
    //! @brief      描画停止フラグを取得します.
    //!
    //! @retval true    描画処理を呼び出しません.
    //! @retval false   描画処理を呼び出します.
    //---------------------------------------------------------------------------------------------
    bool IsStopRendering() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      フレームカウントを取得します.
    //!
    //! @return     フレームカウントを返却します.
    //---------------------------------------------------------------------------------------------
    DWORD GetFrameCount() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      FPSを取得します.
    //!
    //! @return     0.5秒ごとに更新されるFPSを返却します.
    //! @note       各フレームにおけるFPSを取得する場合は FrameEventArgs から取得します.
    //---------------------------------------------------------------------------------------------
    f32 GetFPS() const;

    //---------------------------------------------------------------------------------------------
    //! @brief      コマンドを実行して，画面に表示します.
    //!
    //! @param [in]     syncInterval        垂直同期の間隔です.
    //---------------------------------------------------------------------------------------------
    void Present( u32 syncInterval );

private:
    //=============================================================================================
    // private variables.
    //=============================================================================================
    bool    m_IsStopRendering;      //!< 描画を停止するかどうかのフラグ. 停止する場合はtrueを指定.
    bool    m_IsStandbyMode;        //!< スタンバイモードかどうかを示すフラグです.
    DWORD   m_FrameCount;           //!< フレームカウントです.
    f32     m_FPS;                  //!< FPS(1秒あたりのフレーム描画回数)です.
    f64     m_LatestUpdateTime;     //!< 最後の更新時間です.

    //=============================================================================================
    // private methods.
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    //! @brief      アプリケーションの初期化処理です.
    //!
    //! @retval true    初期化に成功.
    //! @retval false   初期化に失敗.
    //! @note       このメソッドは内部処理で，InitWnd(), InitD3D()を呼び出します.
    //---------------------------------------------------------------------------------------------
    bool InitApp();

    //---------------------------------------------------------------------------------------------
    //! @brief      アプリケーションの終了処理です.
    //!
    //! @note       このメソッドは内部処理で, TermD3D(), TermWnd()を呼び出します.
    //---------------------------------------------------------------------------------------------
    void TermApp();

    //---------------------------------------------------------------------------------------------
    //! @brief      ウィンドウの初期化処理です.
    //!
    //! @retval true    初期化に成功.
    //! @retval false   初期化に失敗.
    //---------------------------------------------------------------------------------------------
    bool InitWnd();

    //---------------------------------------------------------------------------------------------
    //! @brief      ウィンドウの終了処理です.
    //---------------------------------------------------------------------------------------------
    void TermWnd();

    //---------------------------------------------------------------------------------------------
    //! @brief      Direct3Dの初期化処理です.
    //!
    //! @retval true    初期化に成功.
    //! @retval false   初期化に失敗.
    //---------------------------------------------------------------------------------------------
    bool InitD3D();

    //---------------------------------------------------------------------------------------------
    //! @brief      Direct3Dの終了処理です.
    //---------------------------------------------------------------------------------------------
    void TermD3D();

    //---------------------------------------------------------------------------------------------
    //! @brief      メインループ処理です.
    //---------------------------------------------------------------------------------------------
    void MainLoop();

    //---------------------------------------------------------------------------------------------
    //! @brief      キーイベントを処理します.
    //!
    //! @param[in]      param       キーイベント引数です.
    //! @note       このメソッドは内部処理で，OnKey()を呼び出します.
    //!             また，このメソッドはウィンドウプロシージャからのアクセス専用メソッドですので,
    //!             アプリケーション側で呼び出しを行わないでください.
    //---------------------------------------------------------------------------------------------
    void KeyEvent   ( const KeyEventArgs&    param );

    //---------------------------------------------------------------------------------------------
    //! @brief      リサイズイベントを処理します.
    //!
    //! @param[in]      param       リサイズイベント引数です.
    //! @note       このメソッドは内部処理で, OnResize()を呼び出します.
    //!             また，このメソッドはウィンドウプロシージャからのアクセス専用メソッドですので,
    //!             アプリケーション側で呼び出しを行わないでください.
    //---------------------------------------------------------------------------------------------
    void ResizeEvent( const ResizeEventArgs& param );

    //---------------------------------------------------------------------------------------------
    //! @brief      マウスイベントを処理します.
    //!
    //! @param[in]      param       マウスイベント引数です.
    //! @note       このメソッドは内部処理で, OnMouse()を呼び出します.
    //!             また，このメソッドはウィンドウプロシージャからのアクセス専用メソッドですので,
    //!             アプリケーション側で呼び出しを行わないでください.
    //---------------------------------------------------------------------------------------------
    void MouseEvent( const MouseEventArgs&  param );

    //---------------------------------------------------------------------------------------------
    //! @brief      ドロップイベントを処理します.
    //!
    //! @param[in]      dropFile         ドロップされたファイルパスです.
    //---------------------------------------------------------------------------------------------
    void DropEvent( const char16* dropFile );

    //---------------------------------------------------------------------------------------------
    //! @brief      ウィンドウプロシージャです.
    //!
    //! @param [in]     hWnd        ウィンドウハンドル.
    //! @param [in]     uMsg        メッセージ.
    //! @param [in]     wp          メッセージの追加情報.
    //! @param [in]     lp          メッセージの追加情報.
    //---------------------------------------------------------------------------------------------
    static LRESULT CALLBACK MsgProc( HWND hWnd, u32 uMsg, WPARAM wp, LPARAM lp );
};


} // namespace asdx

#endif//__ASDX_APP_H__
