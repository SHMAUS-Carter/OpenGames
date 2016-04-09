// cl.input.c  -- builds an intended movement command to send to the server

// leave this as first line for PCH reasons...
//
#include "../server/exe_headers.h"


#include "client.h"
#include "client_ui.h"

unsigned	frame_msec;
int			old_com_frameTime;

/*
===============================================================================

KEY BUTTONS

Continuous button event tracking is complicated by the fact that two different
input sources (say, mouse button 1 and the control key) can both press the
same button, but the button should only be released when both of the
pressing key have been released.

When a key event issues a button command (+forward, +attack, etc), it appends
its key number as argv(1) so it can be matched up with the release.

argv(2) will be set to the time the event happened, which allows exact
control even at low framerates when the down and up events may both get qued
at the same time.

===============================================================================
*/


kbutton_t	in_left, in_right, in_forward, in_back;
kbutton_t	in_lookup, in_lookdown, in_moveleft, in_moveright;
kbutton_t	in_strafe, in_speed;
kbutton_t	in_up, in_down;

kbutton_t	in_buttons[8];


qboolean	in_mlooking;


void IN_MLookDown( void ) {
	in_mlooking = qtrue;
}

void IN_MLookUp( void ) {
	in_mlooking = qfalse;
	if ( !cl_freelook->integer ) {
		IN_CenterView ();
	}
}

void IN_KeyDown( kbutton_t *b ) {
	int		k;
	char	*c;
	
	c = Cmd_Argv(1);
	if ( c[0] ) {
		k = atoi(c);
	} else {
		k = -1;		// typed manually at the console for continuous down
	}

	if ( k == b->down[0] || k == b->down[1] ) {
		return;		// repeating key
	}
	
	if ( !b->down[0] ) {
		b->down[0] = k;
	} else if ( !b->down[1] ) {
		b->down[1] = k;
	} else {
		Com_Printf ("Three keys down for a button!\n");
		return;
	}
	
	if ( b->active ) {
		return;		// still down
	}

	// save timestamp for partial frame summing
	c = Cmd_Argv(2);
	b->downtime = atoi(c);

	b->active = qtrue;
	b->wasPressed = qtrue;
}

void IN_KeyUp( kbutton_t *b ) {
	int		k;
	char	*c;
	unsigned	uptime;

	c = Cmd_Argv(1);
	if ( c[0] ) {
		k = atoi(c);
	} else {
		// typed manually at the console, assume for unsticking, so clear all
		b->down[0] = b->down[1] = 0;
		b->active = qfalse;
		return;
	}

	if ( b->down[0] == k ) {
		b->down[0] = 0;
	} else if ( b->down[1] == k ) {
		b->down[1] = 0;
	} else {
		return;		// key up without coresponding down (menu pass through)
	}
	if ( b->down[0] || b->down[1] ) {
		return;		// some other key is still holding it down
	}

	b->active = qfalse;

	// save timestamp for partial frame summing
	c = Cmd_Argv(2);
	uptime = atoi(c);
	if ( uptime ) {
		b->msec += uptime - b->downtime;
	} else {
		b->msec += frame_msec / 2;
	}

	b->active = qfalse;
}



/*
===============
CL_KeyState

Returns the fraction of the frame that the key was down
===============
*/
float CL_KeyState( kbutton_t *key ) {
	float		val;
	int			msec;

	msec = key->msec;
	key->msec = 0;

	if ( key->active ) {
		// still down
		if ( !key->downtime ) {
			msec = com_frameTime;
		} else {
			msec += com_frameTime - key->downtime;
		}
		key->downtime = com_frameTime;
	}

#if 0
	if (msec) {
		Com_Printf ("%i ", msec);
	}
#endif

	val = (float)msec / frame_msec;
	if ( val < 0 ) {
		val = 0;
	}
	if ( val > 1 ) {
		val = 1;
	}

	return val;
}



void IN_UpDown(void) {IN_KeyDown(&in_up);}
void IN_UpUp(void) {IN_KeyUp(&in_up);}
void IN_DownDown(void) {IN_KeyDown(&in_down);}
void IN_DownUp(void) {IN_KeyUp(&in_down);}
void IN_LeftDown(void) {IN_KeyDown(&in_left);}
void IN_LeftUp(void) {IN_KeyUp(&in_left);}
void IN_RightDown(void) {IN_KeyDown(&in_right);}
void IN_RightUp(void) {IN_KeyUp(&in_right);}
void IN_ForwardDown(void) {IN_KeyDown(&in_forward);}
void IN_ForwardUp(void) {IN_KeyUp(&in_forward);}
void IN_BackDown(void) {IN_KeyDown(&in_back);}
void IN_BackUp(void) {IN_KeyUp(&in_back);}
void IN_LookupDown(void) {IN_KeyDown(&in_lookup);}
void IN_LookupUp(void) {IN_KeyUp(&in_lookup);}
void IN_LookdownDown(void) {IN_KeyDown(&in_lookdown);}
void IN_LookdownUp(void) {IN_KeyUp(&in_lookdown);}
void IN_MoveleftDown(void) {IN_KeyDown(&in_moveleft);}
void IN_MoveleftUp(void) {IN_KeyUp(&in_moveleft);}
void IN_MoverightDown(void) {IN_KeyDown(&in_moveright);}
void IN_MoverightUp(void) {IN_KeyUp(&in_moveright);}

void IN_SpeedDown(void) {IN_KeyDown(&in_speed);}
void IN_SpeedUp(void) {IN_KeyUp(&in_speed);}
void IN_StrafeDown(void) {IN_KeyDown(&in_strafe);}
void IN_StrafeUp(void) {IN_KeyUp(&in_strafe);}

void IN_Button0Down(void) {IN_KeyDown(&in_buttons[0]);}
void IN_Button0Up(void) {IN_KeyUp(&in_buttons[0]);}
void IN_Button1Down(void) {IN_KeyDown(&in_buttons[1]);}
void IN_Button1Up(void) {IN_KeyUp(&in_buttons[1]);}
void IN_Button2Down(void) {IN_KeyDown(&in_buttons[2]);}
void IN_Button2Up(void) {IN_KeyUp(&in_buttons[2]);}
void IN_Button3Down(void) {IN_KeyDown(&in_buttons[3]);}
void IN_Button3Up(void) {IN_KeyUp(&in_buttons[3]);}
void IN_Button4Down(void) {IN_KeyDown(&in_buttons[4]);}
void IN_Button4Up(void) {IN_KeyUp(&in_buttons[4]);}
void IN_Button5Down(void) {IN_KeyDown(&in_buttons[5]);}
void IN_Button5Up(void) {IN_KeyUp(&in_buttons[5]);}
void IN_Button6Down(void) {IN_KeyDown(&in_buttons[6]);}
void IN_Button6Up(void) {IN_KeyUp(&in_buttons[6]);}
void IN_Button7Down(void) {IN_KeyDown(&in_buttons[7]);}
void IN_Button7Up(void) {IN_KeyUp(&in_buttons[7]);}

void IN_ButtonDown (void) {
	IN_KeyDown(&in_buttons[1]);}
void IN_ButtonUp (void) {
	IN_KeyUp(&in_buttons[1]);}

void IN_CenterView (void) {
	cl.viewangles[PITCH] = -SHORT2ANGLE(cl.frame.ps.delta_angles[PITCH]);
}


//==========================================================================

cvar_t	*cl_upspeed;
cvar_t	*cl_forwardspeed;
cvar_t	*cl_sidespeed;

cvar_t	*cl_yawspeed;
cvar_t	*cl_pitchspeed;

cvar_t	*cl_run;

cvar_t	*cl_anglespeedkey;



/*
================
CL_AdjustAngles

Moves the local angle positions
================
*/
void CL_AdjustAngles( void ) {
	float	speed;
	
	if ( in_speed.active ) {
		speed = 0.001f * cls.frametime * cl_anglespeedkey->value;
	} else {
		speed = 0.001f * cls.frametime;
	}

	if ( !in_strafe.active ) {
		cl.viewangles[YAW] -= speed*cl_yawspeed->value*CL_KeyState (&in_right);
		cl.viewangles[YAW] += speed*cl_yawspeed->value*CL_KeyState (&in_left);
	}

	cl.viewangles[PITCH] -= speed*cl_pitchspeed->value * CL_KeyState (&in_lookup);
	cl.viewangles[PITCH] += speed*cl_pitchspeed->value * CL_KeyState (&in_lookdown);
}

/*
================
CL_KeyMove

Sets the usercmd_t based on key states
================
*/
void CL_KeyMove( usercmd_t *cmd ) {
	int		movespeed;
	int		forward, side, up;

	//
	// adjust for speed key / running
	// the walking flag is to keep animations consistant
	// even during acceleration and develeration
	//
	if ( in_speed.active ^ cl_run->integer ) {
		movespeed = 127;
		cmd->buttons &= ~BUTTON_WALKING;
	} else {
		cmd->buttons |= BUTTON_WALKING;
		movespeed = 64;
	}

	forward = 0;
	side = 0;
	up = 0;
	if ( in_strafe.active ) {
		side += movespeed * CL_KeyState (&in_right);
		side -= movespeed * CL_KeyState (&in_left);
	}

	side += movespeed * CL_KeyState (&in_moveright);
	side -= movespeed * CL_KeyState (&in_moveleft);


	up += movespeed * CL_KeyState (&in_up);
	up -= movespeed * CL_KeyState (&in_down);

	forward += movespeed * CL_KeyState (&in_forward);
	forward -= movespeed * CL_KeyState (&in_back);

	cmd->forwardmove = ClampChar( forward );
	cmd->rightmove = ClampChar( side );
	cmd->upmove = ClampChar( up );
}

void _UI_MouseEvent( int dx, int dy );
void _UI_MouseEventAbs( int dx, int dy );

/*
=================
CL_MouseEvent
=================
*/
void CL_MouseEvent( int dx, int dy, int time ) {
	if ( cls.keyCatchers & KEYCATCH_UI ) {
		_UI_MouseEvent( dx, dy );
	}
	else {
		cl.mouseDx[cl.mouseIndex] += dx;
		cl.mouseDy[cl.mouseIndex] += dy;
	}
}


void CL_MouseEventAbs( int x, int y)
{
	if ( cls.keyCatchers & KEYCATCH_UI ) {
			_UI_MouseEventAbs( x, y );
	}
}

/*
=================
CL_JoystickEvent

Joystick values stay set until changed
=================
*/
void CL_JoystickEvent( int axis, int value, int time ) {
	if ( axis < 0 || axis >= MAX_JOYSTICK_AXIS ) {
		Com_Error( ERR_DROP, "CL_JoystickEvent: bad axis %i", axis );
	}
	cl.joystickAxis[axis] = value;
}

/*
=================
CL_JoystickMove
=================
*/

void CL_JoystickMove( usercmd_t *cmd ) {
	int		movespeed;
	float	anglespeed;

	if ( in_speed.active ^ cl_run->integer ) {
		movespeed = 2;
	} else {
		movespeed = 1;
		cmd->buttons |= BUTTON_WALKING;
	}

	if ( in_speed.active ) {
		anglespeed = 0.001f * cls.frametime * cl_anglespeedkey->value;
	} else {
		anglespeed = 0.001f * cls.frametime;
	}

	if ( !in_strafe.active ) {
		cl.viewangles[YAW] += anglespeed * (cl_yawspeed->value / 100.0f) * cl.joystickAxis[AXIS_SIDE];
	} else {
		cmd->rightmove = ClampChar( cmd->rightmove + cl.joystickAxis[AXIS_SIDE] );
	}

	if ( in_mlooking ) {
		cl.viewangles[PITCH] += anglespeed * (cl_pitchspeed->value / 100.0f) * cl.joystickAxis[AXIS_FORWARD];
	} else {
		cmd->forwardmove = ClampChar( cmd->forwardmove + cl.joystickAxis[AXIS_FORWARD] );
	}

	cmd->upmove = ClampChar( cmd->upmove + cl.joystickAxis[AXIS_UP] );
}

/*
=================
CL_MouseMove
=================
*/
#ifdef AUTOAIM
/*extern short *cg_pcrossHairStatus;
extern int *g_plastFireTime;*/
void CL_MouseClamp(int *x, int *y)
{
	float ax = Q_fabs(*x);
	float ay = Q_fabs(*y);

	ax = (ax-10)*(3.0f/45.0f) * (ax-10) * (Q_fabs(*x) > 10);
	ay = (ay-10)*(3.0f/45.0f) * (ay-10) * (Q_fabs(*y) > 10);
	if (*x < 0)
		*x = -ax;
	else
		*x = ax;
	if (*y < 0)
		*y = -ay;
	else
		*y = ay;
}
#endif
float cl_mPitchOverride = 0.0f;
float cl_mYawOverride = 0.0f;
void CL_MouseMove( usercmd_t *cmd ) {
	float	mx, my;
	float	accelSensitivity;
	float	rate;
	const float	speed = static_cast<float>(frame_msec);
	const float pitch = m_pitch->value;

#ifdef AUTOAIM
//	int g_lastFireTime = *g_plastFireTime;
	int g_lastFireTime = 0;
	short cg_crossHairStatus = 0;
	if (ge) {
		g_lastFireTime = ge->GetLastFireTime();
		cg_crossHairStatus = ge->GetCrossHairStatus();
	}
#endif
#ifdef AUTOAIM0
	const float mouseSpeedX = 0.06f;
	const float mouseSpeedY = 0.05f;

	// allow mouse smoothing
	if ( m_filter->integer ) {
		mx = ( cl.mouseDx[0] + cl.mouseDx[1] ) * 0.5f * frame_msec * mouseSpeedX;
		my = ( cl.mouseDy[0] + cl.mouseDy[1] ) * 0.5f * frame_msec * mouseSpeedY;
	} else {
		int ax = cl.mouseDx[cl.mouseIndex];
		int ay = cl.mouseDy[cl.mouseIndex];
//		CL_MouseClamp(&ax, &ay);
		
		mx = ax * speed * mouseSpeedX;	
		my = ay * speed * mouseSpeedY;		
	}

	const float m_hoverSensitivity = 0.4f;
	if (cg_crossHairStatus == 1)
	{
		mx *= m_hoverSensitivity;
		my *= m_hoverSensitivity;
	}
#else
	// allow mouse smoothing
	if ( m_filter->integer ) {
		mx = ( cl.mouseDx[0] + cl.mouseDx[1] ) * 0.5;
		my = ( cl.mouseDy[0] + cl.mouseDy[1] ) * 0.5;
	} else {
		mx = cl.mouseDx[cl.mouseIndex];
		my = cl.mouseDy[cl.mouseIndex];
	}
#ifdef AUTOAIM
	const float m_hoverSensitivity = 0.4f;
	if (cg_crossHairStatus == 1)
	{
		mx *= m_hoverSensitivity;
		my *= m_hoverSensitivity;
	}
#endif	
#endif
	cl.mouseIndex ^= 1;
	cl.mouseDx[cl.mouseIndex] = 0;
	cl.mouseDy[cl.mouseIndex] = 0;

	rate = sqrtf( mx * mx + my * my ) / (float)frame_msec;
	accelSensitivity = cl_sensitivity->value + rate * cl_mouseAccel->value;

	// scale by FOV
	accelSensitivity *= cl.cgameSensitivity;

	if ( rate && cl_showMouseRate->integer ) {
		Com_Printf( "%f : %f\n", rate, accelSensitivity );
	}

	mx *= accelSensitivity;
	#ifdef PANDORA
	my *= accelSensitivity*0.75f;	// lower acceleration on Y axis
	#else
	my *= accelSensitivity;
	#endif

	if (!mx && !my) {
#ifdef AUTOAIM
		// If there was a movement but no change in angles then start auto-leveling the camera
		float autolevelSpeed = 0.03f;

		if (cg_crossHairStatus != 1 &&							// Not looking at an enemy
			cl.joystickAxis[AXIS_FORWARD] &&					// Moving forward/backward
			cl.frame.ps.groundEntityNum != ENTITYNUM_NONE &&	// Not in the air
			Cvar_VariableIntegerValue("cl_autolevel") &&		// Autolevel is turned on
			g_lastFireTime < Sys_Milliseconds() - 1000)			// Haven't fired recently
		{
			float normAngle = -SHORT2ANGLE(cl.frame.ps.delta_angles[PITCH]);
			// The adjustment to normAngle below is meant to add or remove some multiple
			// of 360, so that normAngle is within 180 of viewangles[PITCH]. It should
			// be correct.
			int diff = (int)(cl.viewangles[PITCH] - normAngle);
			if (diff > 180)
				normAngle += 360.0f * ((diff+180) / 360);
			else if (diff < -180)
				normAngle -= 360.0f * ((-diff+180) / 360);

			if (Cvar_VariableIntegerValue("cg_thirdperson") == 1)
			{
//				normAngle += 10;	// Removed by BTO, 2003/05/14, I hate it
				autolevelSpeed *= 1.5f;
			}
			if (cl.viewangles[PITCH] > normAngle)
			{
				cl.viewangles[PITCH] -= autolevelSpeed * speed;
				if (cl.viewangles[PITCH] < normAngle) cl.viewangles[PITCH] = normAngle;
			}
			else if (cl.viewangles[PITCH] < normAngle)
			{
				cl.viewangles[PITCH] += autolevelSpeed * speed;
				if (cl.viewangles[PITCH] > normAngle) cl.viewangles[PITCH] = normAngle;
			}
		}
#endif
		return;
	}

	// add mouse X/Y movement to cmd
	if ( in_strafe.active ) {
		cmd->rightmove = ClampChar( cmd->rightmove + m_side->value * mx );
	} else {
		if ( cl_mYawOverride )
		{
			cl.viewangles[YAW] -= cl_mYawOverride * mx;
		}
		else
		{
			cl.viewangles[YAW] -= m_yaw->value * mx;
		}
	}

	if ( (in_mlooking || cl_freelook->integer) && !in_strafe.active ) {
#ifdef AUTOAIM
		const float cl_pitchSensitivity = 0.5f;
#else
		const float cl_pitchSensitivity = 1.0f;
#endif
		if ( cl_mPitchOverride )
		{
			if ( m_pitch->value > 0 )
			{
				cl.viewangles[PITCH] += cl_mPitchOverride * my * cl_pitchSensitivity;
			}
			else
			{
				cl.viewangles[PITCH] -= cl_mPitchOverride * my * cl_pitchSensitivity;
			}
		}
		else
		{
			cl.viewangles[PITCH] += m_pitch->value * my * cl_pitchSensitivity;
		}
	} else {
		cmd->forwardmove = ClampChar( cmd->forwardmove - m_forward->value * my );
	}
}


/*
==============
CL_CmdButtons
==============
*/
void CL_CmdButtons( usercmd_t *cmd ) {
	int		i;

	//
	// figure button bits
	// send a button bit even if the key was pressed and released in
	// less than a frame
	//	
	for (i = 0 ; i < 8 ; i++) {
		if ( in_buttons[i].active || in_buttons[i].wasPressed ) {
			cmd->buttons |= 1 << i;
		}
		in_buttons[i].wasPressed = qfalse;
	}

	if ( cls.keyCatchers ) {
		//cmd->buttons |= BUTTON_TALK;
	}

	// allow the game to know if any key at all is
	// currently pressed, even if it isn't bound to anything
	if ( kg.anykeydown && !cls.keyCatchers ) {
		cmd->buttons |= BUTTON_ANY;
	}
}


/*
==============
CL_FinishMove
==============
*/
void CL_FinishMove( usercmd_t *cmd ) {
	int		i;

	// copy the state that the cgame is currently sending
	cmd->weapon = cl.cgameUserCmdValue;

	// send the current server time so the amount of movement
	// can be determined without allowing cheating
	cmd->serverTime = cl.serverTime;

	for (i=0 ; i<3 ; i++) {
		cmd->angles[i] = ANGLE2SHORT(cl.viewangles[i]);
	}
}

#ifdef __ANDROID__
void CL_AndroidMove( usercmd_t *cmd );

int crc_check_count = 0;
int crc_failed = 0;
extern "C"
{
extern int self_crc_check(const char * file);
extern const char * getLibPath();
}
#endif
/*
=================
CL_CreateCmd
=================
*/
vec3_t cl_overriddenAngles = {0,0,0};
qboolean cl_overrideAngles = qfalse;
usercmd_t CL_CreateCmd( void ) {
	usercmd_t	cmd;
	vec3_t		oldAngles;

	VectorCopy( cl.viewangles, oldAngles );

	// keyboard angle adjustment
	CL_AdjustAngles ();
	
	memset( &cmd, 0, sizeof( cmd ) );

	CL_CmdButtons( &cmd );

	// get basic movement from keyboard
	CL_KeyMove (&cmd);

	// get basic movement from mouse
	CL_MouseMove( &cmd );

	// get basic movement from joystick
	CL_JoystickMove( &cmd );

#ifdef __ANDROID__
		CL_AndroidMove ( &cmd );
#endif
	// check to make sure the angles haven't wrapped
	if ( cl.viewangles[PITCH] - oldAngles[PITCH] > 90 ) {
		cl.viewangles[PITCH] = oldAngles[PITCH] + 90;
	} else if ( oldAngles[PITCH] - cl.viewangles[PITCH] > 90 ) {
		cl.viewangles[PITCH] = oldAngles[PITCH] - 90;
	} 

	if ( cl_overrideAngles )
	{
		VectorCopy( cl_overriddenAngles, cl.viewangles );
		cl_overrideAngles = qfalse;
	}
	// store out the final values
	CL_FinishMove( &cmd );

	// draw debug graphs of turning for mouse testing
	if ( cl_debugMove->integer ) {
		if ( cl_debugMove->integer == 1 ) {
			SCR_DebugGraph( fabs(cl.viewangles[YAW] - oldAngles[YAW]), 0 );
		}
		if ( cl_debugMove->integer == 2 ) {
			SCR_DebugGraph( fabs(cl.viewangles[PITCH] - oldAngles[PITCH]), 0 );
		}
	}

	return cmd;
}


/*
=================
CL_CreateNewCommands

Create a new usercmd_t structure for this frame
=================
*/
void CL_CreateNewCommands( void ) {
	usercmd_t	*cmd;
	int			cmdNum;

	// no need to create usercmds until we have a gamestate
//	if ( cls.state < CA_PRIMED ) {
//		return;
//	}

	frame_msec = com_frameTime - old_com_frameTime;

	// if running less than 5fps, truncate the extra time to prevent
	// unexpected moves after a hitch
	if ( frame_msec > 200 ) {
		frame_msec = 200;
	}
	old_com_frameTime = com_frameTime;


	// generate a command for this frame
	cl.cmdNumber++;
	cmdNum = cl.cmdNumber & CMD_MASK;
	cl.cmds[cmdNum] = CL_CreateCmd ();
	cmd = &cl.cmds[cmdNum];
}

/*
=================
CL_ReadyToSendPacket

Returns qfalse if we are over the maxpackets limit
and should choke back the bandwidth a bit by not sending
a packet this frame.  All the commands will still get
delivered in the next packet, but saving a header and
getting more delta compression will reduce total bandwidth.
=================
*/
qboolean CL_ReadyToSendPacket( void ) {
	// don't send anything if playing back a demo
//	if ( cls.state == CA_CINEMATIC ) 
	if ( cls.state == CA_CINEMATIC || CL_IsRunningInGameCinematic())
	{
		return qfalse;
	}

	// if we don't have a valid gamestate yet, only send
	// one packet a second
	if ( cls.state != CA_ACTIVE && cls.state != CA_PRIMED
		&& cls.realtime - clc.lastPacketSentTime < 1000 ) {
		return qfalse;
	}

	// send every frame for loopbacks
	return qtrue;
}

/*
===================
CL_WritePacket

Create and send the command packet to the server
Including both the reliable commands and the usercmds

During normal gameplay, a client packet will contain something like:

4	sequence number
2	qport
4	serverid
4	acknowledged sequence number
4	clc.serverCommandSequence
<optional reliable commands>
1	clc_move or clc_moveNoDelta
1	command count
<count * usercmds>

===================
*/
void CL_WritePacket( void ) {
	msg_t		buf;
	byte		data[MAX_MSGLEN];
	int			i, j;
	usercmd_t	*cmd, *oldcmd;
	usercmd_t	nullcmd;
	int			packetNum;
	int			oldPacketNum;
	int			count;

	// don't send anything if playing back a demo
//	if ( cls.state == CA_CINEMATIC ) 
	if ( cls.state == CA_CINEMATIC || CL_IsRunningInGameCinematic())
	{
		return;
	}

	MSG_Init( &buf, data, sizeof(data) );

	// write any unacknowledged clientCommands
	for ( i = clc.reliableAcknowledge + 1 ; i <= clc.reliableSequence ; i++ ) {
		MSG_WriteByte( &buf, clc_clientCommand );
		MSG_WriteLong( &buf, i );
		MSG_WriteString( &buf, clc.reliableCommands[ i & (MAX_RELIABLE_COMMANDS-1) ] );
	}

	// we want to send all the usercmds that were generated in the last
	// few packet, so even if a couple packets are dropped in a row,
	// all the cmds will make it to the server
	if ( cl_packetdup->integer < 0 ) {
		Cvar_Set( "cl_packetdup", "0" );
	} else if ( cl_packetdup->integer > 5 ) {
		Cvar_Set( "cl_packetdup", "5" );
	}
	oldPacketNum = (clc.netchan.outgoingSequence - 1 - cl_packetdup->integer) & PACKET_MASK;
	count = cl.cmdNumber - cl.packetCmdNumber[ oldPacketNum ];
	if ( count > MAX_PACKET_USERCMDS ) {
		count = MAX_PACKET_USERCMDS;
		Com_Printf("MAX_PACKET_USERCMDS\n");
	}
	if ( count >= 1 ) {
		// begin a client move command
		MSG_WriteByte (&buf, clc_move);

		// write the last reliable message we received
		MSG_WriteLong( &buf, clc.serverCommandSequence );

		// write the current serverId so the server
		// can tell if this is from the current gameState
		MSG_WriteLong (&buf, cl.serverId);

		// write the current time
		MSG_WriteLong (&buf, cls.realtime);

		// let the server know what the last messagenum we
		// got was, so the next message can be delta compressed
		// FIXME: this could just be a bit flag, with the message implicit
		// from the unreliable ack of the netchan
		if (cl_nodelta->integer || !cl.frame.valid) {
			MSG_WriteLong (&buf, -1);	// no compression
		} else {
			MSG_WriteLong (&buf, cl.frame.messageNum);
		}

		// write the cmdNumber so the server can determine which ones it
		// has already received
		MSG_WriteLong( &buf, cl.cmdNumber );

		// write the command count
		MSG_WriteByte( &buf, count );

		// write all the commands, including the predicted command
		memset( &nullcmd, 0, sizeof(nullcmd) );
		oldcmd = &nullcmd;
		for ( i = 0 ; i < count ; i++ ) {
			j = (cl.cmdNumber - count + i + 1) & CMD_MASK;
			cmd = &cl.cmds[j];
			MSG_WriteDeltaUsercmd (&buf, oldcmd, cmd);
			oldcmd = cmd;
		}
	}

	//
	// deliver the message
	//
	packetNum = clc.netchan.outgoingSequence & PACKET_MASK;
	cl.packetTime[ packetNum ] = cls.realtime;
	cl.packetCmdNumber[ packetNum ] = cl.cmdNumber;
	clc.lastPacketSentTime = cls.realtime;
	Netchan_Transmit (&clc.netchan, buf.cursize, buf.data);	
}

/*
=================
CL_SendCmd

Called every frame to builds and sends a command packet to the server.
=================
*/
void CL_SendCmd( void ) {
	// don't send any message if not connected
	if ( cls.state < CA_CONNECTED ) {
		return;
	}

	// don't send commands if paused
	if ( com_sv_running->integer && sv_paused->integer && cl_paused->integer ) {
		return;
	}

	// we create commands even if a demo is playing,
	CL_CreateNewCommands();

	// don't send a packet if the last packet was sent too recently
	if ( !CL_ReadyToSendPacket() ) {
		return;
	}

	CL_WritePacket();
}




/*
============
CL_InitInput
============
*/
void CL_InitInput( void ) {
	Cmd_AddCommand ("centerview",IN_CenterView);

	Cmd_AddCommand ("+moveup",IN_UpDown);
	Cmd_AddCommand ("-moveup",IN_UpUp);
	Cmd_AddCommand ("+movedown",IN_DownDown);
	Cmd_AddCommand ("-movedown",IN_DownUp);
	Cmd_AddCommand ("+left",IN_LeftDown);
	Cmd_AddCommand ("-left",IN_LeftUp);
	Cmd_AddCommand ("+right",IN_RightDown);
	Cmd_AddCommand ("-right",IN_RightUp);
	Cmd_AddCommand ("+forward",IN_ForwardDown);
	Cmd_AddCommand ("-forward",IN_ForwardUp);
	Cmd_AddCommand ("+back",IN_BackDown);
	Cmd_AddCommand ("-back",IN_BackUp);
	Cmd_AddCommand ("+lookup", IN_LookupDown);
	Cmd_AddCommand ("-lookup", IN_LookupUp);
	Cmd_AddCommand ("+lookdown", IN_LookdownDown);
	Cmd_AddCommand ("-lookdown", IN_LookdownUp);
	Cmd_AddCommand ("+strafe", IN_StrafeDown);
	Cmd_AddCommand ("-strafe", IN_StrafeUp);
	Cmd_AddCommand ("+moveleft", IN_MoveleftDown);
	Cmd_AddCommand ("-moveleft", IN_MoveleftUp);
	Cmd_AddCommand ("+moveright", IN_MoverightDown);
	Cmd_AddCommand ("-moveright", IN_MoverightUp);
	Cmd_AddCommand ("+speed", IN_SpeedDown);
	Cmd_AddCommand ("-speed", IN_SpeedUp);
	//buttons
	Cmd_AddCommand ("+attack", IN_Button0Down);//attack
	Cmd_AddCommand ("-attack", IN_Button0Up);
	Cmd_AddCommand ("+force_lightning", IN_Button1Down);//force lightning
	Cmd_AddCommand ("-force_lightning", IN_Button1Up);
	Cmd_AddCommand ("+useforce", IN_Button2Down);	//use current force power
	Cmd_AddCommand ("-useforce", IN_Button2Up);
	Cmd_AddCommand ("+block", IN_Button3Down);//manual saber block
	Cmd_AddCommand ("-block", IN_Button3Up);
	Cmd_AddCommand ("+walk", IN_Button4Down);//walking
	Cmd_AddCommand ("-walk", IN_Button4Up);
	Cmd_AddCommand ("+use", IN_Button5Down);//use object
	Cmd_AddCommand ("-use", IN_Button5Up);
	Cmd_AddCommand ("+force_grip", IN_Button6Down);//force jump
	Cmd_AddCommand ("-force_grip", IN_Button6Up);
	Cmd_AddCommand ("+altattack", IN_Button7Down);//altattack
	Cmd_AddCommand ("-altattack", IN_Button7Up);
	//generic button commands
	Cmd_AddCommand ("+button0", IN_Button0Down);//attack
	Cmd_AddCommand ("-button0", IN_Button0Up);
	Cmd_AddCommand ("+button1", IN_Button1Down);//force lightning
	Cmd_AddCommand ("-button1", IN_Button1Up);
	Cmd_AddCommand ("+button2", IN_Button2Down);//use holdable (not used - change to use jedi power?)
	Cmd_AddCommand ("-button2", IN_Button2Up);
	Cmd_AddCommand ("+button3", IN_Button3Down);//manual saber block
	Cmd_AddCommand ("-button3", IN_Button3Up);
	Cmd_AddCommand ("+button4", IN_Button4Down);//walking
	Cmd_AddCommand ("-button4", IN_Button4Up);
	Cmd_AddCommand ("+button5", IN_Button5Down);//use object
	Cmd_AddCommand ("-button5", IN_Button5Up);
	Cmd_AddCommand ("+button6", IN_Button6Down);//force grip
	Cmd_AddCommand ("-button6", IN_Button6Up);
	Cmd_AddCommand ("+button7", IN_Button7Down);//altattack
	Cmd_AddCommand ("-button7", IN_Button7Up);
	//end buttons
	Cmd_AddCommand ("+mlook", IN_MLookDown);
	Cmd_AddCommand ("-mlook", IN_MLookUp);

	cl_nodelta = Cvar_Get ("cl_nodelta", "0", 0);
	cl_debugMove = Cvar_Get ("cl_debugMove", "0", 0);
}
