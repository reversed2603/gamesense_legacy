#pragma once
#include "sdk.hpp"

class ITraceFilter;

class Beam_t
{
public:
  Beam_t();

  // Methods of IClientRenderable
  virtual const Vector&	GetRenderOrigin(void) = 0;
  virtual const QAngle&	GetRenderAngles(void) = 0;
  virtual const matrix3x4_t &RenderableToWorldTransform() = 0;
  virtual void			GetRenderBounds(Vector& mins, Vector& maxs) = 0;
  virtual bool			ShouldDraw(void) = 0;
  virtual bool			IsTransparent(void) = 0;
  virtual int				DrawModel(int flags) = 0;
  virtual void			ComputeFxBlend() = 0;
  virtual int				GetFxBlend() = 0;

  // Resets the beam state
  void			Reset();

  // Method to computing the bounding box
  void			ComputeBounds();

  // Bounding box...
  Vector			m_Mins;
  Vector			m_Maxs;
  pixelvis_handle_t *m_queryHandleHalo;
  float			m_haloProxySize;

  // Data is below..

  // Next beam in list
  Beam_t*			next;

  // Type of beam
  int				type;
  int				flags;

  // Control points for the beam
  int				numAttachments;
  Vector			attachment[MAX_BEAM_ENTS];
  Vector			delta;

  // 0 .. 1 over lifetime of beam
  float			t;
  float			freq;

  // Time when beam should die
  float			die;
  float			width;
  float			endWidth;
  float			fadeLength;
  float			amplitude;
  float			life;

  // Color
  float			r, g, b;
  float			brightness;

  // Speed
  float			speed;

  // Animation
  float			frameRate;
  float			frame;
  int				segments;

  // Attachment entities for the beam
  HANDLE			entity[MAX_BEAM_ENTS];
  int				attachmentIndex[MAX_BEAM_ENTS];

  // Model info
  int				modelIndex;
  int				haloIndex;

  float			haloScale;
  int				frameCount;

  float			rgNoise[NOISE_DIVISIONS + 1];

  // Popcorn trail for beam follows to use
  BeamTrail_t*	trail;

  // for TE_BEAMRINGPOINT
  float			start_radius;
  float			end_radius;

  // for FBEAM_ONLYNOISEONCE
  bool			m_bCalculatedNoise;

  float			m_flHDRColorScale;

};

class IViewRenderBeams
{
public:
  virtual void	InitBeams(void) = 0;
  virtual void	ShutdownBeams(void) = 0;
  virtual void	ClearBeams(void) = 0;

  // Updates the state of the temp ent beams
  virtual void	UpdateTempEntBeams() = 0;

  virtual void	DrawBeam(C_Beam* pbeam, ITraceFilter *pEntityBeamTraceFilter = NULL) = 0;
  virtual void	DrawBeam(Beam_t *pbeam) = 0;

  virtual void	KillDeadBeams(C_BaseEntity *pEnt) = 0;

  // New interfaces!
  virtual Beam_t	*CreateBeamEnts(BeamInfo_t &beamInfo) = 0;
  virtual Beam_t	*CreateBeamEntPoint(BeamInfo_t &beamInfo) = 0;
  virtual	Beam_t	*CreateBeamPoints(BeamInfo_t &beamInfo) = 0;
  virtual Beam_t	*CreateBeamRing(BeamInfo_t &beamInfo) = 0;
  virtual Beam_t	*CreateBeamRingPoint(BeamInfo_t &beamInfo) = 0;
  virtual Beam_t	*CreateBeamCirclePoints(BeamInfo_t &beamInfo) = 0;
  virtual Beam_t	*CreateBeamFollow(BeamInfo_t &beamInfo) = 0;

  virtual void	FreeBeam(Beam_t *pBeam) = 0;
  virtual void	UpdateBeamInfo(Beam_t *pBeam, BeamInfo_t &beamInfo) = 0;

  // These will go away!
  virtual void	CreateBeamEnts(int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
	 float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
	 float brightness, float speed, int startFrame,
	 float framerate, float r, float g, float b, int type = -1) = 0;
  virtual void	CreateBeamEntPoint(int	nStartEntity, const Vector *pStart, int nEndEntity, const Vector* pEnd,
	 int modelIndex, int haloIndex, float haloScale,
	 float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
	 float brightness, float speed, int startFrame,
	 float framerate, float r, float g, float b) = 0;
  virtual void	CreateBeamPoints(Vector& start, Vector& end, int modelIndex, int haloIndex, float haloScale,
	 float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
	 float brightness, float speed, int startFrame,
	 float framerate, float r, float g, float b) = 0;
  virtual void	CreateBeamRing(int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
	 float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
	 float brightness, float speed, int startFrame,
	 float framerate, float r, float g, float b, int flags = 0) = 0;
  virtual void	CreateBeamRingPoint(const Vector& center, float start_radius, float end_radius, int modelIndex, int haloIndex, float haloScale,
	 float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
	 float brightness, float speed, int startFrame,
	 float framerate, float r, float g, float b, int flags = 0) = 0;
  virtual void	CreateBeamCirclePoints(int type, Vector& start, Vector& end,
	 int modelIndex, int haloIndex, float haloScale, float life, float width,
	 float m_nEndWidth, float m_nFadeLength, float amplitude, float brightness, float speed,
	 int startFrame, float framerate, float r, float g, float b) = 0;
  virtual void	CreateBeamFollow(int startEnt, int modelIndex, int haloIndex, float haloScale,
	 float life, float width, float m_nEndWidth, float m_nFadeLength, float r, float g, float b,
	 float brightness) = 0;
};
