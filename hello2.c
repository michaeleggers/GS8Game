#include <acknex.h>
#include <default.c>

#define WALK_SPEED        50.0
#define RUN_SPEED         20.0
#define SLOW_WALK_SPEED   WALK_SPEED/2.0
#define MOUSE_SENSITIVITY 10
#define ACCELERATION      2.0
#define MAX_ACCELERATION  6.0
#define MAX_ROLL_DEG      2.0
#define ROLL_DEG_INC      0.5
#define CAM_WAVE_INC      0.5
#define CAM_WAVE_STRENGTH 5.0

#define PLAYER_HEIGHT     100

// Define the cube entity
ENTITY* cube;

float g_Velocity     = 0.0;
float g_VelocitySide = 0.0;
int   g_ForwardDir   = 0;
float g_RollDeg      = 0.0;
float g_CamWave      = 0.0;
float g_SaveCamZ     = 150.0;
int   g_CamMode      = 1;

TEXT* txtPlayerName =
{
   strings = 1;
   flags = CENTER_X;
   font = "Arial#30bi";
}



//function ShowDebugInfo(STRING* sstr, STRING* str, float x, float y)
//{
//	STRING* s = "                                                    ";
//	str_cat(s, sstr);
//	str_cat(s, str);
//  	VECTOR* vTextpos = vector(x, y, 0); 
//  
//  	txtPlayerName.pos_x = vTextpos.x; // set the position
//  	txtPlayerName.pos_y = vTextpos.y;
//  	str_cpy((txtPlayerName.pstring)[0], s); // set the TEXT string
//  	draw_obj(txtPlayerName);
//}

function ShowDebugInfo(STRING* str1, STRING* str2, float x, float y) {
	draw_text(str1, x, y, vector(255, 255, 255));	
	draw_text(str2, x + 200, y, vector(255, 255, 255));
}

float g_SpeedZ;

action MovePlayer() {
	
	float nTraceDown = 0;
	
	VECTOR vecPlayerMoveSpeed;
	VECTOR* newAngle = NULL;
	VECTOR* newPos   = NULL;


   // Access the model's mesh
//   BMAP* modelBmap = bmap_for_entity(my, 0);	
	// Loop through all vertices and swap X and Y positions	
//	var i = ent_status(my, 0);
//   for (; i > 0; i--)
//   {
//   	CONTACT* c = ent_getvertex(my, NULL, i);
//   	float saveX = c.v.x;
//   	c.v.x = c.v.z;
//   	c.v.z = saveX;
//   	ent_setvertex(my, c, i);
//   }
   

	while (1) {
	
		nTraceDown = c_trace(my.x, vector(my.x, my.y, my.z - 1000), // high value so we always hit something
						IGNORE_ME | IGNORE_PASSENTS | IGNORE_PASSABLE |IGNORE_SPRITES | USE_BOX);
					
		ShowDebugInfo("nTraceDown: ", str_for_float(NULL, nTraceDown), 100, 100);		
		ShowDebugInfo("my.min_z: ", str_for_float(NULL, my.min_z), 100, 120);		
		ShowDebugInfo("my.z: ", str_for_float(NULL, my.z), 100, 140);				
		ShowDebugInfo("hit.z: ", str_for_float(NULL, hit.z), 100, 160);		
		
		vecPlayerMoveSpeed.z = 0;
		if (nTraceDown > 5) { // Hit some geometry, still falling?
			float distZ = abs(my.z - hit.z); // my.z should always be larger than hit.z! 
			if (distZ > 1) {
				distZ = 1;
			}
			ShowDebugInfo("distZ: ", str_for_float(NULL, distZ), 100, 180);		
			vecPlayerMoveSpeed.z = -distZ;
			g_SpeedZ = vecPlayerMoveSpeed.z;
		}
	 	else if (nTraceDown == 0) { // Nothing hit or stuck inside something
	 		if (g_SpeedZ < 0) { // Player was falling in prev frame
	 			vecPlayerMoveSpeed.z += 3.0; // Move up a bit and check again next frame.	 			
	 			g_SpeedZ = 0;
 			} else {
 				vecPlayerMoveSpeed.z = -1;	
 			}	 		
 		}
 		else if (nTraceDown < 0) { // Definitely stuck inside something
 			vecPlayerMoveSpeed.z += 5;
 			g_SpeedZ = 0;
 		}


		//if (nTraceDown > 0) { // No poly hit according to docs
//			VECTOR* vGravity = vector(0, 0, -(nTraceDown+my.min_z));
//			c_move(my, vGravity, nullvector,
//					IGNORE_PASSABLE | IGNORE_PASSENTS | GLIDE);						
		//}



		
		//vecPlayerMoveSpeed.z = 0;
		
		// Berechnung wie schnell der Spieler sich bewegt. Wenn wir die linke
		// Shifttaste gedr?ckt haben, addieren wir zur Bewegungsgeschwindigkeit
		// den Wert von "RUN_SPEED" um zu rennen.
		vecPlayerMoveSpeed.x = (key_w - key_s) *
			(WALK_SPEED+key_shiftl*RUN_SPEED) * time_step;

		vecPlayerMoveSpeed.y = (key_a - key_d) * WALK_SPEED * time_step;
			
		// Hier drehen wir den Spieler mit der Maus nach links und rechts. Die
		// Drehgeschwindigkeit kann mit dem Define "MOUSE_SENSITIVITY"
		// beeinflusst werden.
		my.pan -= mouse_force.x * time_step*MOUSE_SENSITIVITY;

	
		
		float distCovered = c_move(my, vecPlayerMoveSpeed.x, nullvector,
				IGNORE_PASSABLE | IGNORE_PASSENTS | GLIDE);
		

		

		//vec_add(player.x, vecPlayerMoveSpeed);
	

				
		wait(1);
	}
	
	
}

function MoveCamera() {

	while (1) {
		
		camera.pan = player.pan;
		
		camera.tilt = clamp(
			camera.tilt+mouse_force.y*time_step*MOUSE_SENSITIVITY,
			-90,
			90
		);
		
		vec_set(camera.x, vector(player.x, player.y, player.z + PLAYER_HEIGHT));
		
		if (key_1) {
			g_CamMode = 1;
		}  
		else if (key_2) {
			g_CamMode = 2;	
		}
		ShowDebugInfo("g_CamMode: ", str_for_num(NULL, g_CamMode), 100, 200);
		
		if (g_CamMode == 1) {
			float x = 300*cos(player.pan);	
			float y = 300*sin(player.pan);
			ShowDebugInfo("x: ", str_for_num(NULL, x), 100, 220);
			ShowDebugInfo("y: ", str_for_num(NULL, y), 100, 240);
			camera.x -= x;
			camera.y -= y;
			//camera.z += 100;
			camera.z = 2.0;
			
		} else if (g_CamMode == 2) {
			camera.x = player.x;	
		}
		
		
		
		wait(1);
	}

//	while(1) {
	
		
		
		// Cam wave
		
//		float sinAt = abs(sin(g_CamWave));
//		//sinAt = smooth(&sinAt, 0.09);
//		camera.z = g_SaveCamZ + CAM_WAVE_STRENGTH*sinAt;		
//		g_RollDeg = clamp(g_RollDeg, -MAX_ROLL_DEG, MAX_ROLL_DEG);
//		camera.roll = g_RollDeg;
//		//vec_set(camera.x, newPos);
		
				
//		wait(1);
//	}
}

action ActPlayer() {
	player = me;
	me.scale_x *= 0.25;
	me.scale_y *= 0.25;
	me.scale_z *= 0.25;
 	//set(me, INVISIBLE);
 	c_setminmax(me);
 	wait(1);
 	//player.eflags |= NARROW;
 	my.min_z += 5;
 	my.min_y += 30;
 	my.max_y -= 30;
 	my.max_x += 20;
 	my.min_x -= 20;
 	
 	//vec_set(my.min_x, vector(my.min_x, my.min_y, my.));
 	//vec_fill(my.min_z,-2);
	//vec_fill(my.max_z,2);	   
   //me.min_z -= 10;
   //me.min_z += 100;
   
 	MovePlayer();
 	MoveCamera();	
}

action RotateEntity() {
	while (1) {
		
		if (key_cuu) {
    		cube.z += 30*time_step;
    	}
    	if (key_cud) {
    		cube.z -= 30*time_step;
    	}
    	if (key_cur) {
    		cube.y += 30*time_step;
    	}
    	if (key_cul) {
    		cube.y -= 30*time_step;
    	}
    	
    	if (key_brackr) {
    		cube.scale_x += 1.1*time_step;
    		cube.scale_y += 1.1*time_step;
    		cube.scale_z += 1.1*time_step;
    	}
    	if (key_brackl) {
    		cube.scale_x -= 1.1*time_step;
    		cube.scale_y -= 1.1*time_step;
    		cube.scale_z -= 1.1*time_step;
    	}  
    	
		// Rotate the cube around the Y-axis
	   my.pan += 5 * time_step; // Adjust the rotation speed as needed
	   my.tilt += 2 * time_step;
	    
	   wait(-0.005);
    }
}

// Initialize the game
void main()
{
    video_mode = 12; // Set the video mode (adjust as needed)
    
    level_load("simple_map.wmb"); // Load an empty level

	//camera->flags |= SHOW;
	//camera->x = -1500;
	//camera->z = g_SaveCamZ;	
	    
    // Create the cube
    VECTOR* pos = vector(-100, 0, 150);
    cube = ent_create("blob.mdl", pos, RotateEntity);
    cube.scale_x = 5.0;
    cube.scale_y = 5.0;
    cube.scale_z = 5.0;
    
    // Create the player
    VECTOR* playerPos = vector(-1000, -500, 800);
    ent_create("rain_with_idle_anim.mdl", playerPos, ActPlayer);
    //ent_create("player.mdl", playerPos, ActPlayer);
    
	    	 

    
}





