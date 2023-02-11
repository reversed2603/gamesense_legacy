#pragma once
#include "IAppSystem.hpp"
#include "Definitions.hpp"

typedef unsigned long HFont;
typedef unsigned int VPANEL;

struct FontCharRenderInfo
{
	// Text pos
	int				x, y;
	// Top left and bottom right
	// This is now a pointer to an array maintained by the surface, to avoid copying the data on the 360
	void* verts;
	int				textureId;
	int				abcA;
	int				abcB;
	int				abcC;
	int				fontTall;
	void*	currentFont;
	// In:
	int	drawType;
	wchar_t			ch;

	// Out
	bool			valid;
	// In/Out (true by default)
	bool			shouldclip;
};

class ISurface : public IAppSystem
{
public:
	virtual void          RunFrame() = 0;
	virtual VPANEL		  GetEmbeddedPanel() = 0;
	virtual void          SetEmbeddedPanel(VPANEL pPanel) = 0;
	virtual void          PushMakeCurrent(VPANEL panel, bool useInsets) = 0;
	virtual void          PopMakeCurrent(VPANEL panel) = 0;
	virtual void          DrawSetColor(int r, int g, int b, int a) = 0;
	virtual void          DrawSetColor(Color col) = 0;
	virtual void          DrawFilledRect(int x0, int y0, int x1, int y1) = 0;
	virtual void          DrawFilledRectArray(IntRect *pRects, int numRects) = 0;
	virtual void          DrawOutlinedRect(int x0, int y0, int x1, int y1) = 0;
	virtual void          DrawLine(int x0, int y0, int x1, int y1) = 0;
	virtual void          DrawPolyLine(int *px, int *py, int numPoints) = 0;
	virtual void          DrawSetApparentDepth(float f) = 0;
	virtual void          DrawClearApparentDepth(void) = 0;
	virtual void          DrawSetTextFont(HFont font) = 0;
	virtual void          DrawSetTextColor(int r, int g, int b, int a) = 0;
	virtual void          DrawSetTextColor(Color col) = 0;
	virtual void          DrawSetTextPos(int x, int y) = 0;
	virtual void          DrawGetTextPos(int& x, int& y) = 0;
	virtual void          DrawPrintText(const wchar_t *text, int textLen, FontDrawType drawType = FontDrawType::FONT_DRAW_DEFAULT) = 0;
	virtual void          DrawUnicodeChar(wchar_t wch, FontDrawType drawType = FontDrawType::FONT_DRAW_DEFAULT) = 0;
	virtual void          DrawFlushText() = 0;
	virtual void*         CreateHTMLWindow(void *events, VPANEL context) = 0;
	virtual void          PaintHTMLWindow(void *htmlwin) = 0;
	virtual void          DeleteHTMLWindow(void *htmlwin) = 0;
	virtual int           DrawGetTextureId(char const *filename) = 0;
	virtual bool          DrawGetTextureFile(int id, char *filename, int maxlen) = 0;
	virtual void          DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload) = 0;
	virtual void          DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall) = 0;
	virtual void          DrawSetTexture(int id) = 0;
	virtual void          DeleteTextureByID(int id) = 0;
	virtual void          DrawGetTextureSize(int id, int &wide, int &tall) = 0;
	virtual void          DrawTexturedRect(int x0, int y0, int x1, int y1) = 0;
	virtual bool          IsTextureIDValid(int id) = 0;
	virtual int           CreateNewTextureID(bool procedural = false) = 0;
	virtual void          GetScreenSize(int &wide, int &tall) = 0;
	virtual void          SetAsTopMost(VPANEL panel, bool state) = 0;
	virtual void          BringToFront(VPANEL panel) = 0;
	virtual void          SetForegroundWindow(VPANEL panel) = 0;
	virtual void          SetPanelVisible(VPANEL panel, bool state) = 0;
	virtual void          SetMinimized(VPANEL panel, bool state) = 0;
	virtual bool          IsMinimized(VPANEL panel) = 0;
	virtual void          FlashWindow(VPANEL panel, bool state) = 0;
	virtual void          SetTitle(VPANEL panel, const wchar_t *title) = 0;
	virtual void          SetAsToolBar(VPANEL panel, bool state) = 0;
	virtual void          CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true, bool kbInput = true) = 0;
	virtual void          SwapBuffers(VPANEL panel) = 0;
	virtual void          Invalidate(VPANEL panel) = 0;
	virtual void          SetCursor(unsigned long cursor) = 0;
	virtual bool          IsCursorVisible() = 0;
	virtual void          ApplyChanges() = 0;
	virtual bool          IsWithin(int x, int y) = 0;
	virtual bool          HasFocus() = 0;
	virtual bool          SupportsFeature(int /*SurfaceFeature_t*/ feature) = 0;
	virtual void          RestrictPaintToSinglePanel(VPANEL panel, bool bForceAllowNonModalSurface = false) = 0;
	virtual void          SetModalPanel(VPANEL) = 0;
	virtual VPANEL		  GetModalPanel() = 0;
	virtual void          UnlockCursor() = 0;
	virtual void          LockCursor() = 0;
	virtual void          SetTranslateExtendedKeys(bool state) = 0;
	virtual VPANEL		  GetTopmostPopup() = 0;
	virtual void          SetTopLevelFocus(VPANEL panel) = 0;
	virtual HFont		  CreateFont_() = 0;
	virtual bool          SetFontGlyphSet(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin = 0, int nRangeMax = 0) = 0;
	virtual bool          AddCustomFontFile(const char *fontFileName) = 0;
	virtual int           GetFontTall(HFont font) = 0;
	virtual int           GetFontAscent(HFont font, wchar_t wch) = 0;
	virtual bool          IsFontAdditive(HFont font) = 0;
	virtual void          GetCharABCwide(HFont font, int ch, int &a, int &b, int &c) = 0;
	virtual int           GetCharacterWidth(HFont font, int ch) = 0;
	virtual void          GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall) = 0;
	virtual VPANEL		  GetNotifyPanel() = 0;
	virtual void          SetNotifyIcon(VPANEL context, unsigned long icon, VPANEL panelToReceiveMessages, const char *text) = 0;
	virtual void          PlaySound_(const char *fileName) = 0;
	virtual int           GetPopupCount() = 0;
	virtual VPANEL		  GetPopup(int index) = 0;
	virtual bool          ShouldPaintChildPanel(VPANEL childPanel) = 0;
	virtual bool          RecreateContext(VPANEL panel) = 0;
	virtual void          AddPanel(VPANEL panel) = 0;
	virtual void          ReleasePanel(VPANEL panel) = 0;
	virtual void          MovePopupToFront(VPANEL panel) = 0;
	virtual void          MovePopupToBack(VPANEL panel) = 0;
	virtual void          SolveTraverse(VPANEL panel, bool forceApplySchemeSettings = false) = 0;
	virtual void          PaintTraverse(VPANEL panel) = 0;
	virtual void          EnableMouseCapture(VPANEL panel, bool state) = 0;
	virtual void          GetWorkspaceBounds(int &x, int &y, int &wide, int &tall) = 0;
	virtual void          GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall) = 0;
	virtual void          GetProportionalBase(int &width, int &height) = 0;
	virtual void          CalculateMouseVisible() = 0;
	virtual bool          NeedKBInput() = 0;
	virtual bool          HasCursorPosFunctions() = 0;
	virtual void          SurfaceGetCursorPos(int &x, int &y) = 0;
	virtual void          SurfaceSetCursorPos(int x, int y) = 0;
	virtual void          DrawTexturedLine(const Vertex_t &a, const Vertex_t &b) = 0;
	virtual void          DrawOutlinedCircle(int x, int y, int radius, int segments) = 0;
	virtual void          DrawTexturedPolyLine(const Vertex_t *p, int n) = 0;
	virtual void          DrawTexturedSubRect(int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1) = 0;
	virtual void          DrawTexturedPolygon(int n, Vertex_t *pVertice, bool bClipVertices = true) = 0;

	virtual const wchar_t* GetTitle( VPANEL panel ) = 0;
	virtual bool IsCursorLocked( void ) const = 0;
	virtual void SetWorkspaceInsets( int left, int top, int right, int bottom ) = 0;

	// squarish comic book word bubble with pointer, rect params specify the space inside the bubble
	virtual void DrawWordBubble( int x0, int y0, int x1, int y1, int nBorderThickness, Color rgbaBackground, Color rgbaBorder,
		bool bPointer = false, int nPointerX = 0, int nPointerY = 0, int nPointerBaseThickness = 16 ) = 0;

	// Lower level char drawing code, call DrawGet then pass in info to DrawRender
	virtual bool DrawGetUnicodeCharRenderInfo( wchar_t ch, FontCharRenderInfo& info ) = 0;
	virtual void DrawRenderCharFromInfo( const FontCharRenderInfo& info ) = 0;

	// global alpha setting functions
	// affect all subsequent draw calls - shouldn't normally be used directly, only in Panel::PaintTraverse()
	virtual void DrawSetAlphaMultiplier( float alpha /* [0..1] */ ) = 0;
	virtual float DrawGetAlphaMultiplier( ) = 0;

	// web browser
	virtual void SetAllowHTMLJavaScript( bool state ) = 0;

	// video mode changing
	virtual void OnScreenSizeChanged( int nOldWidth, int nOldHeight ) = 0;

	virtual /*vgui::HCursor*/ void CreateCursorFromFile( char const* curOrAniFile, char const* pPathID = 0 ) = 0;

	// create IVguiMatInfo object ( IMaterial wrapper in VguiMatSurface, NULL in CWin32Surface )
	virtual /*IVguiMatInfo**/ void* DrawGetTextureMatInfoFactory( int id ) = 0;

	virtual void PaintTraverseEx( VPANEL panel, bool paintPopups = false ) = 0;

	virtual float GetZPos( ) const = 0;

	// From the Xbox
	virtual void SetPanelForInput( VPANEL vpanel ) = 0;
	virtual void DrawFilledRectFastFade( int x0, int y0, int x1, int y1, int fadeStartPt, int fadeEndPt, unsigned int alpha0, unsigned int alpha1, bool bHorizontal ) = 0;
	virtual void DrawFilledRectFade( int x0, int y0, int x1, int y1, unsigned int alpha0, unsigned int alpha1, bool bHorizontal ) = 0;

	// homo
	char pad27C[ 0x27C ];
	bool bClippingEnabled;

	void SetClipRect( int x0, int y0, int x1, int y1 ) {
		using Fn = void * ( __thiscall* )( void*, int, int, int, int );
		Memory::VCall<Fn>( this, 147 )( this, x0, y0, x1, y1 );
	}

	void UpdateTexture( uint32_t texture_handle, const uint8_t* texture_data, int data_width, int data_height ) {
		using Fn = void * ( __thiscall* )( void*, uint32_t, const uint8_t*, int, int );
		Memory::VCall<Fn>( this, 37 )( this, texture_handle, texture_data, data_width, data_height );
	}
};