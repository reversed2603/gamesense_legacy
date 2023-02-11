#pragma once
struct vrect_t;
class IViewRender {
public:
   virtual void                Init( void ) = 0;
   virtual void                LevelInit( void ) = 0;
   virtual void                LevelShutdown( void ) = 0;
   virtual void                Shutdown( void ) = 0;
   virtual void                OnRenderStart( ) = 0;
   virtual	void                Render( vrect_t* rect ) = 0;
   virtual void                RenderView( const CViewSetup& view, int nClearFlags, int whatToDraw ) = 0;
   virtual int                 GetDrawFlags( ) = 0;
   virtual void                StartPitchDrift( void ) = 0;
   virtual void                StopPitchDrift( void ) = 0;
   virtual void* GetFrustum( ) = 0;
   virtual bool                ShouldDrawBrushModels( void ) = 0;
   virtual const CViewSetup* GetPlayerViewSetup( void ) const = 0;
   virtual const CViewSetup* GetViewSetup( void ) const = 0;
   virtual void                DisableVis( void ) = 0;
   virtual int                 BuildWorldListsNumber( ) const = 0;
   virtual void                SetCheapWaterStartDistance( float flCheapWaterStartDistance ) = 0;
   virtual void                SetCheapWaterEndDistance( float flCheapWaterEndDistance ) = 0;
   virtual void                GetWaterLODParams( float& flCheapWaterStartDistance, float& flCheapWaterEndDistance ) = 0;
   virtual void                DriftPitch( void ) = 0;
   virtual void                SetScreenOverlayMaterial( IMaterial* pMaterial ) = 0;
   virtual IMaterial* GetScreenOverlayMaterial( ) = 0;
   virtual void                WriteSaveGameScreenshot( const char* pFilename ) = 0;
   virtual void                WriteSaveGameScreenshotOfSize( const char* pFilename, int width, int height ) = 0;
   virtual void                QueueOverlayRenderView( const CViewSetup& view, int nClearFlags, int whatToDraw ) = 0;
   virtual float               GetZNear( ) = 0;
   virtual float               GetZFar( ) = 0;
   virtual void                GetScreenFadeDistances( float* min, float* max ) = 0;
   virtual C_BaseEntity* GetCurrentlyDrawingEntity( ) = 0;
   virtual void               SetCurrentlyDrawingEntity( C_BaseEntity* pEnt ) = 0;
};