#include "CDrawManager.h"
//===================================================================================
CDrawManager gDraw;

#define ESP_HEIGHT 14
//===================================================================================
void CDrawManager::Initialize( )
{
	if ( gInts.Surface == NULL )
		return;

	gInts.Engine->GetScreenSize( gScreenSize.iScreenWidth, gScreenSize.iScreenHeight );
	g_DefaultFont = gInts.Surface->CreateFont( );	//This is better than Arial
	gInts.Surface->SetFontGlyphSet( g_DefaultFont, "Tahoma", ESP_HEIGHT, 500, 0, 0, 0x200 );
}
//===================================================================================
Color CDrawManager::GetPlayerColor(CBaseEntity* pPlayer)
{
	if (pPlayer->GetIndex() == gCvars.iAimbotIndex)
		return Color::Green();
	else if (!gCvars.PlayerMode[pPlayer->GetIndex()])
		return Color(0, 255, 255, 255);
	else if (gCvars.PlayerMode[pPlayer->GetIndex()] == 2)
		return Color::Yellow();
	else
	{

		switch (pPlayer->GetTeamNum())
		{
		case 2: //RED
			return Color(255, 20, 20, 255);
		case 3: //BLU
			return Color(0, 153, 255, 255);
		default:
			return Color(0, 0, 0, 0);
		}
	}

	return Color(0, 0, 0, 0); //no reason for this to be here, i just wanna look smart
}
//===================================================================================
void CDrawManager::DrawString( int x, int y, Color clrColor, const wchar_t *pszText, HFont font, bool CenterX, bool CenterY)
{
	if( pszText == NULL )
		return;

	if (CenterX || CenterY)
	{
		int wide, tall;
		gInts.Surface->GetTextSize(font, pszText, wide, tall);
		if (CenterX)
			x -= wide / 2;
		if (CenterY)
			y -= tall / 2;
	}

	gInts.Surface->DrawSetTextPos( x, y );
	gInts.Surface->DrawSetTextFont( font );
	gInts.Surface->DrawSetTextColor( clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a() );
	gInts.Surface->DrawPrintText( pszText, wcslen( pszText ) );
}
//===================================================================================
void CDrawManager::DrawString( int x, int y, Color clrColor, const char *pszText, ... )
{
	if( pszText == NULL )
		return;

	va_list va_alist;
	char szBuffer[1024] = { '\0' };
	wchar_t szString[1024] = { '\0' };

	va_start( va_alist, pszText );
	vsprintf_s( szBuffer, pszText, va_alist );
	va_end( va_alist );

	wsprintfW( szString, L"%S", szBuffer );

	gInts.Surface->DrawSetTextPos( x, y );
	gInts.Surface->DrawSetTextFont( g_DefaultFont );
	gInts.Surface->DrawSetTextColor( clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a() );
	gInts.Surface->DrawPrintText( szString, wcslen( szString ) );
}
//===================================================================================
byte CDrawManager::GetESPHeight( )
{
	return ESP_HEIGHT;
}
//===================================================================================
void CDrawManager::DrawLine(int x, int y, int x1, int y1, Color clrColor)
{
	gInts.Surface->DrawSetColor(clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a());
	gInts.Surface->DrawLine(x, y, x1, y1);
}
//===================================================================================
void CDrawManager::DrawLineEx(int x, int y, int w, int h, Color clrColor)
{
	gInts.Surface->DrawSetColor(clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a());
	gInts.Surface->DrawLine(x, y, x + w, y + h);
}
//===================================================================================
void CDrawManager::DrawRect( int x, int y, int w, int h, Color clrColor)
{
	gInts.Surface->DrawSetColor( clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a() );
	gInts.Surface->DrawFilledRect( x, y, x + w, y + h );
}
//===================================================================================
void CDrawManager::OutlineRect( int x, int y, int w, int h, Color clrColor)
{
	gInts.Surface->DrawSetColor( clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a() );
	gInts.Surface->DrawOutlinedRect( x, y, x + w, y + h );
}
//===================================================================================
void CDrawManager::DrawBox( Vector vOrigin, int r, int g, int b, int alpha, int box_width, int radius )
{
	Vector vScreen;

	if( !WorldToScreen( vOrigin, vScreen ) )
		return;

	int radius2 = radius<<1;

	OutlineRect( vScreen.x - radius + box_width, vScreen.y - radius + box_width, radius2 - box_width, radius2 - box_width, Color::Black() );
	OutlineRect( vScreen.x - radius - 1, vScreen.y - radius - 1, radius2 + ( box_width + 2 ), radius2 + ( box_width + 2 ), Color::Black() );
	DrawRect( vScreen.x - radius + box_width, vScreen.y - radius, radius2 - box_width, box_width, Color( r, g, b, alpha ));
	DrawRect( vScreen.x - radius, vScreen.y + radius, radius2, box_width, Color( r, g, b, alpha ));
	DrawRect( vScreen.x - radius, vScreen.y - radius, box_width, radius2, Color( r, g, b, alpha ));
	DrawRect( vScreen.x + radius, vScreen.y - radius, box_width, radius2 + box_width, Color( r, g, b, alpha ) );
}
//===================================================================================
bool CDrawManager::WorldToScreen( Vector &vOrigin, Vector &vScreen )
{
	const matrix3x4& worldToScreen = gInts.Engine->WorldToScreenMatrix(); //Grab the world to screen matrix from CEngineClient::WorldToScreenMatrix

	float w = worldToScreen[3][0] * vOrigin[0] + worldToScreen[3][1] * vOrigin[1] + worldToScreen[3][2] * vOrigin[2] + worldToScreen[3][3]; //Calculate the angle in compareson to the player's camera.
	vScreen.z = 0; //Screen doesn't have a 3rd dimension.

	if( w > 0.001 ) //If the object is within view.
	{
		float fl1DBw = 1 / w; //Divide 1 by the angle.
		vScreen.x = (gScreenSize.iScreenWidth / 2) + ( 0.5 * ((worldToScreen[0][0] * vOrigin[0] + worldToScreen[0][1] * vOrigin[1] + worldToScreen[0][2] * vOrigin[2] + worldToScreen[0][3]) * fl1DBw) * gScreenSize.iScreenWidth + 0.5); //Get the X dimension and push it in to the Vector.
		vScreen.y = (gScreenSize.iScreenHeight / 2) - ( 0.5 * ((worldToScreen[1][0] * vOrigin[0] + worldToScreen[1][1] * vOrigin[1] + worldToScreen[1][2] * vOrigin[2] + worldToScreen[1][3]) * fl1DBw) * gScreenSize.iScreenHeight + 0.5); //Get the Y dimension and push it in to the Vector.
		return true;
	}

	return false;
}

void CDrawManager::DrawCrosshair(int iValue)
{
	int m_iScreenWidth = gScreenSize.iScreenWidth;
	int     m_iScreenHeight = gScreenSize.iScreenHeight;

	int x = m_iScreenWidth / 2;
	int y = m_iScreenHeight / 2;
	switch (iValue)
	{
	case 1:
		DrawRect(x - 14, y, 9, 1, Color(255, 255, 255, 255));
		DrawRect(x + 5, y, 9, 1, Color(255, 255, 255, 255));
		DrawRect(x, y - 14, 1, 9, Color(255, 255, 255, 255));
		DrawRect(x, y + 5, 1, 9, Color(255, 255, 255, 255));
		DrawRect(x, y, 1, 1, Color(255, 255, 255, 255));
	break;
	}

}

void CDrawManager::DrawCornerBox(int x, int y, int w, int h, int cx, int cy, Color Col)
{
	gDraw.DrawLine(x, y, x + (w / cx), y, Col);
	gDraw.DrawLine(x, y, x, y + (h / cy), Col);

	gDraw.DrawLine(x + w, y, x + w - (w / cx), y, Col);
	gDraw.DrawLine(x + w, y, x + w, y + (h / cy), Col);

	gDraw.DrawLine(x, y + h, x + (w / cx), y + h, Col);
	gDraw.DrawLine(x, y + h, x, y + h - (h / cy), Col);

	gDraw.DrawLine(x + w, y + h, x + w - (w / cx), y + h, Col);
	gDraw.DrawLine(x + w, y + h, x + w, y + h - (h / cy), Col);

}