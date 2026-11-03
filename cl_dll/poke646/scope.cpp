/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <cstring>

#include "triangleapi.h"
#include "pm_shared.h"
#include "pm_defs.h"
#include "pmtrace.h"

#include "SDL2/SDL_video.h"

extern vec3_t v_origin;		// last view origin
extern vec3_t v_angles;		// last view angle

#define SCOPE_BORDER_SPRITE	"sprites/scopeborder.spr"

// Minimal OpenGL bits needed to change the wrap mode of the sprite textures.
#ifdef _WIN32
#define SCOPE_GLAPI __stdcall
#else
#define SCOPE_GLAPI
#endif

#define SCOPE_GL_TEXTURE_2D			0x0DE1
#define SCOPE_GL_TEXTURE_WRAP_S		0x2802
#define SCOPE_GL_TEXTURE_WRAP_T		0x2803
#define SCOPE_GL_CLAMP_TO_EDGE		0x812F

typedef void (SCOPE_GLAPI *pfnGlTexParameteri_t)(unsigned int target, unsigned int pname, int param);
typedef void (SCOPE_GLAPI *pfnGlGetTexParameteriv_t)(unsigned int target, unsigned int pname, int *params);

static pfnGlTexParameteri_t pfnGlTexParameteri;
static pfnGlGetTexParameteriv_t pfnGlGetTexParameteriv;

DECLARE_MESSAGE(m_Scope, Scope)

int CHudScope::Init(void)
{
	HOOK_MESSAGE(Scope);

	Reset();

	gHUD.AddHudElem(this);
	return 1;
}

void CHudScope::Reset(void)
{
	m_iFlags &= ~HUD_ACTIVE;
}

int CHudScope::VidInit(void)
{
	m_hSprite = SPR_Load(SCOPE_BORDER_SPRITE);

	pfnGlTexParameteri = (pfnGlTexParameteri_t)SDL_GL_GetProcAddress("glTexParameteri");
	pfnGlGetTexParameteriv = (pfnGlGetTexParameteriv_t)SDL_GL_GetProcAddress("glGetTexParameteriv");

	return 1;
}

int CHudScope::MsgFunc_Scope(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int fOn = READ_BYTE();

	if (fOn)
	{
		m_iFlags |= HUD_ACTIVE;
	}
	else
	{
		m_iFlags &= ~HUD_ACTIVE;
	}

	return 1;
}


int CHudScope::Draw(float flTime)
{
	return 1;
}

void CHudScope::DrawScopeBorder(int frame, int x, int y, int width, int height)
{
	if (!m_hSprite)
	{
		m_hSprite = SPR_Load(SCOPE_BORDER_SPRITE);
		if (!m_hSprite)
			return;
	}

	struct model_s* hSpriteModel = (struct model_s*)gEngfuncs.GetSpritePointer(m_hSprite);
	if (!hSpriteModel)
		return;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture); //additive

	gEngfuncs.pTriAPI->SpriteTexture(hSpriteModel, frame);
	gEngfuncs.pTriAPI->Color4f(1.0, 1.0, 1.0, 1.0);

	// The engine uploads sprite frames with GL_REPEAT, so bilinear filtering
	// at the edge of each quarter blends in the opposite (transparent) edge,
	// which shows up as seams between the quarters. Clamp while drawing.
	int oldWrapS = 0, oldWrapT = 0;
	bool clamp = pfnGlTexParameteri && pfnGlGetTexParameteriv;
	if (clamp)
	{
		pfnGlGetTexParameteriv(SCOPE_GL_TEXTURE_2D, SCOPE_GL_TEXTURE_WRAP_S, &oldWrapS);
		pfnGlGetTexParameteriv(SCOPE_GL_TEXTURE_2D, SCOPE_GL_TEXTURE_WRAP_T, &oldWrapT);
		pfnGlTexParameteri(SCOPE_GL_TEXTURE_2D, SCOPE_GL_TEXTURE_WRAP_S, SCOPE_GL_CLAMP_TO_EDGE);
		pfnGlTexParameteri(SCOPE_GL_TEXTURE_2D, SCOPE_GL_TEXTURE_WRAP_T, SCOPE_GL_CLAMP_TO_EDGE);
	}

	gEngfuncs.pTriAPI->CullFace(TRI_NONE);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);

	//top left
	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	gEngfuncs.pTriAPI->Vertex3f(x, y, 0);

	//bottom left
	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	gEngfuncs.pTriAPI->Vertex3f(x, y + height, 0);

	//bottom right
	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	gEngfuncs.pTriAPI->Vertex3f(x + width, y + height, 0);

	//top right
	gEngfuncs.pTriAPI->TexCoord2f(1, 0);
	gEngfuncs.pTriAPI->Vertex3f(x + width, y, 0);

	gEngfuncs.pTriAPI->End();

	if (clamp)
	{
		pfnGlTexParameteri(SCOPE_GL_TEXTURE_2D, SCOPE_GL_TEXTURE_WRAP_S, oldWrapS);
		pfnGlTexParameteri(SCOPE_GL_TEXTURE_2D, SCOPE_GL_TEXTURE_WRAP_T, oldWrapT);
	}

	gEngfuncs.pTriAPI->RenderMode(kRenderNormal); //return to normal
}

int CHudScope::DrawScope(void)
{
	if (!(m_iFlags & HUD_ACTIVE))
		return 1;

	// The scope was designed for 4:3. Lay it out relative to a centered 4:3
	// area so it looks the same on wider screens.
	int areaWidth = ScreenHeight * 4 / 3;
	if (areaWidth > ScreenWidth)
		areaWidth = ScreenWidth;
	int areaHeight = areaWidth * 3 / 4;
	int areaX = (ScreenWidth - areaWidth) / 2;
	int areaY = (ScreenHeight - areaHeight) / 2;

	// The original drew the scope 10 units in front of the view with a half
	// size of 11 * tan(fov / 2), i.e. 10% wider than the 4:3 screen.
	int size = areaWidth * 11 / 10;
	int halfSize = size / 2;
	size = halfSize * 2;

	int scopeX = (ScreenWidth - size) / 2;
	int scopeY = (ScreenHeight - size) / 2;

	// Top left scope border.
	DrawScopeBorder(0, scopeX, scopeY, halfSize, halfSize);

	// Top right scope border.
	DrawScopeBorder(1, scopeX + halfSize, scopeY, halfSize, halfSize);

	// Bottom left scope border.
	DrawScopeBorder(2, scopeX, scopeY + halfSize, halfSize, halfSize);

	// Bottom right scope border.
	DrawScopeBorder(3, scopeX + halfSize, scopeY + halfSize, halfSize, halfSize);

	// Black out the area outside the scope square. The outermost texels of the
	// sprite are partially transparent, so overlap the fill slightly into the
	// sprite's solid black border to hide the seam.
	int overlap = size / 128 + 1;

	if (scopeX >= 0)
	{
		int barWidth = scopeX + overlap;
		gEngfuncs.pfnFillRGBABlend(0, 0, barWidth, ScreenHeight, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(ScreenWidth - barWidth, 0, barWidth, ScreenHeight, 0, 0, 0, 255);
	}

	if (scopeY >= 0)
	{
		int barHeight = scopeY + overlap;
		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth, barHeight, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(0, ScreenHeight - barHeight, ScreenWidth, barHeight, 0, 0, 0, 255);
	}

	vec3_t forward;
	AngleVectors(v_angles, forward, NULL, NULL);
	VectorScale(forward, 8192, forward);
	VectorAdd(forward, v_origin, forward);
	pmtrace_t * trace = gEngfuncs.PM_TraceLine(v_origin, forward, PM_TRACELINE_PHYSENTSONLY, 2, -1);

	float dist = (trace->endpos - v_origin).Length();

	// 1 unit = 1 inch, as in the original.
	char szDistance[16] = {};
	if (dist < 118)
		std::strcpy(szDistance, "-.-- m");
	else
		std::snprintf(szDistance, sizeof(szDistance), "%.2f m", dist * 0.0254f);

	// The original right-aligns the text at 82% of the screen width, with its
	// bottom at 49% of the screen height.
	int textWidth = 0, textHeight = 0;
	GetConsoleStringSize(szDistance, &textWidth, &textHeight);

	int x = areaX + areaWidth * 82 / 100 - textWidth;
	int y = areaY + areaHeight * 49 / 100 - textHeight;

	// Draw distance.
	gEngfuncs.pfnDrawSetTextColor(1, 0.25, 0.25);
	DrawConsoleString(x, y, szDistance);

	return 1;
}
